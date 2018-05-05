//
// Created by Nicholas Arnold on 4/25/18.
//

#include "Setpoint_Queue.h"


Setpoint_Queue::Setpoint_Queue() {
	// Constructor to initialize the setpoint queue:
	set_queue.clear();
	begin = set_queue.begin();
}

void Setpoint_Queue::pop_front() {
	set_queue.pop_front();
	begin = set_queue.begin();
}

void Setpoint_Queue::push_back(Coordinate new_point) {
	set_queue.push_back(new_point);
	begin = set_queue.begin();
}

void Setpoint_Queue::modify(Coordinate change_point) {

}

void Setpoint_Queue::remove(Coordinate remove_point) {

}

void Setpoint_Queue::reset() {
	set_queue.clear();
	begin = set_queue.begin();
}

Coordinate Setpoint_Queue::current_point() {
	return *begin;
}

Coordinate Setpoint_Queue::next_point() {
	auto returnval = *(begin++);
	begin = set_queue.begin();
	return returnval;
}

bool Setpoint_Queue::is_empty(){
	return set_queue.empty();
}

