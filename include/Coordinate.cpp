//
// Created by Nicholas Arnold on 4/22/18.
//


#include "Coordinate.h"

double Coordinate::getX() const {
	return x;
}

double Coordinate::getY() const {
	return y;
}

double Coordinate::getAngle() const {
	return angle;
}

int Coordinate::getId() const {
	return id;
}

void Coordinate::setX(double x) {
	Coordinate::x = x;
}

void Coordinate::setY(double y) {
	Coordinate::y = y;
}

void Coordinate::setAngle(double angle) {
	Coordinate::angle = angle;
}

void Coordinate::setId(int id) {
	Coordinate::id = id;
}

Coordinate::Coordinate() {
	this->x = NULL;
	this->y = NULL;
	this->angle = NULL;
	this->id = NULL;
}

Coordinate::Coordinate(double x, double y) {
	this->x = x;
	this->y = y;
	this->angle = NULL;
	this->id = NULL;
}

Coordinate::Coordinate(double x, double y, double angle) {
	this->x = x;
	this->y = y;
	this->angle = angle;
	this->id = NULL;
}

Coordinate::Coordinate(double x, double y, double angle, int id) {
	this->x = x;
	this->y = y;
	this->angle = angle;
	this->id = id;
}

Coordinate::Coordinate(double x, double y, int id) {
	this->x = x;
	this->y = y;
	this->angle = NULL;
	this->id = id;
}
