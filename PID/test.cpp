//
// Created by Nicholas Arnold on 4/23/18.
//

#include "../include/CoordinateMap.h"
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>

using namespace std;

int main(){
	CoordinateMap myMap = CoordinateMap();
	const char* FILENAME = "coord_map.txt";

	ofstream outfile;
	ifstream infile;

	outfile.open(FILENAME, ofstream::trunc);
	outfile.write((char*)myMap, sizeof(myMap));

	return 0;
}