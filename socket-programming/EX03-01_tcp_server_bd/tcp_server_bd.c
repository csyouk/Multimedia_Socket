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

#define PORT_ADDR 25000
#define MAX_BUF 256

pid_t pid;

int main(int argc, char **argv)
{
	int ret;
	int len;
	int sfd_server, sfd_client;
	struct sockaddr_in addr_server;
	struct sockaddr_in addr_client;
	char buf[MAX_BUF];

	printf("[%d] running %s\n", pid = getpid(), argv[0]);

	sfd_server = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd_server == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}

	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(PORT_ADDR);
	len = sizeof(addr_server);
	ret = bind(sfd_server, (struct sockaddr *)&addr_server, len);
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}

	ret = listen(sfd_server, 5);
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}

	printf("[%d] waiting for client ...\n", pid);
	sfd_client = accept(sfd_server, (struct sockaddr *)&addr_client, &len);
	if(sfd_client == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("[%d] connected\n", pid);

	for(;;) {
		len = read(sfd_client, buf, MAX_BUF);
		if(len > 0) {
			buf[len] = 0;
			printf("[%d] Received: %s", pid, buf);
		}

		printf("[%d] Input message(Q for quit): ", pid);
		if(fgets(buf, MAX_BUF, stdin)) {
			if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
				break;
			}
			write(sfd_client, buf, strlen(buf));
		}
	}
	close(sfd_client);
	close(sfd_server);

	printf("[%d] terminated\n", pid);

	return EXIT_SUCCESS;
}

