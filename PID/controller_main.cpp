//
// Created by Nicholas Arnold on 4/22/18.
//

// Language Includes
#include <iostream>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Project File Includes
#include "PID.h"
#include "../include/Setpoint_Queue.h"
#include "../include/pid_params.h"
#include "../include/shared_def.h"
#include "../include/CoordinateMap.h"

// Shared Memory Definitions
#define SERVOSEM "/servosemaphore"
#define POSITSEM "/position_sem"
#define SHM_SIZE 4096

using namespace std;

// Shared Memory Variables PID is reading
sem_t *pos_sem;
int pos_fid;
void *pos_ptr;

// Shared mem values PID is updating
sem_t *servo_sem;
int servo_fid;
void *servo_ptr;
double* throttle_out;
double * steer_out;

// Pointers to position variables:
Coordinate* current;
Coordinate* setpoint;
Setpoint_Queue* setpoints;

// Initial Values for controller:
bool run, throttle_active, steer_active;
double dKp = 0.2, dKi = 0, dKd = 0.1, aKp = 0.5, aKi = 0, aKd = 0;

int main(){
	/* Initializing interaction with shared mem (based off loothrottle_king @ actuation files) */
	sem_t *servo_sem = sem_open(SERVOSEM, 1);
	sem_t *pos_sem = sem_open(POSITSEM, 1);

	if (servo_sem == SEM_FAILED){
		sem_unlink(SERVOSEM);
		servo_sem = sem_open(SERVOSEM,1);
		if (servo_sem == SEM_FAILED){
			perror("Servo Semaphore open error (from controller_main) \n");
			exit(1);
		}
	}

	if (pos_sem == SEM_FAILED){
		sem_unlink(POSITSEM);
		pos_sem = sem_open(POSITSEM,1);
		if (pos_sem == SEM_FAILED){
			perror("Position Semaphore open error (from controller_main) \n");
			exit(1);
		}
	}

	servo_fid = shm_open("racecar", O_CREAT | O_RDWR, 0666);
	pos_fid = shm_open("position", O_CREAT | O_RDWR, 2000);
	if (servo_fid == -1 | pos_fid == -1) {
		perror("Memory open error. (from controller_main) \n");
		return -1;
	}

	servo_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, servo_fid, 0);
	pos_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, pos_fid, 0);
	if (servo_ptr == MAP_FAILED | pos_ptr == MAP_FAILED) {
		perror("Memory mapping error. (from controller_main) \n");
		exit(-1);
	}

	struct throttle_steer *actuation_vals = (struct throttle_steer *)servo_ptr;
	struct pid_params *pid_inputs = (struct pid_params *)pos_ptr;

	// Copying shared mem values into Coordinate pointers & checking if active
	*setpoint = pid_inputs->setpoint;
	Coordinate *current = pid_inputs->location;
	run = pid_inputs->active;

	CoordinateMap crd_map = CoordinateMap();
	Setpoint_Queue setpoints = Setpoint_Queue();
	setpoints.push_back(crd_map.get_coords(9));
	setpoints.push_back(crd_map.get_coords(12));
	setpoints.push_back(Coordinate(30.1042,-1.52545));
	setpoints.push_back(crd_map.get_coords(29));
	setpoints.push_back(crd_map.get_coords(10));
	setpoints.push_back(crd_map.get_coords(7));


	PID controller = PID(current, setpoint, dKp, dKi, dKd, aKp, aKi, aKd, steer_out, throttle_out);
	throttle_active = true;
	steer_active = true;

	while(run){
		sem_wait(servo_sem);

		if(setpoints.is_empty()){
			actuation_vals->throttle = 0;
			actuation_vals->steer = 0;
		}
		else{
			// Get updated location info from shared mem
			*current = pid_inputs->location;
			*setpoint = setpoints.current_point();

			// Compute output values:
			controller.compute();

			// Update Values to pointers if parameter is active:
			if (throttle_active){
				actuation_vals->throttle = (float)*throttle_out;
			}
			if (steer_active){
				actuation_vals->steer = (float)*steer_out;
			}

			// Switch point if within a half meter of the goal
			if(controller.get_dst_err() <= 0.2){
				setpoints.pop_front();
				*setpoint = setpoints.current_point();
			}

			// Printout of results
			// TODO: Make a window that displays results, & also log the last run
			cout << "Car: (" << current->getX() << ", " << current->getY() << ", " << current->getAngle() << ") ";
			cout << "Set: (" << setpoint->getX() << ", " << setpoint->getY() << ") ";
			cout << "ERR: (" << controller.get_dst_err() << " m., " << controller.get_ang_err() << " Rads.)";
			cout << "Out: (" << *throttle_out << ", " << *steer_out << ")" << endl;
		}

		// Post to shared memory
		sem_post(servo_sem);
	}

	return 0;
}