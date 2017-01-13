#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>


#define PRINT_ERROR do { \
    fprintf(stderr, "Error at line %d file %s (%d) [%s]\n", \
    __LINE__, __FILE__, errno, strerror(errno)); \
    exit(1); \
    } while (0);


#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int main(int argc, char *argv[])
{
    int fd;
    void* map_base = NULL, *virt_address = NULL;
    uint32_t read_result, writeval;
    char* filename = NULL;
    off_t target;
    int access_type = 'w';

    if (argc < 3) {
        //pci-reader /sys/bus/pci/devices/0001\:00\:07.0/resource0 0x100 w 0x00
        // argv[0] [1]
        fprintf(stderr, "\nUsage: \t%s {sys file} {offset} [type [data] ]\n"
                "\tsys file : sysfs file for the pci resource on act on\n"
                "\toffset   : offset into pci memory region to act upon\n"
                "\ttype     : access operation type [b]yte, [h]alfword, [w]ord\n"
                "\tdata     : data to be written\n\n",
                argv[0]);
        exit(1);
    }

    filename = argv[1];
    target = strtoul(argv[2], 0, 0);


    if (argc > 3) {
        access_type = tolower(argv[3][0]);
    }

    if ((fd = open(filename, O_RDWR | O_SYNC)) == -1) PRINT_ERROR;

    printf("%s openede.\n", filename);
    printf("Target offset is 0x%x page size is %ld\n", (int)target, sysconf(_SC_PAGE_SIZE));
    fflush(stdout);

    /* map one page */
    printf("mmap(%d, %ld, 0x%x, %d, 0x%x)\n", MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
           fd, (int)target);

    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                    fd, target & ~MAP_MASK);

    if (map_base == (void*)-1) PRINT_ERROR;

    printf("PCI memory mapped to address 0x%08lx.\n", (unsigned long)map_base);
    fflush(stdout);

    virt_address = map_base + (target & MAP_MASK);

    switch (access_type) {
    case 'b':
        read_result = *((uint8_t*)virt_address);
        break;
    case 'h':
        read_result = *((uint16_t*)virt_address);
        break;
    case 'w':
        read_result = *((uint32_t*)virt_address);
        break;
    default:
        fprintf(stderr, "Illegal datatype '%c'.\n", access_type);
        exit(2);
        break;
    }

    printf("Value at offset 0x%X (%p): 0x%X\n", (int)target, virt_address, read_result);
    fflush(stdout);


    if (munmap(map_base, MAP_SIZE) == -1) {
        PRINT_ERROR;
    }

    close(fd);
    return 0;
}
