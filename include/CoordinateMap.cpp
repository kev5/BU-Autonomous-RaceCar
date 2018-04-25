//
// Created by Nicholas Arnold on 4/22/18.
//

#include "CoordinateMap.h"

CoordinateMap::CoordinateMap() {
	Coordinate twentytwo = Coordinate(2.4, 1.9);
	Coordinate seven = Coordinate(4.3, 1.9);
	Coordinate four = Coordinate(6.0, 1.9);

	this->add_marker(twentytwo);
	this->add_marker(seven);
	this->add_marker(four);
}

Coordinate CoordinateMap::get_coords(int marker_id) {
	std::map<int, Coordinate>::iterator it;
	it = aruco_map.find(marker_id);

	if(it != aruco_map.end()){
		return it->second; // returns coordinate obj instead of ID
	}
	else{
		return nullptr;
	}
}

void CoordinateMap::add_marker(Coordinate marker) {
	aruco_map.insert(std::pair<int, Coordinate>(marker.getId(), marker));
}
