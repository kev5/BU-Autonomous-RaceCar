#ifndef BU_AUTONOMOUS_RACECAR_PID_SOURCE_H
#define BU_AUTONOMOUS_RACECAR_PID_SOURCE_H
/*	Floating point PID control loop for Microcontrollers
	Copyright (C) 2014 Jesus Ruben Santa Anna Zamudio.
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	Author website: http://www.geekfactory.mx
	Author e-mail: ruben at geekfactory dot mx
 */

// Modified & reimplemented by Nicholas Arnold

/*		Includes and dependencies			*/

#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265

/*		Typedefs enums & structs			*/

/**
 * Defines if the controler is direct or reverse
 */
enum pid_control_directions {
	E_PID_DIRECT,
	E_PID_REVERSE,
};

/**
 * Structures that hold PID all the PID controller data, multiple instances are
 * posible using different structures for each controller
 */

struct coordinate {
	double x;
	double y;
	double angle;
};

struct pid_controller {
	// Input, output and shared_position
	struct coordinate * input; // Current coordinate of unit
	struct coordinate * setpoint; // Current goal destination point
	double * steer_out;
	double * throttle_out; // Corrective Output from PID Controller

	// Tuning parameters
	double aKp, dKp; // Stores the gain for the Proportional term
	double aKi, dKi; // Stores the gain for the Integral term
	double aKd, dKd; // Stores the gain for the Derivative term

	// Current Error
	double ang_err, dst_error;

	// Output minimum and maximum values
	double out_min; // Maximum value allowed at the output
	double out_max; // Minimum value allowed at the output

	// Variables for PID algorithm
	double a_int, d_int; // Accumulators for integral term
	struct coordinate *lastin; //  Last input value for differential term

	// Time related
	clock_t lasttime; // Stores the time when the control loop ran last time
	clock_t sampletime; // Defines the PID sample time

	// Operation mode
	uint8_t automode; // Defines if the PID controller is enabled or disabled
	enum pid_control_directions direction;
};

typedef struct pid_controller * pid_ct;

/*-------------------------------------------------------------*/
/*		Function prototypes				*/
/*-------------------------------------------------------------*/
#ifdef	__cplusplus
extern "C" {
#endif
/**
 * @brief Creates a new PID controller
 *
 * Creates a new pid controller and initializes it�s input, output and internal
 * variables. Also we set the tuning parameters
 *
 * @param pid A pointer to a pid_controller structure
 * @param input Pointer to a struct containing x, y, angle, representing current position
 * @param out Pointer to put the controller output value
 * @param set Pointer to a struct containing x, y, angle, representing desired position
 * @param kp Proportional gain
 * @param ki Integral gain
 * @param kd Diferential gain
 *
 * @return returns a pid_t controller handle
 */
pid_ct pid_create(pid_ct pid, struct coordinate *current_pos, struct coordinate *setpoint, float dKp, float dKi, float dKd,
                  float aKp, float aKi, float aKd, double *steer_out, double *throttle_out);

/**
 * @brief Check if PID loop needs to run
 *
 * Determines if the PID control algorithm should compute a new output value,
 * if this returs true, the user should read process feedback (sensors) and
 * place the reading in the input variable, then call the pid_compute() function.
 *
 * @return return Return true if PID control algorithm is required to run
 */
bool pid_need_compute(pid_ct pid); // not working right now on nvidia??

/**
 * @brief Computes the output of the PID control
 *
 * This function computes the PID output based on the parameters, setpoint and
 * current system input.
 *
 * @param pid The PID controller instance which will be used for computation
 */
void pid_compute(pid_ct pid);

/**
 * @brief Sets new PID tuning parameters
 *
 * Sets the gain for the Proportional (Kp), Integral (Ki) and Derivative (Kd)
 * terms.
 *
 * @param pid The PID controller instance to modify
 * @param dKp Proportional gain
 * @param dKi Integral gain
 * @param dKd Derivative gain
 */
void pid_tune(pid_ct pid, float dKp, float dKi, float dKd, float aKp, float aKi, float aKd);

/**
 * @brief Sets the pid algorithm period
 *
 * Changes the between PID control loop computations.
 *
 * @param pid The PID controller instance to modify
 * @param time The time in milliseconds between computations
 */
void pid_change_sampling(pid_ct pid, uint32_t time);

/**
 * @brief Sets the limits for the PID controller output
 *
 * @param pid The PID controller instance to modify
 * @param min The minimum output value for the PID controller
 * @param max The maximum output value for the PID controller
 */
void pid_change_limits(pid_ct pid, float min, float max);

/**
 * @brief Enables automatic control using PID
 *
 * Enables the PID control loop. If manual output adjustment is needed you can
 * disable the PID control loop using pid_manual(). This function enables PID
 * automatic control at program start or after calling pid_manual()
 *
 * @param pid The PID controller instance to enable
 */
void pid_auto(pid_ct pid);

/**
 * @brief Disables automatic process control
 *
 * Disables the PID control loop. User can modify the value of the output
 * variable and the controller will not overwrite it.
 *
 * @param pid The PID controller instance to disable
 */

void pid_manual(pid_ct pid);

/**
 * @brief Configures the PID controller direction
 *
 * Sets the direction of the PID controller. The direction is "DIRECT" when a
 * increase of the output will cause a increase on the measured value and
 * "REVERSE" when a increase on the controller output will cause a decrease on
 * the measured value.
 *
 * @param pid The PID controller instance to modify
 * @param direction The new direction of the PID controller
 */
void pid_direction(pid_ct pid, enum pid_control_directions dir);

/**
 *
 * @param x
 * @return 1 if >0 else 0
 */
int sign(double x);


#ifdef	__cplusplus
}
#endif

#endif //BU_AUTONOMOUS_RACECAR_PID_SOURCE_H
