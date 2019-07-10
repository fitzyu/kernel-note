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
	unsigned char keyValue[4];

	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0) {
		printf("open %s failed\n", DEVICE_NAME);
		return 0;
	}

	while (1) {
		read(fd, keyValue, sizeof(keyValue));
 
		if (!keyValue[0] || !keyValue[1] || !keyValue[2] || !keyValue[3]) {
			printf("value status key1 %d key2 %d key3 %d key4 %d \n", keyValue[0], keyValue[1], keyValue[2], keyValue[3]);
		}
	}
	return 0;
}

