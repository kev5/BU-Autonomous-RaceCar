#include <iostream>
#include "AStar.hpp"
#include "obstacles_coords.hpp"
#define SHM_SIZE 4096
//shared memory variables


int main()
{
	int fid = shm_open("obstacles", O_CREATE | 0_RDONLY, 0666);
	if (fid < 0) {
	perror("shm_open error");
	return -1;
	}

	void * ptr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fid, 0);
	if (ptr == MAP_FAILED) {
	perror("error with mapping");
	return -1;
	}
	vector<vector<int>> * coordsptr;

	coordsptr = (vector <vector< <int>> obstacles *) ptr; //obstacles is name of vector in header file, shared memory object

   	AStar::Generator generator;
    	generator.setWorldSize({25, 25});
	vector<int> coord;
   	// obstacle coordinates from shared memory
  	for  (int i =0; i <= obstacles.size() ; i++){
	coord = (*coordsptr).at(i);
	generator.addCollision(coord);
}
	


    generator.setHeuristic(AStar::Heuristic::euclidean);
    generator.setDiagonalMovement(true);

    std::cout << "Generate path ... \n";
    auto path = generator.findPath({0, 0}, {20, 20});

    for(auto& coordinate : path) {
        std::cout << coordinate.x << " " << coordinate.y << "\n";
    }

close(fid);
}
