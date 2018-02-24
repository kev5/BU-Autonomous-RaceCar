#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifdef _WIN32
#define 0_NOCTTY 0
#else
#include <termios.h>
#endif

// Masestro serial mode must be set to "USB dual Port"
// fd (file descriptor): used to access files
// struct: defines a physically grouped list of variables to be placed under one block of memeory
// tcgetattr: gets parameters associated with the terminal 

void getErrors(int fd)
{  

  unsigned char serialBytes [] = {0xAA, 0x0C, 0x21};
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
  // the status tab of the Maestro Control Center 

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
  
int main(){
  
  const char * device = "/dev/ttyACM0"; // linux
  
  int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
  if(fd < 0){
    perror(device);
    return 1;

  }

  // printf("%d\n", fd);
  
#ifdef _WIN32
  _setmode(fd. _O_BINARY);
#else

  speed_t baud = B115200;
  set_interface_attribs(fd, baud, 0);
#endif

  // Example/test code that is given 
  printf("Device opened. Fetching position \n");
  
  int position = getPosition(fd, 4); // Currently in channel 4
  printf("Current Position: %d.\n", position);

  int target = (position < 6000) ? 9000 : 3000;

  printf("Setting target to %d (%d us).\n", target, target/4);

  int i;
  for (i = 3000; i < 9000; i += 100) {
    usleep(100000);
    setTarget(fd, i, 4);
  }

  getErrors(fd);
  
  close(fd);
  return 0;  
  
}
