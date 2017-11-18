#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP_ADDR "192.168.10.3"
#define PORT_ADDR 25000
#define MAX_BUF 256

enum {
	CONNECT_STATE_NONE,
	CONNECT_STATE_CONNECTED,
};

pid_t pid;
int g_count;

int sfd;
int connect_state;
char rxbuf[MAX_BUF];
char txbuf[MAX_BUF];

void *network_thread(void *arg)
{
	int ret;
	int len;
	struct sockaddr_in addr;

	printf("[%d] thread started with arg \"%s\"\n", pid, (char *)arg);

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if(ret != 0) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
	addr.sin_port = htons(PORT_ADDR);
	len = sizeof(addr);

	ret = connect(sfd, (struct sockaddr *)&addr, len);
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}
	connect_state = CONNECT_STATE_CONNECTED;
	printf("[%d] connected\n", pid);
	printf("[%d] Input message(Q for quit): ", pid);

	for(;;) {
		len = read(sfd, rxbuf, MAX_BUF);
		if(len > 0) {
			rxbuf[len] = 0;
			printf("\n");
			printf("[%d] Received: %s", pid, rxbuf);
			printf("[%d] Input message(Q for quit): ", pid);
			fflush(stdout);
		}
	}

	pthread_exit("Goodbye Thread");
}

int main(int argc, char **argv)
{
	pthread_t thread_id;
	void *thread_ret;
	int ret;

	printf("[%d] running %s\n", pid = getpid(), argv[0]);

	printf("[%d] creating thread\n", pid);
	ret = pthread_create(&thread_id, NULL, network_thread, "Network Thread");
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}

	for(;;) {
		if(connect_state != CONNECT_STATE_CONNECTED) continue;
		if(fgets(txbuf, MAX_BUF, stdin)) {
			fflush(stdin);
			if(!strcmp(txbuf, "q\n") || !strcmp(txbuf, "Q\n")) {
				break;
			}
			write(sfd, txbuf, strlen(txbuf));
			printf("[%d] Input message(Q for quit): ", pid);
		}
	}

	ret = pthread_cancel(thread_id);
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}

	printf("[%d] waiting to join with a terminated thread\n", pid);
	ret = pthread_join(thread_id, &thread_ret);
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}
	if(!thread_ret) {
		printf("[%d] thread returned \"%s\"\n", pid, (char *)thread_ret);
	}

	close(sfd);

	printf("[%d] terminted\n", pid);

	return EXIT_SUCCESS;
}
