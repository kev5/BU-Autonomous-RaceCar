//
// Created by Nicholas Arnold on 4/22/18.
//

#ifndef PID_COORDINATE_H
#define PID_COORDINATE_H

#include <cstdlib>

class Coordinate{
public:

	Coordinate() {
		this->x = NULL;
		this->y = NULL;
		this->angle = NULL;
		this->id = NULL;
	}

	Coordinate(double x, double y) {
		this->x = x;
		this->y = y;
		this->angle = NULL;
		this->id = NULL;
	}

	Coordinate(double x, double y, double angle) {
		this->x = x;
		this->y = y;
		this->angle = angle;
		this->id = NULL;
	}

	Coordinate(double x, double y, double angle, int id) {
		this->x = x;
		this->y = y;
		this->angle = angle;
		this->id = id;
	}

	Coordinate(double x, double y, int id) {
		this->x = x;
		this->y = y;
		this->angle = NULL;
		this->id = id;
	}

	double getX() const {
		return x;
	}

	double getY() const {
		return y;
	}

	double getAngle() const {
		return angle;
	}

	int getId() const {
		return id;
	}

	void setX(double x) {
		x = x;
	}

	void setY(double y) {
		y = y;
	}

	void setAngle(double angle) {
		angle = angle;
	}

	void setId(int id) {
		id = id;
	}


private:
	double x;
	double y;
	double angle;
	int id;
};


#endif //PID_COORDINATE_H
