//
// Created by Nicholas Arnold.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/shared_def.h"
#include "pid_source.h"
#include "setpoint.h"

#define DEV_PATH  "/dev/PWM_OUT"
#define SERVOSEM "/servosemaphore"
#define POSITSEM "/position_sem"
#define SHM_SIZE 4096

struct pid_controller throttle_ctrldata;
struct pid_controller steering_ctrldata;

pid_ct throttle_controller;
pid_ct steering_controller;

bool run, throttle_active, steer_active;

// Initial p, i, d values will be refined by actual live testing.. will tune these params from live testing
float throttle_input = 0, throttle_output = 0; steer_input = 0, steer_output = 0;
float throttle_kp = 0.2, throttle_ki = 0, throttle_kd = 0;
float steering_kp = 1; steering_ki = 0, steering_kd = 0;

float setpoint;
float setpoint_ang;

int main() {
	/* Initializing interaction with shared mem (based off loothrottle_king @ actuation files) */
	sem_t *servo_sem = sem_open(SERVOSEM, 1);
	sem_t *pos_sem = sem_open(POSITSEM,1);
	if (servo_sem == SEM_FAILED | pos_sem == SEM_FAILED){
		perror("semaphore open error in userinput\n");
	}

	int servo_fid = shm_open("racecar", O_CREAT|O_RDWR, 0666);
	int pos_fid = shm_open("position", O_CREAT|O_RDWR, 2000);
	if (servo_fid == -1 |pos_fid == -1){
		perror("shm_open error\n");
		return -1;
	}

	void *servo_ptr = mmap(NULL, SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, servo_fid, 0);
	void *pos_ptr = mmap(NULL,SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED,pos_fid,0);
	if (servo_ptr == MAP_FAILED | pos_ptr == MAP_FAILED){
		perror("mmap error\n");
		exit(-1);
	}

	struct throttle_steer *moveptr = (struct throttle_steer *)servo_ptr;
	struct setpoint *current_dest = (struct setpoint *)pos_ptr;
	
	printf("Current X = %f ", current_dest->x_pos); 
	printf("Current Y = %f ", current_dest->y_pos); 
	printf("Theta = %f ", current_dest->angle); 
	printf("Set X = %f ", current_dest->x_set); 
	printf("Set Y = %f /n ", current_dest->y_set); 
	float distance = pow(pow(current_dest->x_pos - current_dest->x_set,2) + pow(current_dest->y_pos - current_dest->y_set,2),0.5);
	float angle = current_dest->angle;

	setpoint = 4;                
	setpoint_ang = 0;

	// Initializing PID controllers
	throttle_controller = pid_create(&throttle_ctrldata, &throttle_input, &throttle_output, &setpoint , throttle_kp, throttle_ki, throttle_kd);
	steering_controller = pid_create(&steering_ctrldata, &steer_input, &steer_output, &setpoint_ang, steering_kp, steering_ki, steering_kd);
	pid_auto(throttle_controller);
	pid_auto(steering_controller);
	pid_change_limits(throttle_controller, -1, 1);
	pid_change_limits(steering_controller, -1, 1);

	char t, s;
	// FIX ??  
	throttle_active = false;
	steer_active = false;
	printf("Throttle PID active? (y/n): ");
	scanf("%c", &t);
	getchar();
	printf("Steering PID active? (y/n): ");
	scanf("%c", &s);
	getchar();
	if (t == 'y') {
		throttle_active = true;
	}
	if (s == 'y') {
		steer_active = true;
	}
	printf("Steering: %d Throttle: %d\n", throttle_active, steer_active);

	/* Main Loop to be run continuously controlling throttle output on Race-car */
	run = true; // hard-coding to true. eventually some control could switch
	while (run) {
		if (true || pid_need_compute(throttle_controller)) { // time check to see if needed to update PID
			printf("Throttle: %f , Steering: 0 ",throttle_output);
			printf("Angle: %f \n", current_dest->angle);
			sem_wait(servo_sem);
			if(false){
			  distance = current_dest->x_set - current_dest->x_pos;
				printf("Error: %f \n", distance); 
				throttle_input = current_dest->x_pos;
				pid_compute(throttle_controller);

				moveptr->throttle = throttle_output;
			}

			if(true){
				steer_input = current_dest->angle;
				pid_compute(steering_controller);
				moveptr->steer = steer_output;
			}

			// Update output throttle to shared memory

			sem_post(servo_sem);
 
		}
	}
	return 0;
}
