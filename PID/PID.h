//
// Created by Nicholas Arnold on 4/22/18.
//

#ifndef PID_PID_H
#define PID_PID_H

#include "../include/Coordinate.h"
#include "../include/Setpoint_Queue.h"
#include <ctime>
#include <cmath>

#define PI 3.14159265

class PID {
public:
	/**
	 * @brief Constructor for PID that calculates both steering & throttle output
	 * @param current_pos - Coordinate object ptr to current location (X,Y,Theta,ID)
	 * @param setpoint - Coordinate object ptr to goal location (X,Y,Null,ID)
	 * @param dKp - Proportional constant for distance
	 * @param dKi - Integral constant for distance
	 * @param dKd - Derivative constant for distance
	 * @param aKp - Proportional constant for angle
	 * @param aKi - Integral constant for angle
	 * @param aKd - Derivative constant for angle
	 * @param steer_out - corrective output value for steering
	 * @param throttle_out - corrective output value for throttle
	 */
	PID(Coordinate *current_pos, Coordinate *setpoint, double dKp, double dKi, double dKd,
	    double aKp, double aKi, double aKd, double *steer_out, double *throttle_out);

	/**
	 * @brief Returns true if sampling time has been exceeded
	 */
	bool need_compute();

	/**
	 * @brief Updates the error calculations for angular & distance, updates pointers to steer_out and throttle_out
	 */
	void compute();

	/**
	 * @param dKp - Proportional constant for distance
	 * @param dKi - Integral constant for distance
	 * @param dKd - Derivative constant for distance
	 * @param aKp - Proportional constant for angle
	 * @param aKi - Integral constant for angle
	 * @param aKd - Derivative constant for angle
	 *
	 * @brief Modifies the constants used to calculate the output - default argument leaves the value as is
	 */
	void tune(double dKp, double dKi, double dKd, double aKp, double aKi, double aKd);

	/**
	 * @param time - the amount of time between computations
	 */
	void change_sampling(int time);

	/**
	 * @param min - the new mininum value that the PID can output
	 * @param max - the new maximum value that the PID can output
	 */
	void change_limits(double min, double max);

	/**
	 *
	 * @param new_state - PID will continuously operate when true
	 */
	void pid_mode(bool new_state);

	/**
	 *
	 * @param dir - The direction of the controller: if DIRECT, then increasing output will reduce error
	 * 					if REVERSE then decreasing output will reduce error
	 */
	void set_direction(bool dir);

	/* Getters */
	double get_dst_err();
	double get_ang_err();

private:

	/* Internal Functions for error calculations etc. */
	/**
	 *
	 * @param x - value to be checked
	 * @return  - true if value is non-negative, else false
	 */
	bool sign(double x);

	/**
	 *
	 * @param current - 1st point
	 * @param other - 2nd point
	 * @return the euclidean distance between the two coordinates
	 */
	double euclidean_dist(Coordinate current, Coordinate other);

	/**
	 *
	 * @param value - the value to be checked
	 * @return - Returns the value that was passed in unless it exceeds the max or min threshold of the PID
	 * 			if so, it will return the value of that threshold
	 */
	double enforce_bounds(double value);

	/*  Member Variables for computation */

	// Input, output and shared_position
	Coordinate * current; // Current coordinate of unit
	Coordinate * setpoint; // Current goal destination point
	double * steer_out;
	double * throttle_out; // Corrective Output from PID Controller

	// Tuning parameters
	double aKp, dKp; // Stores the gain for the Proportional term
	double aKi, dKi; // Stores the gain for the Integral term
	double aKd, dKd; // Stores the gain for the Derivative term

	// Current Error
	double ang_err, dst_err;

	// Output minimum and maximum values
	double out_min; // Maximum value allowed at the output
	double out_max; // Minimum value allowed at the output

	// Terms for PID algorithm
	double ang_int, dst_int; // Accumulators for integral term
	double ang_dif, dst_dif; // Differential between calculations
	Coordinate *lastin; //  Last input value for differential term

	// Time related
	clock_t lasttime; // Stores the time when the control loop ran last time
	clock_t sampletime; // Defines the PID sample time

	// Operation mode
	bool active; // Defines if the PID controller is enabled or disabled
	bool direction; // True if direct false if reverse
};

#endif //PID_PID_H
