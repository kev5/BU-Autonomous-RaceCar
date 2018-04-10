//
// Created by Nicholas Arnold on 4/4/18.
//

#ifndef BU_AUTONOMOUS_RACECAR_COORDINATEMAP_H
#define BU_AUTONOMOUS_RACECAR_COORDINATEMAP_H

#include "pid_source.h"
#include <map>

struct coordinate {
    float x;
    float y;
    float angle;
};

class CoordinateMap{
public:
    CoordinateMap();

    coordinate getCoords(int marker);
    void add_marker(int value, coordinate position);

private:
    std::map<int, coordinate> aruco_map;

};

#endif //BU_AUTONOMOUS_RACECAR_COORDINATEMAP_H
