//
// Created by Nicholas Arnold on 4/22/18.
//

#include "PID.h"

PID::PID(Coordinate *current_pos, Coordinate *setpoint, double dKp, double dKi, double dKd, double aKp, double aKi,
         double aKd, double *steer_out, double *throttle_out) {
	this->current = current_pos;
	this->setpoint = setpoint;
	this->dKp = dKp;
	this->dKi = dKi;
	this->dKd = dKd;
	this->aKp = aKp;
	this->aKi = aKi;
	this->aKd = aKd;
	this->steer_out = steer_out;
	this->throttle_out = throttle_out;

	out_min = -1;
	out_max = 1;
	active = true;
    ang_int = 0;
    dst_int = 0;
    direction = true;

	this->sampletime = 100*(CLOCKS_PER_SEC / 1000); // Default time is 100ms
	this->change_limits(-1,1);
	this->lasttime = clock() - sampletime;
}

bool PID::need_compute() {
	return (clock() - this->lasttime >= this->sampletime);
}

bool PID::sign(double x) {
	return (x >= 0);
}

double PID::euclidean_dist(Coordinate current, Coordinate other) {
	return pow(pow(current.getX() - other.getX(),2) + pow(current.getY() - other.getY(), 2),0.5);
}

double PID::enforce_bounds(double value){
	value = (value > out_max) ? out_max : value;
	value = (value < out_min) ? out_min : value;
	return value;
}

void PID::compute() {
	if(!active)
		return;

	// Copying prevents modifying pointers to shared mem coordinates
	curr = Coordinate(current->getX(),current->getY(),current->getAngle());
	set_prime = *setpoint - curr;
	double ang_err_t, dst_err_t;

	// Converting Set-point angle to polar:
	set_prime.setAngle(atan2(set_prime.getY(),set_prime.getX())); // Returns in range -[PI , PI] (WRT. X+)
	set_prime.setAngle((set_prime.getAngle() >=0) ? set_prime.getAngle() : 2PI - abs(set_prime.getAngle())); // [0, 2PI]

	// Converting car's angle to polar:
	if(!sign(curr.getAngle())){
		// ~Quad II and III:
		curr.setAngle(abs(curr.getAngle()) + (PI/2));
	} else if (sign(curr.getAngle()) && (curr.getAngle() <= (PI/2))){
		// ~Quad I:
		curr.setAngle(((PI/2) - curr.getAngle()));
	}else{
		// ~Quad IV:
		curr.setAngle((5*PI/2)- curr.getAngle());
	}

	// Calculating angular error, correcting for values over 180 degrees (PI radians):
	ang_err_t = curr.getAngle() - set_prime.getAngle();
	if(abs(ang_err_t) > PI){
		ang_err_t = ang_err_t + (sign(ang_err_t) ? (-2*PI) :  (2*PI));
	}

	// Calculating distance error, if the setpoint is behind us, abs(ang_err) > PI/2
	dst_err_t = hypot(set_prime.getX(), set_prime.getY());


	// Updating calculated error to object
	dst_err = dst_err_t;
	ang_err = ang_err_t;

	// Computing Difference between this time & last
	ang_dif = abs(curr.getAngle() - lastin.getAngle());
	dst_dif = euclidean_dist(curr, lastin);

	// Compute Integral values, enforce that they're within bounds
	ang_int += aKi * ang_err;
	dst_int += dKi * dst_err;
	ang_int = enforce_bounds(ang_int);
	dst_int = enforce_bounds(dst_int);

	// Compute PID output, check if its within bound, update shared pointer
	double steer = (aKp*ang_err) + ang_int - (aKd*ang_dif);
	double throttle = (dKp*dst_err) + dst_int - (dKd*dst_dif);

	*steer_out = enforce_bounds(steer);
	*throttle_out = enforce_bounds(throttle);

	// Keep track of some values for next calculation
	lasttime = clock();
	lastin = curr;
}

void PID::tune(double dKp_in, double dKi_in, double dKd_in, double aKp_in, double aKi_in, double aKd_in) {
	if(dKp_in < 0 || dKi_in < 0 || dKd_in < 0 || aKp_in < 0 || aKi_in < 0 || aKd_in < 0)
		return;

	double sample_t = (double)sampletime / CLOCKS_PER_SEC;
	dKp = dKp_in;
	dKi = dKi_in * sample_t;
	dKd = dKd_in / sample_t;

	aKp = aKp_in;
	aKi = aKi_in * sample_t;
	aKd = aKd_in / sample_t;

	if(!direction){
		dKp = 0 - dKp;
		dKi = 0 - dKi;
		dKd = 0 - dKd;
		aKp = 0 - aKp;
		aKi = 0 - aKi;
		aKd = 0 - aKd;
	}
}

void PID::change_sampling(int time) {
	if(time > 0){
		double ratio = ((double)time * (CLOCKS_PER_SEC/1000.0)) / (double)sampletime;
		aKi *= ratio;
		dKi *= ratio;
		aKd /= ratio;
		dKd /= ratio;
		sampletime = time * (CLOCKS_PER_SEC / 1000);
	}
}

void PID::change_limits(double min, double max){
	if(min >= max)
		return;

	out_min = min;
	out_max = max;

	// Re-applying new limits on steering & tuning params
	*throttle_out = enforce_bounds(*throttle_out);
	*steer_out = enforce_bounds(*steer_out);
	ang_int = enforce_bounds(ang_int);
	dst_int = enforce_bounds(dst_int);
}

void PID::pid_mode(bool new_state) {
	if(!active && new_state) {
		*throttle_out = enforce_bounds(*throttle_out);
		*steer_out = enforce_bounds(*steer_out);
		ang_int = enforce_bounds(ang_int);
		dst_int = enforce_bounds(dst_int);
	}
	active = new_state;
}

void PID::set_direction(bool dir) {
	if(active && (direction != dir)){
		// Changing directions
		dKp = 0 - dKp;
		dKi = 0 - dKi;
		dKd = 0 - dKd;
		aKp = 0 - aKp;
		aKi = 0 - aKi;
		aKd = 0 - aKd;
	}
	direction = dir;
}

double PID::get_dst_err() {
	return this->dst_err;
}

double PID::get_ang_err() {
	return this->ang_err;
}
