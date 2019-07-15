#include <stdio.h>
#include <fcntl.h> // O_RDWR

#define DEVICE_NAME "/dev/fitz_led"

/*
 * led on
 * led off
 */
int main(int argc, char *argv[])
{
	int fd;
	int val = 1;
	unsigned char keyValue;

	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0) {
		printf("open %s failed\n", DEVICE_NAME);
		return 0;
	}

	while (1) {
		read(fd, &keyValue, 1);
 		printf("key_val = %d\n", keyValue); 
		
	}
	return 0;
}

