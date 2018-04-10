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
    CoordinateMap(){
        // Initiate Coordinate Map with already known coordinates
        coordinate twentytwo; twentytwo.x = 2.4 ;twentytwo.y = 1.9;
        coordinate seven; seven.x = 4.3; seven.y = 1.9;
        coordinate four; four.x = 6.0; seven.y = 1.9;

        add_marker(22 ,twentytwo);
        add_marker(7, seven);
        add_marker(4, four);
        // TODO: Serialization to permanently store newly created markers programmatically
    };

    coordinate getCoords(int marker){
        std::map<int,coordinate>::iterator it;
        it = aruco_map.find(marker);
	coordinate non; 
	non.x = 0; 
	non.y = 0; 
	non.angle = 0; 
        if(it != aruco_map.end()){
            return it->second;
        }
        else{
            return non;
        }
    };

    void add_marker(int value, coordinate position){
        aruco_map.insert(std::pair<int, coordinate>(value,position));
    };

private:
    std::map<int, coordinate> aruco_map;

};

#endif //BU_AUTONOMOUS_RACECAR_COORDINATEMAP_H
