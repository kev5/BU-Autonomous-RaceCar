//
// Created by Nicholas Arnold on 4/4/18.
//

#ifndef BU_AUTONOMOUS_RACECAR_COORDINATEMAP_H
#define BU_AUTONOMOUS_RACECAR_COORDINATEMAP_H

#include "Coordinate.h"
#include <map>

// WARNING: don't include from .c files because <map> only works in c++

class CoordinateMap{
public:
    CoordinateMap();

	Coordinate get_coords(int marker_id);

    void add_marker(Coordinate position);

private:
    std::map<int, Coordinate> aruco_map;
};

#endif //BU_AUTONOMOUS_RACECAR_COORDINATEMAP_H
