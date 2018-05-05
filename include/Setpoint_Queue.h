//
// Created by Nicholas Arnold on 4/25/18.
//

#ifndef PID_SETPOINT_QUEUE_H
#define PID_SETPOINT_QUEUE_H

#include <deque>
#include "Coordinate.h"


class Setpoint_Queue {
public:
	Setpoint_Queue();

	void pop_front();
	Coordinate current_point();
	Coordinate next_point();

	void push_back(Coordinate new_point);
	void modify(Coordinate change_point);
	void remove(Coordinate remove_point);
	void reset();

	bool is_empty();

private:
	std::deque<Coordinate> set_queue;
	std::deque<Coordinate>::iterator begin;
};


#endif //PID_SETPOINT_QUEUE_H
