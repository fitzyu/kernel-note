/*
 * 使用 ./a.out & 让程序在后台运行 然后执行ps查出PID，然后执行kill -USR1 pid号
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void MySignalFunc(int signum)
{
	static int cnt = 0;
	printf("signum %d, %d times\n", signum, ++cnt);
	return;
}

int main(int argc, char *argv[])
{
	signal(SIGUSR1,MySignalFunc);
	while (1) {
		sleep(1000);
	}

	return 0;
}

