//
// Created by Nicholas Arnold on 3/21/18.
//

#ifndef PID_SHARED_POS_H
#define PID_SHARED_POS_H

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
#endif //PID_SHARED_POS_H
