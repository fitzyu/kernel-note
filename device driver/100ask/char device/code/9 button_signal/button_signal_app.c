/*
 * 后台执行程序
 */
#include <stdio.h>
#include <fcntl.h> // O_RDWR
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define DEVICE_NAME "/dev/fitz_led"

int fd;

void MySignalFunc(int signum)
{
	unsigned char keyValue;
	read(fd, &keyValue, 1);
	printf("keyValue = %d\n", keyValue);
}

/*
 * led on
 * led off
 */
int main(int argc, char *argv[])
{
	unsigned char keyValue;
	int ret;
	int oflags;

	signal(SIGIO,MySignalFunc);

	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0) {
		printf("open %s failed\n", DEVICE_NAME);
		return 0;
	}
	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oflags | FASYNC);

	while (1) {
		sleep(1000);
	}
	return 0;
}

