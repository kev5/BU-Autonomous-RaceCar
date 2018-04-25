//
// Created by Nicholas Arnold on 4/22/18.
//

#ifndef PID_COORDINATE_H
#define PID_COORDINATE_H


class Coordinate{
public:
	Coordinate(double x, double y);
	Coordinate(double x, double y, double angle);
	Coordinate(double x, double y, double angle, int id);
	Coordinate(double x, double y, int id);

	double getX() const;
	double getY() const;
	double getAngle() const;
	int getId() const;

	void setX(double x);
	void setY(double y);
	void setAngle(double angle);
	void setId(int id);

private:
	double x;
	double y;
	double angle;
	int id;
};


#endif //PID_COORDINATE_H
