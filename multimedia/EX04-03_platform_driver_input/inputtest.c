#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>

#define APP_NAME "inputtest"

int main(int argc, char *argv[])
{
	int fd, ret;
	char *dev_name;
	struct input_event ev;	

	if(argc != 2) {
		printf("%s: there is no input device\n", APP_NAME);
		return EXIT_FAILURE;
	}

	dev_name = argv[1];

	fd = open(dev_name, O_RDONLY);
	if(fd < 0) {
		printf("%s: can't open %s\n", APP_NAME, dev_name);
		return EXIT_FAILURE;
	}
	printf("%s: %s opened\n", APP_NAME, dev_name);
	fflush(stdout);

	for(;;) {
		ret = read(fd, &ev, sizeof(struct input_event));
		if(ret == -1) {
			printf("%s: read error\n", APP_NAME);
			return EXIT_FAILURE;
		}
		printf("%s: ev=%d, code=%3d, value=%d\n", APP_NAME, ev.type, ev.code, ev.value);
	}

	close(fd);
	printf("%s: %s closed\n", APP_NAME, dev_name);
	fflush(stdout);

	return EXIT_SUCCESS;
}
