//
// Created by Nicholas Arnold on 3/21/18.
//

#ifndef PID_SHARED_POS_H
#define PID_SHARED_POS_H

struct setpoint{
	float car_x;
	float car_y;
	float car_angle;

	float set_x;
	float set_y;
	float set_angle;
};
#endif //PID_SHARED_POS_H
