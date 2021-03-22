#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

int main()
{
    long long sz;

    int buf[100];
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        sz = write(fd, write_buf, strlen(write_buf));
        printf("Writing to " FIB_DEV ", returned the sequence %lld\n", sz);
    }

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 100 * sizeof(int));
        printf("Reading from " FIB_DEV " at offset %d, returned the sequence ",
               i);
        for (int j = sz - 1; j >= 0; j--) {
            printf("%d", buf[j]);
        }
        printf(".\n");
        memset(buf, 0, sizeof(int) * 100);
    }

    for (int i = offset; i >= 0; i--) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 100 * sizeof(int));
        printf("Reading from " FIB_DEV " at offset %d, returned the sequence ",
               i);
        for (int j = sz - 1; j >= 0; j--) {
            printf("%d", buf[j]);
        }
        printf(".\n");
        memset(buf, 0, sizeof(int) * 100);
    }

    /*for (int i = offset; i >= 0; i--) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
    }*/

    close(fd);
    return 0;
}
