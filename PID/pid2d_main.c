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

#include "../include/pid_source.h"
#include "../include/pid_params.h"

#define DEV_PATH  "/dev/PWM_OUT"
#define SERVOSEM "/servosemaphore"
#define POSITSEM "/position_sem"
#define SHM_SIZE 4096


struct pid_controller throttle_ctrldata;
struct pid_controller steering_ctrldata;

struct coordinate location;
struct coordinate setpoint;

pid_ct throttle_controller;
pid_ct steering_controller;

bool run, throttle_active, steer_active;

// Initial p, i, d values will be refined by actual live testing.. will tune these params from live testing
float throttle_kp = 0.2, throttle_ki = 0, throttle_kd = 0;
float steering_kp = 0.2, steering_ki = 0, steering_kd = 0;

// Variables for PID to access, updated from shared mem..
float throttle_output = 0, steer_output = 0;


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

	struct throttle_steer *actuation_vals = (struct throttle_steer *)servo_ptr;
	struct pid_params *pid_inputs = (struct pid_params *)pos_ptr;

	location = pid_inputs->location; setpoint = pid_inputs->setpoint;

	/*                 Initializing PID controllers               */
	throttle_controller = pid_create(&throttle_ctrldata, &location, &throttle_output, &setpoint, throttle_kp,
	                                 throttle_ki, throttle_kd, false);
	steering_controller = pid_create(&steering_ctrldata, &location, &steer_output, &setpoint, steering_kp,
	                                 steering_ki, steering_kd, true);

	pid_auto(throttle_controller); pid_auto(steering_controller);
	pid_change_limits(throttle_controller, -1, 1); pid_change_limits(steering_controller, -1, 1);

	// FIX ??
	char t, s;
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
	printf("Steering: %d Throttle: %d \n", throttle_active, steer_active);

	/* Main Loop to be run continuously controlling throttle output on Race-car */
	run = true; // hard-coding to true. eventually some control could switch
	while (run) {
		sem_wait(servo_sem);
		location = pid_inputs->setpoint;
		setpoint = pid_inputs->setpoint;

		if(throttle_active){
			pid_compute(throttle_controller);
			actuation_vals->throttle = throttle_output;
		}
		if(steer_active){
			pid_compute(steering_controller);
			actuation_vals->steer = steer_output;
		}

		// Update output throttle & steering to shared memory
		sem_post(servo_sem);
		printf("Distance from dest: %f. Angle difference: %f", throttle_controller->current_err, steering_controller->current_err);
		printf("Throttle: %f , Steering: %f ", throttle_output, steer_output);
	}
	return 0;
}