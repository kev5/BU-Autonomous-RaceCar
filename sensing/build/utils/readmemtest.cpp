// Shared Memory includes
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include "setpoint.h"
#define SEM_NAME "/position_sem"

int main(){
	sem_t *sem = sem_open(SEM_NAME,0); 
	int fd = shm_open("position", O_CREAT | O_RDWR, 2000);
	if ( fd < 0){
		perror("shm_open error"); 
		return -1; 
	}
	
	void * ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED){
		perror("error with mapping");
		return -1;
	}
	struct setpoint *distanceptr = (struct setpoint *) ptr; 
	while(1){
		std::cout << "X = " << distanceptr->x_pos << std:: endl; 
		std::cout << "Y = " << distanceptr->y_pos << std:: endl; 
	}

}
