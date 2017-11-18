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

#define PORT_ADDR 25000
#define CLIENT_IP_ADDR "192.168.10.2"
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
	struct sockaddr_in addr_server;
	struct sockaddr_in addr_client;
	int client_len;
	char buf[MAX_BUF];

	printf("[%d] thread started with arg \"%s\"\n", pid, (char *)arg);

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if(ret != 0) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&addr_server, 0, sizeof(addr_server));
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(PORT_ADDR);
	ret = bind(sfd, (struct sockaddr *)&addr_server, sizeof(addr_server));
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	connect_state = CONNECT_STATE_CONNECTED;
	for(;;) {
		client_len = sizeof(struct sockaddr);
		len = recvfrom(sfd, (void *)buf, MAX_BUF, 0, (struct sockaddr *)&addr_client, &client_len);
		if(len > 0) {
			buf[len] = 0;
			printf("\n");
			printf("[%d] Received: %s", pid, buf);
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
	struct sockaddr_in addr_client;
	char buf[MAX_BUF];

	printf("[%d] running %s\n", pid = getpid(), argv[0]);

	printf("[%d] creating thread\n", pid);
	ret = pthread_create(&thread_id, NULL, network_thread, "Network Thread");
	if(ret != 0) {
		printf("[%d] error: %d (%d)\n", pid, ret, __LINE__);
		return EXIT_FAILURE;
	}

	memset(&addr_client, 0, sizeof(addr_client));
	addr_client.sin_family = AF_INET;
	addr_client.sin_addr.s_addr = inet_addr(CLIENT_IP_ADDR);
	addr_client.sin_port = htons(PORT_ADDR);
	for(;;) {
		if(connect_state != CONNECT_STATE_CONNECTED) continue;
		printf("[%d] Input message(Q for quit): ", pid);
		fflush(stdout);
		if(fgets(buf, MAX_BUF, stdin)) {
			fflush(stdin);
			if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
				break;
			}
			sendto(sfd, (void *)buf, strlen(buf), 0, (struct sockaddr *)&addr_client, sizeof(addr_client));
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
