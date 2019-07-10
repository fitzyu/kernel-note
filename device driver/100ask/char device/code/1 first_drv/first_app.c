#include <stdio.h>
#include <fcntl.h> // O_RDWR

#define DEVICE_NAME "/dev/first_drv"

int main(int argc, char *argv)
{
	int fd;
	int val = 1;

	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0)
		printf("open %s failed\n", DEVICE_NAME);

	write(fd, &val, 4);
	return 0;
}
