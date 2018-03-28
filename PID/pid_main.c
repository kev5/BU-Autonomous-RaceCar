//
// Created by Nicholas Arnold.
//

#include "pid_source.h"
#include "setpoint.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#define DEV_PATH  "/dev/PWM_OUT"
#include <stdlib.h>
#include "../actuation/shared_def.h"
#include <semaphore.h>
#include "math.h"

#define SERVOSEM "/servosemaphore"
#define POSITSEM "/position_sem"
#define SHM_SIZE 4096


struct pid_controller throttle_ctrldata;
struct pid_controller steering_ctrldata;

pid_ct throttle_controller;
pid_ct steering_controller;

float throttle_input = 0, throttle_output = 0, steer_input = 0, steer_output = 0;
bool pid_active;

// Initial p, i, d values will be refined by actual live testing.. will tune these params from live testing
float throttle_kp = 6, throttle_ki = 3, throttle_kd = 2;
float steering_kp = 6; steering_ki = 3, steering_kd = 2;

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
	if (servo_fid |pos_fid == -1){
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

	float distance = (float) pow(pow(current_dest->x_pos,2) + pow(current_dest->y_pos,2),0.5);
	float angle = current_dest->angle;

	pid_active = true; // hard-coding to true. eventually some control could switch

	setpoint = 0;
	setpoint_ang = 0;

	// Initializing PID controllers
	throttle_controller = pid_create(&throttle_ctrldata, &throttle_input, &throttle_output, &setpoint , throttle_kp, throttle_ki, throttle_kd);
	steering_controller = pid_create(&steering_ctrldata, &steer_input, &steer_output, &setpoint_ang, steering_kp, steering_ki, steering_kd);
	pid_auto(throttle_controller);
	pid_auto(steering_controller);
	pid_change_limits(throttle_controller, -1, 1);
	pid_change_limits(steering_controller, -1, 1);

	/* Main Loop to be run continuously controlling throttle output on Race-car */
	while (pid_active) {
		if (pid_need_compute(throttle_controller)) { // time check to see if needed to update PID
			sem_wait(servo_sem);
			distance = (float) pow(pow(current_dest->x_pos,2) + pow(current_dest->y_pos,2),0.5);
			throttle_input = distance; // function to get current location from sensing

			// Compute new result from PID
			pid_compute(throttle_controller);
			pid_compute(steering_controller);

			// Update output throttle to shared memory
			moveptr->steer = steer_output;
			moveptr->throttle = throttle_output;
			sem_post(servo_sem);
		}
		printf("Throttle: %f , Steering: %f \n",throttle_output, steer_output);
	}
}