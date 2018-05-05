//
// Created by Nicholas Arnold on 4/22/18.
//

#include "CoordinateMap.h"

CoordinateMap::CoordinateMap() {
	Coordinate twentytwo = Coordinate(2.4, 1.9, (int)22);
	Coordinate seven = Coordinate(22.573, -0.973, (int)7);
	Coordinate four = Coordinate(6.0, 1.9, (int)4);
	Coordinate eight = Coordinate(23.7922,-0.973,(int)8); 
	Coordinate nine = Coordinate(25.0114,-0.973,(int)9); 
	Coordinate ten = Coordinate(26.2306,-0.973,(int)10);
	Coordinate eleven = Coordinate(27.4498,-0.973,(int)11);
	Coordinate twelve = Coordinate(28.885,-0.973,(int)12); 
	Coordinate thirteen = Coordinate(30.1042,-0.973,(int)13); 
	Coordinate fourteen = Coordinate(31.3234,-0.973,(int)14); 
	Coordinate twentyfive = Coordinate(32.5426,-0.973,(int)25); 
	Coordinate twentysix = Coordinate(32.5426,-2.0779,(int)26); 
	Coordinate twentyseven = Coordinate(31.3234,-2.0779,(int)27); 
	Coordinate twentyeight = Coordinate(30.1042,-2.0779,(int)28); 
	Coordinate twentynine = Coordinate(28.885,-2.0779,(int)29); 
	Coordinate thirty = Coordinate(27.6658,-2.0779,(int)30); 

	this->add_marker(twentytwo);
	this->add_marker(seven);
	this->add_marker(four);
	this->add_marker(eight); 
	this->add_marker(nine); 
	this->add_marker(ten); 
	this->add_marker(eleven); 
	this->add_marker(twelve); 
	this->add_marker(thirteen); 
	this->add_marker(fourteen); 
	this->add_marker(twentyfive); 
	this->add_marker(twentysix); 
	this->add_marker(twentyseven); 
	this->add_marker(twentyeight); 
	this->add_marker(twentynine); 
	this->add_marker(thirty); 
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
