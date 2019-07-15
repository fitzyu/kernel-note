#include <stdio.h>
#include <fcntl.h> // O_RDWR
#include <poll.h>

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
	int ret;
	struct pollfd *fds;

	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0) {
		printf("open %s failed\n", DEVICE_NAME);
		return 0;
	}

	fds->fd = fd;
	fds->events = POLLIN;

	while (1) {
		ret = poll(fds, 1, 5000);
		if (ret == 0) {
			printf("timeout \n");
		} else {
			read(fd, &keyValue, 1);
			printf("key_val = %d\n", keyValue);
		}
	}
	return 0;
}

