/*
 * ʹ�� ./a.out & �ó����ں�̨���� Ȼ��ִ��ps���PID��Ȼ��ִ��kill -USR1 pid��
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

