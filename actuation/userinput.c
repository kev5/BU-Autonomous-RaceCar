#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#define DEV_PATH  "/dev/PWM_OUT"
#include <stdlib.h>
#include "shared_def.h"

#define SHM_SIZE 4096

int main()
{
	int fid = shm_open("racecar", O_RDWR, 0666);
	if (fid == -1)
	{
		perror("shm_open error\n");
		return -1;
	}

	void *ptr = mmap(NULL, SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fid, 0);
	if (ptr == MAP_FAILED){
		perror("mmap error\n");
		exit(-1);
	}

	struct throttle_steer *moveptr = (struct throttle_steer *)ptr;

	while(1) {
		printf("Input steer and throttle target: ");
		scanf("%f %f", &(moveptr->steer), &(moveptr->throttle));
		
		//}
	
		//while(1) {
		
		// printf("Input throttle target: ");
		// scanf("%f", &(moveptr->throttle));
	}

	//fid = munmap(NULL, SHM_SIZE); //unmaps when finished 

return 0;

}
