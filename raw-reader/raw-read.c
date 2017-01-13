#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>



int main(int argc, char** argv) 
{
	(void) argc;
	(void) argv;
	char c, *ptr=NULL; 
	int fd = open("/sys/bus/pci/drivers/8139cp/0000:00:03.0", 
			O_RDONLY|O_SYNC);
	if (fd < 0) {
		printf("Could not open (%d) : (%s)\n", fd, strerror(errno));
		exit(1);
	} else {
		printf("Opened (%d) file handle...\n");
		puts("Mapping...");
		fflush(stdout);
		ptr = (char*) mmap(0, 4096, PROT_READ, MAP_SHARED, fd, 0);
		if (!ptr) {
			puts("mmap() failed!");
			exit(1);
		}
		for(;;) {
			printf("%p", ptr);	
		}
	}
	
	exit(0);
}


