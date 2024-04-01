#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

// 0x80087101 0x40047101 0x40087102  

#define GET_IOCTL       0x80087101
#define SET_KEY_IOCTL   0x40047101
#define SET_STR_IOCTL   0x40087102

int main(int argc, char *argv[]) {
    int fd = open("/dev/lkp_enc", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return -1;
    }

    char *input_string = NULL;
    int encryption_key = 0;

    // Process command line options using getopt
    int opt;
    while ((opt = getopt(argc, argv, "s:k:")) != -1) {
        switch (opt) {
            case 's':
                input_string = optarg;
                break;
            case 'k':
                encryption_key = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -s string -k key\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Check if both -s and -k options are provided
    if (input_string == NULL || encryption_key == 0) {
        fprintf(stderr, "Both -s and -k options are required.\n");
        fprintf(stderr, "Usage: %s -s string -k key\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Initial string: %s\n", input_string);

    if(ioctl(fd, SET_KEY_IOCTL, &encryption_key)) {
        perror("ioctl set key failed");
        return -1;
    }

    if(ioctl(fd, SET_STR_IOCTL, input_string)) {
        perror("ioctl set string failed");
        return -1;
    }

    long result = ioctl(fd, GET_IOCTL, input_string);
    if (result < 0) {
        perror("ioctl get failed");
        return -1;
    }

    // Print the return value
    //printf("Return code from the device: %ld\n", result);
    printf("Encrypted string: %s\n", input_string);

    close(fd);
    return 0;
}


