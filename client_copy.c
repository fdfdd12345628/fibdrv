#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

int main()
{
    int buf[100];
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        long long sz;
        sz = write(fd, write_buf, strlen(write_buf));
        long long kernel_time = sz;
        printf("Writing to " FIB_DEV ", returned the sequence %lld\n", sz);
        lseek(fd, i, SEEK_SET);
        struct timespec start, end;
        double time_used;

        // 計算開始時間
        clock_gettime(CLOCK_MONOTONIC, &start);

        sz = read(fd, buf, 100 * sizeof(int));
        clock_gettime(CLOCK_MONOTONIC, &end);
        printf("Reading from " FIB_DEV " at offset %d, returned the sequence ",
               i);
        for (int j = sz - 1; j >= 0; j--) {
            printf("%d", buf[j]);
        }
        printf(".\n");
        struct timespec temp = diff(start, end);
        time_used = (double) temp.tv_nsec;
        printf("kernel: %lld, user: %lf, diff: %lf\n", kernel_time, time_used,
               time_used - kernel_time);
        memset(buf, 0, sizeof(int) * 100);
    }

    /*for (int i = 0; i <= offset; i++) {
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
    }*/

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
