//
// Created by Nicholas Arnold on 3/21/18.
//

#ifndef BU_AUTONOMOUS_RACECAR_PID_PARAMS_H
#define BU_AUTONOMOUS_RACECAR_PID_PARAMS_H

#include "CoordinateMap.h"

struct pid_params{

	struct coordinate location;
	struct coordinate setpoint;

	/* Coordinate has fields:
	 * float x
	 * float y
	 * float angle
	 */

	// TODO: Setting true will enable PID
	bool active;
};
#endif //BU_AUTONOMOUS_RACECAR_PID_PARAMS_H
