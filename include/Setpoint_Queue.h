//
// Created by Nicholas Arnold on 4/25/18.
//

#ifndef PID_SETPOINT_QUEUE_H
#define PID_SETPOINT_QUEUE_H

#include <deque>
#include "Coordinate.h"


class Setpoint_Queue {
public:
	Setpoint_Queue() {
		// Constructor to initialize the setpoint queue:
		set_queue.clear();
		begin = set_queue.begin();
	}

	void pop_front() {
		set_queue.pop_front();
		begin = set_queue.begin();
	}

	void push_back(Coordinate new_point) {
		set_queue.push_back(new_point);
		begin = set_queue.begin();
	}

	void modify(Coordinate change_point) {

	}

	void remove(Coordinate remove_point) {

	}

	void reset() {
		set_queue.clear();
		begin = set_queue.begin();
	}

	Coordinate current_point() {
		return *begin;
	}

	Coordinate next_point() {
		auto returnval = *(begin++);
		begin = set_queue.begin();
		return returnval;
	}

	bool is_empty(){
		return set_queue.empty();
	}

private:
	std::deque<Coordinate> set_queue;
	std::deque<Coordinate>::iterator begin;
};


#endif //PID_SETPOINT_QUEUE_H
