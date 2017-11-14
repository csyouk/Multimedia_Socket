#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP_ADDR "192.168.10.3"
#define PORT_ADDR 25000
#define MAX_BUF 256

pid_t pid;

int main(int argc, char **argv)
{
	int ret;
	int len;
	int sfd;
	struct sockaddr_in addr;
	char buf[MAX_BUF];

	printf("[%d] running %s\n", pid = getpid(), argv[0]);

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
	addr.sin_port = htons(PORT_ADDR);
	len = sizeof(addr);

	ret = connect(sfd, (struct sockaddr *)&addr, len);
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("[%d] connected\n", pid);

	for(;;) {
		printf("[%d] Input message(Q for quit): ", pid);
		if(fgets(buf, MAX_BUF, stdin)) {
			if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
				break;
			}
			write(sfd, buf, strlen(buf));
		}
		len = read(sfd, buf, MAX_BUF);
		if(len > 0) {
			buf[len] = 0;
			printf("[%d] Received: %s", pid, buf);
		}
	}
	close(sfd);
	printf("[%d] terminated\n", pid);

	return EXIT_SUCCESS;
}

