//
// Created by Nicholas Arnold.
//

// System level includes
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>

// Project file level includes
#include "../include/pid_source.h"
#include "../include/pid_params.h"
#include "../include/shared_def.h"

// Shared memory includes
#define SERVOSEM "/servosemaphore"
#define POSITSEM "/position_sem"
#define SHM_SIZE 4096

// Variables to store control data for PID
struct pid_controller ctrldata;
struct coordinate location;
struct coordinate setpoint;
pid_ct controller;
double throttle_output = 0, steer_output = 0;
bool run, throttle_active, steer_active;

// Initial p, i, d values will be refined by actual live testing.. will tune these params from live testing
float dKp = 0.2, dKi = 0, dKd = 0, aKp = 0.5, aKi = 0, aKd = 0;

// Shared Memory Variables
sem_t *servo_sem;
sem_t *pos_sem;
int servo_fid;
int pos_fid;
void *servo_ptr;
void *pos_ptr;


int main() {

	/* Initializing interaction with shared mem (based off loothrottle_king @ actuation files) */
	sem_t *servo_sem = sem_open(SERVOSEM, 1);
	sem_t *pos_sem = sem_open(POSITSEM, 1);
	if (servo_sem == SEM_FAILED | pos_sem == SEM_FAILED) {
		perror("Semaphore open error. \n");
	}

	servo_fid = shm_open("racecar", O_CREAT | O_RDWR, 0666);
	pos_fid = shm_open("position", O_CREAT | O_RDWR, 2000);
	if (servo_fid == -1 | pos_fid == -1) {
		perror("Shared mem open error. \n");
		return -1;
	}

	servo_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, servo_fid, 0);
	pos_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, pos_fid, 0);
	if (servo_ptr == MAP_FAILED | pos_ptr == MAP_FAILED) {
		perror("Memory map error. \n");
		exit(-1);
	}

	struct throttle_steer *actuation_vals = (struct throttle_steer *)servo_ptr;
	struct pid_params *pid_inputs = (struct pid_params *)pos_ptr;

	location = pid_inputs->location; setpoint = pid_inputs->setpoint;

	/*                 Initializing PID controller               */
	// Location is in form (X_car, Y_car, Theta_car) where theta car is radians from +y
	// Setpoint is in fomm (X_set, Y_set, 0)
	// Kp,Ki,Kd are proportional, integral, and derivative parameters for dist & angle respectively
	// steer_output and throttle_output are values calculated by PID, pushed to shared mem for actuation
	controller = pid_create(&ctrldata, &location, &setpoint, dKp, dKi, dKd, aKp, aKi, aKd, &steer_output, &throttle_output);
	pid_auto(controller);
	pid_change_limits(controller, -1, 1);

	/* Prompt to see which controls to enable */
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
		// Get location info from shared mem, compute PID, updated wanted results
		location = pid_inputs->location;
		setpoint = pid_inputs->setpoint;
		pid_compute(controller);

		if(throttle_active){
			actuation_vals->throttle = (float) throttle_output;
		}
		if(steer_active){
			actuation_vals->steer = (float) steer_output;
		}

		// Update output throttle & steering to shared memory
		sem_post(servo_sem);
		// Update Results to the terminal in easy to read:
		printf("(X,Y,Ang).. C:(%f, %f, %f) S:(%f, %f).. [Err,Out] Dist:[%f, %f] Steer:[%f, %f] \n",location.x,location.y,
		       location.angle,setpoint.x,setpoint.y, controller->dst_error,throttle_output,controller->ang_err,steer_output);
	}
	return 0;
}
