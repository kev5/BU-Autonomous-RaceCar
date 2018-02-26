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

       int c;
       initscr();  // creates interface
       cbreak(); // Allows user to exit
       // echo(); // Echoes commands
       keypad(stdscr, TRUE); // stdscr default window
       printf("use arrow keys to control throttle and steering!\n");
	
       int fid = shm_open("racecar", O_RDWR, 0666);
       if (fid == -1){
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
	  int c = getch();
	  switch (c) {
	  case KEY_UP: // pressing up arrow makes car goes forwards
	    moveptr->throttle += .1;
	    printf("%f",moveptr->throttle);
	    break;
	  case KEY_DOWN: //makes the car go backwards 
	    moveptr->throttle -= .1;
	    printf("%f",moveptr->throttle);
	    break;
	  case KEY_RIGHT: // steering servo goes one increment to the right
	    moveptr->steer += .1;
	    printf("%f",moveptr->steer);
	    break;
	  case KEY_LEFT: //steering servo goes one increment to the left
	    moveptr->steer -= .1;
	    printf("%f",moveptr->steer);
	    break;
	  }
	  //}
	  
	  //while(1) {
	  
	  // printf("Input throttle target: ");
	  // scanf("%f", &(moveptr->throttle));
	}

	//fid = munmap(NULL, SHM_SIZE); //unmaps when finished 

return 0;

}
