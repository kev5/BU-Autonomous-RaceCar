#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdlib.h>
#include "shared_def.h"

#define DEV_PATH  "/dev/PWM_OUT"
#define SHM_SIZE 4096
#define SERVO_LEFT 3100
#define SERVO_RIGHT 8900
#define SPAN 5800
#define START -1
#define END 1
#define THROTTLE_LEFT (1458*4)
#define THROTTLE_RIGHT (1570*4)
#define SPAN_2 (THROTTLE_RIGHT - THROTTLE_LEFT)
#ifdef _WIN32
#define 0_NOCTTY 0
#define SERVOSEM "/servosemaphore"
#else
#include <termios.h>
#endif

// Masestro serial mode must be set to "USB dual Port"
// fd (file descriptor): used to access files
// struct: defines a physically grouped list of variables to be placed under one block of memeory
// tcgetattr: gets parameters associated with the terminal f

void getErrors(int fd)
{  

    unsigned char serialBytes [] = {0xAA, 0x4C, 0x21};
    int len;

    if(write(fd, serialBytes, sizeof(serialBytes)) == -1){    
        perror("write error");
        return;
    }

    unsigned char response[2];
    if((len = read(fd, response, 2)) != 2){
        response[len] = 0;
        perror("read error");
        printf("%d [%s]\n", len, response);
        return;
    }

    printf("Errors: 0x%02x %02x\n", response[0], response[1]);

}

int setTarget(int fd, unsigned short target, unsigned char channel){

    // Target is in quarter-microseconds
    // Set target (0x84), channel number, lower 7 bits of target, bits 7-13 of target

    unsigned char serialBytes[]= {0xAA, 0x0C, 0x04, channel, target & 0x7F, target >> 7 & 0x7F};
    int len;

    // Write: writes data from a buffer declared by the user to a given device. Outputs data from a program

    if((len = write(fd, serialBytes, sizeof(serialBytes))) == -1){

        perror("error");
        return -1;
    }

    printf("Written: %d bytes\n", len);

    return 0;

}

int sendInit(int fd){

    unsigned char command [] = {0xAA};

    if(write(fd, command, sizeof(command)) == -1){
        perror("error");
        return -1;
    }

    return 0;
}

int getPosition(int fd, unsigned char channel){

    // Compact protocol: 0x90, channel number
    // The position value returned by this command is four times what is displayed in the position box in

    unsigned char serialBytes [] = {0xAA, 0x0C, 0x10, channel};
    int len;

    if(write(fd, serialBytes, sizeof(serialBytes)) == -1){

        perror("write error");
        return -1;
    }

    unsigned char response[2];
    if((len = read(fd, response, 2)) != 2){
        response[len] = 0;
        perror("read error");
        printf("%d [%s]\n", len, response);
        return -1;
    }

    return response[0] + 256*response[1];

}

int set_interface_attribs (int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        perror ("Get attribute error");
        return -1;
    }

    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
        perror ("Set attribute error");
        return -1;
    }
    return 0;
}

void servo(struct throttle_steer *ptr){
    const char * device = DEV_PATH; // linux
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);

    float steerval; //declare local variables to save steer and throttle values while the function won't have the semaphore for an excessively long time
    float throttleval;

    if(fd < 0){
        perror(device);
    }

    sem_t *sem = sem_open(SERVOSEM, O_CREAT, 0777, 1);
    if (sem == SEM_FAILED){
        perror("semaphore open error in servo\n");
    }

    while(1){

        sem_wait(sem);
        steerval = ptr->steer;
        throttleval = ptr->throttle;
        sem_post(sem);

        printf("%f",steerval); // prints steering input stored in shared memory object                                                                                        
        float position = getPosition(fd, 4); // Currently in channel 4
        printf("Current Position: %f\n", position);

        if(steerval > 1){ //makes sure input stays within boundaries
            steerval = 1;
        }
        if(steerval < -1){
            steerval = -1;
        }

        float target = SERVO_LEFT + ((steerval - START) / (END - START))*SPAN; //converts input in range [-1 1] to PWM values
        if(position != target){
            printf("\nsteer target is %f\n",target);
            setTarget(fd, target, 4);
        }

        /////////////////// throttle ////////////////////////////

        float position2 = getPosition(fd, 5);
        printf("%f",throttleval);

        //makes sure input stays within boundaries
        if(throttleval > 1){
            throttleval = 1;
        }
        if(throttleval < -1){
            throttleval = -1;
        }

        float target2 = THROTTLE_LEFT + ((throttleval - START) / (END - START))*SPAN_2;
        if(position2 != target2){
            printf( "\nthrottle target is %f\n", target2);
            setTarget(fd, target2, 5);
        }
    }
}


int main(){

    const char * device = DEV_PATH; // linux

    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd < 0){
        perror(device);
        return 1;
    }

#ifdef _WIN32
    _setmode(fd. _O_BINARY);
#else

    speed_t baud = B115200;
    set_interface_attribs(fd, baud, 0);
#endif

    struct throttle_steer * moveptr; 

    int fid;
    const char *name = "racecar";
    fid = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fid <  0)
    {
        perror("shm_open error");
        return -1;
    }

    ftruncate(fid, SHM_SIZE);

    void * ptr = mmap(NULL, SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fid, 0);
    if (ptr == MAP_FAILED)
    {
        perror("error with mapping");
        return -1;
    }

    /* Mapping successful */
    moveptr = (struct throttle_steer *)ptr;

    //call function that send PWM

    servo(moveptr);

    //fiid = munmap(NULL, SHM_SIZE); //unmaps when finished 

    getErrors(fd);

    close(fid);
    close(fd);
    return 0;

}
