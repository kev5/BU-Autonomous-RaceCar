//
// Created by Nicholas Arnold on 3/28/18.
//

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <semaphore.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {
	bool throttle_active, steer_active;
	char t, s;
	throttle_active = false;
	steer_active = false;
	printf("Throttle PID active? (y/n): ");
	scanf("%c", &t);
	getchar();
	printf("Steering PID active? (y/n): ");
	scanf("%c", &s);
	if (t == 'y') {
		throttle_active = true;
	}
	if (s == 'y') {
		steer_active = true;
	}
	printf("Steering: %d Throttle: %d", throttle_active, steer_active);

	return 0;
}