#include "../include/pid_source.h"
/*	Floating point PID control loop for Microcontrollers
	Copyright (C) 2015 Jesus Ruben Santa Anna Zamudio.
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

pid_ct pid_create(pid_ct pid, struct coordinate *in, float *out, struct coordinate *set, float kp, float ki, float kd,
                  bool angle) {
	pid->input = in;
	pid->output = out;
	pid->setpoint = set;
	pid->automode = false;
	pid->angle = angle;

	pid_change_limits(pid, 0, 255);

	// Set default sample time to 100 ms
	pid->sampletime = 100 * (CLOCKS_PER_SEC / 1000);

	pid_direction(pid, E_PID_DIRECT);
	pid_tune(pid, kp, ki, kd);

	pid->lasttime = clock() - pid->sampletime;

	return pid;
}

bool pid_need_compute(pid_ct pid) {
	// Check if the PID period has elapsed
	return(clock() - pid->lasttime >= pid->sampletime) ? true : false;
}

void pid_compute(pid_ct pid) {
	// Check if control is enabled
	if (!pid->automode)
		return;

	float error;
	float dinput;
	struct coordinate current = *(pid->input);
	struct coordinate set = *(pid->setpoint);
	struct coordinate last = pid->lastin;

	// Measuring error & deriv. are different for distance & angle

	// Error is merely difference b/t angles, but need to calc. euclidean distance between coords.
	error = (pid->angle) ? (set.angle - current.angle) : (float) pow(pow(set.x - current.x, 2) + pow(set.y - current.y, 2), 0.5);
	// Dinput is merely difference b/t current, last angle, but need to calculate euclidean distance between coords.
	dinput = (pid->angle) ? (current.angle - last.angle) : (float) pow(pow(current.x - last.x, 0.5) + pow(current.y - last.y, 0.5), 2);

	// Store current error in structure
	pid->current_err = error;

	// Compute integral
	pid->iterm += (pid->Ki * error);
	if (pid->iterm > pid->out_max) {
		pid->iterm = pid->out_max;
	}
	else if (pid->iterm < pid->out_min) {
		pid->iterm = pid->out_min;
	}

	// Compute PID output
	float out = pid->Kp * error + pid->iterm - pid->Kd * dinput;

	// Apply limit to output value
	out = (out > pid->out_max) ? pid->out_max : out;
	out = (out < pid->out_min) ? pid->out_min : out;

	// Update structure's pointer
	(*pid->output) = out;

	// Keep track of some variables for next execution
	pid->lasttime = clock();
	pid->lastin = current;
}

void pid_tune(pid_ct pid, float kp, float ki, float kd) {
	// Check for validity
	if (kp < 0 || ki < 0 || kd < 0)
		return;

	//Compute sample time in seconds
	float ssec = ((float) pid->sampletime) / ((float) CLOCKS_PER_SEC);

	pid->Kp = kp;
	pid->Ki = ki * ssec;
	pid->Kd = kd / ssec;

	if (pid->direction == E_PID_REVERSE) {
		pid->Kp = 0 - pid->Kp;
		pid->Ki = 0 - pid->Ki;
		pid->Kd = 0 - pid->Kd;
	}
}

void pid_change_sampling(pid_ct pid, uint32_t time) {
	if (time > 0) {
		float ratio = (float) (time * (CLOCKS_PER_SEC / 1000)) / (float) pid->sampletime;
		pid->Ki *= ratio;
		pid->Kd /= ratio;
		pid->sampletime = time * (CLOCKS_PER_SEC / 1000);
	}
}

void pid_change_limits(pid_ct pid, float min, float max) {
	if (min >= max) return;
	pid->out_min = min;
	pid->out_max = max;
	//Adjust output to new limits
	if (pid->automode) {
		if (*(pid->output) > pid->out_max){
			*(pid->output) = pid->out_max;
		}
		else if (*(pid->output) < pid->out_min) {
			*(pid->output) = pid->out_min;
		}

		if (pid->iterm > pid->out_max){
			pid->iterm = pid->out_max;
		}
		else if (pid->iterm < pid->out_min){
			pid->iterm = pid->out_min;
		}
	}
}

void pid_auto(pid_ct pid) {
	// If going from manual to auto
	if (!pid->automode) {
		pid->iterm = *(pid->output);
		pid->lastin = *(pid->input);
		if (pid->iterm > pid->out_max)
			pid->iterm = pid->out_max;
		else if (pid->iterm < pid->out_min)
			pid->iterm = pid->out_min;
		pid->automode = true;
	}
}

void pid_manual(pid_ct pid) {
	pid->automode = false;
}

void pid_direction(pid_ct pid, enum pid_control_directions dir) {
	if (pid->automode && pid->direction != dir) {
		pid->Kp = (0 - pid->Kp);
		pid->Ki = (0 - pid->Ki);
		pid->Kd = (0 - pid->Kd);
	}
	pid->direction = dir;
}