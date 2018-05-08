//
// Created by Nicholas Arnold on 4/22/18.
//

#ifndef PID_COORDINATE_H
#define PID_COORDINATE_H

#include <cstdlib>

class Coordinate{
public:

	Coordinate() {
		this->x = 0;
		this->y = 0;
		this->angle = 0;
		this->id = 0;
	}

	Coordinate(double x, double y) {
		this->x = x;
		this->y = y;
		this->angle = 0;
		this->id = 0;
	}

	Coordinate(double x, double y, double angle) {
		this->x = x;
		this->y = y;
		this->angle = angle;
		this->id = 0;
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
		this->angle = 0;
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
		this->x = x;
	}

	void setY(double y) {
		this->y = y;
	}

	void setAngle(double angle) {
		this->angle = angle;
	}

	void setId(int id) {
		this->id = id;
	}

private:
	double x;
	double y;
	double angle;
	int id;
};


#endif //PID_COORDINATE_H
