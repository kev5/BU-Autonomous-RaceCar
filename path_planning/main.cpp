#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <math.h>
#include "json.hpp"
#include <iostream>
#include "AStar.hpp"
#include "shared_path.hpp"
#define SHM_SIZE 4096
#include <vector>

//shared memory variables
using json = nlohmann::json;

int main()
{
	int fid = shm_open("path", O_CREAT | O_RDWR, 0666);
	if (fid < 0) {
		perror("shm_open error");
		return -1;
	}

	void * ptr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fid, 0);
	if (ptr == MAP_FAILED) {
		perror("error with mapping");
		return -1;
	}

	AStar::Generator generator;
	generator.setWorldSize({25, 25});  //need to change with actual worldsize
	
	std::cout << "Generate path ... \n";

	AStar::Vec2i obstaclecoord;  // obstacle coordinates 
	struct obstacle * obstacleptr;
	obstacleptr = (struct obstacle *) ptr;
	obstaclecoord.x = (obstacleptr -> distance)*sin(obstacleptr -> angle);
	obstaclecoord.y = (obstacleptr -> distance)*cos(obstacleptr -> angle);
	
	generator.addCollision(obstaclecoord);

	generator.setHeuristic(AStar::Heuristic::euclidean);
	generator.setDiagonalMovement(true);

	struct begin_end * pathptr;
	pathptr = (struct begin_end *) ptr;

	AStar::Vec2i start_coord; // initial position
	start_coord.x = pathptr -> startx;
	start_coord.y = pathptr -> starty;

	AStar::Vec2i destination_coord;  // final position
	destination_coord.x = pathptr -> endx;
	destination_coord.y = pathptr -> endy;

	auto path = generator.findPath(start_coord, destination_coord);

	for(auto& coordinate : path) {
		std::cout << coordinate.x << " " << coordinate.y << "\n";
	}

	//close(fid);
	return 0;
}
