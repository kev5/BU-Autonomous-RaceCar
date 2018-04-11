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

pid_ct pid_create(pid_ct pid, struct coordinate *current_pos, struct coordinate *setpoint, float dKp, float dKi, float dKd,
                  float aKp, float aKd, float aKi, double *steer_out, double *throttle_out) {
	pid->input = current_pos;
	pid->setpoint = setpoint;
	pid->steer_out = steer_out;
	pid->throttle_out = throttle_out;
	pid->automode = false;

	pid_change_limits(pid, 0, 255);

	// Set default sample time to 100 ms
	pid->sampletime = 100 * (CLOCKS_PER_SEC / 1000);

	pid_direction(pid, E_PID_DIRECT);
	pid_tune(pid, dKp, dKi, dKd, aKp, aKi, aKd);

	pid->lasttime = clock() - pid->sampletime;

	return pid;
}

bool pid_need_compute(pid_ct pid) {
	// Check if the PID period has elapsed
	return(clock() - pid->lasttime >= pid->sampletime) ? true : false;
}

int sign(double x){
    return x >= 0;
}


double eucledian_dist(struct coordinate *current, struct coordinate *last){
	return (float)pow(pow(current->x - last->x, 2) + pow(current->y - last->y, 2), 0.5);
}

double check_bounds(pid_ct pid,double value){
	value = (value > pid->out_max) ? pid->out_max : value;
	value = (value < pid->out_min) ? pid->out_min : value;
	return value;
}

void pid_compute(pid_ct pid) {
	// Check if control is enabled
	if (!pid->automode)
		return;

	double ang_err = 0, dst_error = 0;
	double d_ang = 0, d_dst = 0;
	struct coordinate *current = pid->input;
	struct coordinate *set = pid->setpoint;
	struct coordinate *last = pid->lastin;

    /* Normalizing angles to polar system */
    struct coordinate *set2 = set;
    set2->x = set->x - current->x;
    set2->y = set->y - current->y;

    set2->angle = atan(set2->y/set2->x);
    if(!sign(set2->x)){
        set2->angle += PI/2;
    } else if(sign(set2->x) & !sign(set2->y)){
        set2->angle += 2*PI;
    }
    if(!sign(current->angle)){
        current->angle = (PI/2) + fabs(current->angle);
    } else if(sign(current->angle) && current->angle <= PI/2){
        current->angle = (PI/2) - current->angle;
    } else{
        current->angle = (2*PI) - current->angle;
    }

    /* Getting angular error */
	ang_err = set2->angle - current->angle;
    if(fabs(ang_err) > 2*PI){
        ang_err = (ang_err > 0) ? (2*PI - ang_err) : (ang_err - 2*PI);
    }

    // Getting distance error, if its behind us: abs(ang_err) > PI/2
    dst_error = eucledian_dist(current,last);

	// Store current error in structure
	pid->ang_err = ang_err;
	pid->dst_error = dst_error;

	// Compute integrals & making sure they're within bound
	pid->a_int += (pid->aKi * ang_err);
	pid->d_int += (pid->dKi * dst_error);
	pid->a_int = check_bounds(pid, pid->a_int);
	pid->d_int = check_bounds(pid, pid->d_int);

	// Compute PID output, make sure its within bound update pointer
	double steer_out = pid->aKp * ang_err + pid->a_int - pid->aKd * d_ang;
	double throttle_out = pid->dKp * dst_error + pid->d_int - pid->dKd * d_dst;
	(*pid->steer_out) = check_bounds(pid, steer_out);
	(*pid->throttle_out) = check_bounds(pid, throttle_out);

	// Keep track of some variables for next execution
	pid->lasttime = clock();
	pid->lastin = current;
}

void pid_tune(pid_ct pid, float dKp, float dKi, float dKd, float aKp, float aKi, float aKd) {
	// Check for validity
	if (dKp < 0 || dKi < 0 || dKd < 0)
		return;

	//Compute sample time in seconds
	float ssec = ((float) pid->sampletime) / ((float) CLOCKS_PER_SEC);

	// Setting parameter values
	pid->dKp = dKp;
	pid->aKp = aKp;
	pid->dKi = dKi * ssec;
	pid->aKi = aKi * ssec;
	pid->aKd = aKd / ssec;
	pid->dKd = dKd / ssec;

	if (pid->direction == E_PID_REVERSE) {
		pid->aKp = 0 - pid->aKp;
		pid->aKi = 0 - pid->aKi;
		pid->aKd = 0 - pid->aKd;
		pid->dKp = 0 - pid->dKp;
		pid->dKi = 0 - pid->dKi;
		pid->dKd = 0 - pid->dKd;
	}
}

void pid_change_sampling(pid_ct pid, uint32_t time) {
	if (time > 0) {
		float ratio = (float) (time * (CLOCKS_PER_SEC / 1000)) / (float) pid->sampletime;
		pid->aKi *= ratio;
		pid->dKi *= ratio;
		pid->aKd /= ratio;
		pid->dKd /= ratio;
		pid->sampletime = time * (CLOCKS_PER_SEC / 1000);
	}
}

void pid_change_limits(pid_ct pid, float min, float max) {
	if (min >= max) return;
	pid->out_min = min;
	pid->out_max = max;

	// Re-applying new limits on steering & tuning params
	*(pid->throttle_out) = check_bounds(pid, *(pid->throttle_out));
	*(pid->steer_out) = check_bounds(pid, *(pid->steer_out));
	pid->a_int = check_bounds(pid, pid->a_int);
	pid->d_int = check_bounds(pid,pid->d_int);

}

void pid_auto(pid_ct pid) {
	// If going from manual to auto
	if (!pid->automode) {
		pid->a_int = check_bounds(pid, *(pid->steer_out));
		pid->d_int = check_bounds(pid,*(pid->throttle_out));
		pid->lastin = pid->input;
		pid->automode = true;
	}
}

void pid_manual(pid_ct pid) {
	pid->automode = false;
}

void pid_direction(pid_ct pid, enum pid_control_directions dir) {
	if (pid->automode && pid->direction != dir) {
		pid->aKp = 0 - pid->aKp;
		pid->aKi = 0 - pid->aKi;
		pid->aKd = 0 - pid->aKd;
		pid->dKp = 0 - pid->dKp;
		pid->dKi = 0 - pid->dKi;
		pid->dKd = 0 - pid->dKd;
	}
	pid->direction = dir;
}
