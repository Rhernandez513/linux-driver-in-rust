#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define RKEY_IOCTL 0x80047101
#define WKEY_IOCTL 0x40047102

#define RCH_IOCTL  0x80047103
#define WCH_IOCTL  0x40047104


#define MAX_LKP_ENC_BUFFER_SIZE 128

int main() {
    unsigned int usr_encryption_key = 0x2;
    char usr_str_data[MAX_LKP_ENC_BUFFER_SIZE] = "Hello World";

    printf("USERSPACE: Initial Encryption key: %u\n", usr_encryption_key);
    printf("USERSPACE: Initial String data: %s\n", usr_str_data);

    int fd = open("/dev/lkp_enc_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return -1;
    }
    printf("USERSPACE: Device file opened successfully\n");

    printf("USERSPACE: writing encryption key to device driver\n");
    // write the encryption key
    if(ioctl(fd, WKEY_IOCTL, &usr_encryption_key)) {
        perror("ioctl write key");
        return -1;
    }

    printf("USERSPACE: writing string to device driver\n");
    // write string to device
    if(ioctl(fd, WCH_IOCTL, usr_str_data)) {
        perror("ioctl write string");
        return -1;
    }

    printf("USERSPACE: reading string from device driver\n");
    // read string from device
    if(ioctl(fd, RCH_IOCTL, &usr_str_data)) {
        perror("ioctl read string");
        return -1;
    }
    printf("USERSPACE: String data from device driver: %s\n", usr_str_data);

    printf("USERSPACE: reading encryption key from device driver\n");
    // read the device encryption key, should be zero
    if(ioctl(fd, RKEY_IOCTL, &usr_encryption_key)) {
        perror("ioctl read key");
        return -1;
    }
    printf("USERSPACE: Encryption key from device driver: %u\n", usr_encryption_key);
    

    close(fd);
    return 0;
}
