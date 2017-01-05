#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LEN 256
static char recv[BUFFER_LEN] = {0};

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	char sentStr[BUFFER_LEN] = {0};
	printf("Starting test of a char device!\n");
	int ret = -1, dev = -1;
	dev = open("/dev/izchar", O_RDWR);
	if (dev < 0) {
		perror("Failed to open the device!\n");
		return errno;
	}

	printf("Type a short message to send to the kern module!\n");
	scanf("%[^\n]%*c", sentStr);
	ret = write(dev, sentStr, strlen(sentStr));
	if (ret < 0) {
		perror("Failed to write to kernel module!\n");
		return errno;
	}
	printf("Press ENTER to read back from modyule!\n");
	getchar();
	printf("Reading back from device!\n");
	ret = read(dev, recv, BUFFER_LEN);
	if (ret < 0) {
		perror("Failed to read message from module!\n");
		return errno;
	}
	printf("The message recieved is: [%s]\n", recv);
	printf("PROGRAM END!\n\n");
	return 0;
}

