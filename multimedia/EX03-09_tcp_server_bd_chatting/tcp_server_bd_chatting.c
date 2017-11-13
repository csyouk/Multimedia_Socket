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
#define MAX_BUF 256

pid_t pid;

int sfd_server;
char rxbuf[MAX_BUF];
char txbuf[MAX_BUF];

void *network_thread(void *arg)
{
	int ret;
	int len;
	int sfd_client;
	struct sockaddr_in addr_server;
	struct sockaddr_in addr_client;

	struct timeval tv;
	fd_set rfd, tfd;
	int fd;

	printf("[%d] thread started with arg \"%s\"\n", pid, (char *)arg);

	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if(ret != 0) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	sfd_server = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd_server == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_server.sin_port = htons(PORT_ADDR);
	len = sizeof(addr_server);
	ret = bind(sfd_server, (struct sockaddr *)&addr_server, len);
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	ret = listen(sfd_server, 5);
	if(ret == -1) {
		printf("[%d] error: %s\n", pid, strerror(errno));
		exit(EXIT_FAILURE);
	}

	FD_ZERO(&rfd);
	FD_SET(sfd_server, &rfd);

	for(;;) {
		printf("[%d] waiting for client ...\n", pid);
		printf("[%d] press q to quit\n", pid);
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		tfd = rfd;
		ret = select(FD_SETSIZE, &tfd, NULL, NULL, &tv);
		if(ret == -1) {
			printf("[%d] error: %s\n", pid, strerror(errno));
			exit(EXIT_FAILURE);
		}

		for(fd = 0; fd < FD_SETSIZE; fd++) {
			if(!FD_ISSET(fd, &tfd)) continue;
			if(fd == sfd_server) {
				sfd_client = accept(sfd_server, (struct sockaddr *)&addr_client, &len);
				if(sfd_client == -1) {
					printf("[%d] error: %s\n", pid, strerror(errno));
					exit(EXIT_FAILURE);
				}
#if 1
				{
					int fd_temp;
					for(fd_temp = 0; fd_temp < FD_SETSIZE; fd_temp++) {
						if(!FD_ISSET(fd_temp, &rfd)) continue;
						if(fd_temp == sfd_server) continue;
						//if(fd_temp == fd) continue;
						sprintf(rxbuf, "new client entered!!!\n");
						write(fd_temp, rxbuf, strlen(rxbuf)+1);
					}
				}
#endif
				FD_SET(sfd_client, &rfd);
				printf("[%d] connected (fd=%d)\n", pid, sfd_client);
			}
			else {
				len = read(fd, rxbuf, MAX_BUF);
				if(len == 0) {
					close(fd);
					FD_CLR(fd, &rfd);
					printf("[%d] closed (fd=%d)\n", pid, fd);
#if 1
					{
						int fd_temp;
						for(fd_temp = 0; fd_temp < FD_SETSIZE; fd_temp++) {
							if(!FD_ISSET(fd_temp, &rfd)) continue;
							if(fd_temp == sfd_server) continue;
							//if(fd_temp == fd) continue;
							sprintf(rxbuf, "a client exited!!!\n");
							write(fd_temp, rxbuf, strlen(rxbuf)+1);
						}
					}
#endif
				}
				else if(len > 0) {
					rxbuf[len] = 0;
#if 1
					{
						int fd_temp;
						for(fd_temp = 0; fd_temp < FD_SETSIZE; fd_temp++) {
							if(!FD_ISSET(fd_temp, &rfd)) continue;
							if(fd_temp == sfd_server) continue;
							if(fd_temp == fd) continue;
							write(fd_temp, rxbuf, len);
						}
					}
#endif

				}
			}
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
		fgets(txbuf, MAX_BUF, stdin);
		fflush(stdin);
		if(!strcmp(txbuf, "q\n") || !strcmp(txbuf, "Q\n")) {
			break;
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

	if(sfd_server) close(sfd_server);

	printf("[%d] terminted\n", pid);

	return EXIT_SUCCESS;
}
