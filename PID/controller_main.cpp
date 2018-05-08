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
#include <typeinfo>

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

// Pointers to position variables:
Setpoint_Queue* setpoints;

int main(){
	bool run, throttle_active, steer_active;
	double dKp = 0.2, dKi = 0, dKd = 0, aKp = 0.4, aKi = 0, aKd = 0.15;

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
	actuation_vals->steer = 0;
	actuation_vals->throttle = 0;

	// Copying shared mem values into Coordinate pointers & checking if active
	double *throttle_out = new double;
	*throttle_out = 0;
	double *steer_out = new double;
	*steer_out = 0;
	Coordinate *current = new Coordinate(0,0,0);
	Coordinate *setpoint = new Coordinate(0,0,0);

	*current = pid_inputs->location;
	run = true;
	CoordinateMap crd_map = CoordinateMap();
	Setpoint_Queue setpoints = Setpoint_Queue();
	setpoints.push_back(crd_map.get_coords(26));
	setpoints.push_back(crd_map.get_coords(28));
	setpoints.push_back(crd_map.get_coords(11));
	setpoints.push_back(crd_map.get_coords(10));
	setpoints.push_back(Coordinate(26.0114, 0.973));

	PID controller = PID(current, setpoint, dKp, dKi, dKd, aKp, aKi, aKd, steer_out, throttle_out);

	throttle_active = false;
	steer_active = false;
	char t, s;
	cout << "Throttle active? (y/n):";
	cin >> t;
	cout << endl;
	cout << "Steer active? (y/n):";
	cin >> s;
	cout << endl;

	if(t == 'y'){
		throttle_active = true;
	}
	if(s == 'y'){
		steer_active = true;
	}

	while(run){

		sem_wait(servo_sem);
		if(setpoints.is_empty()){
			actuation_vals->throttle = 0;
			actuation_vals->steer = 0;
			sem_post(servo_sem);
			exit(1);
		}
		else{
			// Get updated location info from shared mem
			*current = pid_inputs->location;
			*setpoint = setpoints.current_point();
			Coordinate *nextpoint = new Coordinate(0,0);  
			*nextpoint = setpoints.next_point(); 
			double x2 = nextpoint->getX(); 
			double y2 = nextpoint->getY(); 
			double x1 = setpoint->getX(); 
			double y1 = setpoint->getY();
			double slope;  
			if((x2-x1)==0){
				slope = 0; 
			} else {
				slope = -(y2-y1)/(x2-x1);
			}
			double intercept = x1-slope*y1; 

			// Compute output values:
			controller.compute();

			// Update Values to pointers if parameter is active:
			if (throttle_active){
				actuation_vals->throttle = (float)*throttle_out;
			}
			else{
				actuation_vals->throttle = 0;
			}

			if (steer_active){
				actuation_vals->steer = (float)*steer_out;
			}
			else{
				actuation_vals->steer = 0;
			}

			// Switch point if within a half meter of the goal
			if(controller.get_dst_err() <= 0.40 || controller.curr.getX() < slope*controller.curr.getY()+intercept ){
				setpoints.pop_front();
				*setpoint = setpoints.current_point();
			}
			

			// Printout of results
			cout << "Car: (" << controller.curr.getX() << ", " << controller.curr.getY() << ", " << controller.curr.getAngle() << ") ";
			cout << "Set: (" << setpoint->getX() << ", " << setpoint->getY() << ") ";
			cout << "*SET*: (" << controller.set_prime.getX() << ", " << controller.set_prime.getY() << ", " << controller.set_prime.getAngle() << ") ";
			cout << "ERR: (" << controller.get_dst_err() << " m., " << controller.get_ang_err() << " Rads.) ";
			cout << "Out: (" << *throttle_out << ", " << *steer_out << ")" << endl;
		}

		// Post to shared memory
		sem_post(servo_sem);
	}

	return 0;
}
