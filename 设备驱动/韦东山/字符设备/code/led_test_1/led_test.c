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
	int ret;

	if (argc != 2) {
		printf("Usage %s <on/off>\n", argv[0]);
		return 0;
	}

	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0) {
		printf("open %s failed\n", DEVICE_NAME);
		return 0;
	}

	ret = strcmp(argv[1], "on");
	if (ret) {
		val = 1;
	} else {
		val = 0;
	}
	write(fd, &val, 4);
	return 0;
}

