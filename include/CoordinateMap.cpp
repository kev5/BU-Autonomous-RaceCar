//
// Created by Nicholas Arnold on 4/4/18.
//

#include "CoordinateMap.h"

CoordinateMap::CoordinateMap() {
    // Initiate Coordinate Map with already known coordinates
    coordinate twentytwo; twentytwo.x = 2.4 ;twentytwo.y = 1.9;
    coordinate seven; seven.x = 4.3; seven.y = 1.9;
    coordinate four; four.x = 6.0; seven.y = 1.9;

    add_marker(22 ,twentytwo);
    add_marker(7, seven);
    add_marker(4, four);
    // TODO: Serialization to permanently store newly created markers programmatically
}

void CoordinateMap::add_marker(int value, coordinate position) {
    aruco_map.insert(std::pair<int, coordinate>(value,position));
}

coordinate CoordinateMap::getCoords(int marker) {
    std::map<int,coordinate>::iterator it;
    it = aruco_map.find(marker);

    if(it != aruco_map.end()){
        return it->second;
    }
    else{
        return nullptr;
    }
}