//
// Created by Nicholas Arnold on 3/21/18.
//

#ifndef BU_AUTONOMOUS_RACECAR_PID_PARAMS_H
#define BU_AUTONOMOUS_RACECAR_PID_PARAMS_H

#include "Coordinate.h"

struct pid_params{

	Coordinate location;
	Coordinate setpoint;

	/* Coordinate has fields:
	 * float x
	 * float y
	 * float angle
	 * int id
	 */

	bool active;
};
#endif //BU_AUTONOMOUS_RACECAR_PID_PARAMS_H
