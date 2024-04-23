#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define LKP_ENC_WRITE_SEED 0x40047101
#define LKP_ENC_READ_SEED 0x80047102
#define LKP_ENC_WRITE_STRING 0x40087103
#define LKP_ENC_READ_STRING 0x80087104

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s <seed> <string>\n", argv[0]);
        return -1;
    }

    int fd = open("/dev/lkp_enc", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return -1;
    }

    unsigned int seed = atoi(argv[1]);
    char orig[256], app[256];

    strncpy(orig, argv[2], 256);

    if(ioctl(fd, LKP_ENC_WRITE_SEED, &seed)) {
        perror("ioctl seed");
        return -1;
    }

    if(ioctl(fd, LKP_ENC_WRITE_STRING, orig)) {
        perror("ioctl write string");
        return -1;
    }

    if(ioctl(fd, LKP_ENC_READ_STRING, app)) {
        perror("ioctl read string");
        return -1;
    }

    printf("Original string %s\n"
           "Encrypted string %s\n", orig, app);

    close(fd);
    return 0;
}
