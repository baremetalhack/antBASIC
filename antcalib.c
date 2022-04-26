#include <stdio.h>          // printf()
#include <sys/time.h>       // *_t, timeval{}, gettimeofday()
#include <stdlib.h>         // atoi()

void microsec_wait(int count) {
    volatile int i;

    for (i = 0; i < count; i++) {
    };
}

int main(int argc, char* argv[]) {
    struct timeval start, end;
    time_t sec;
    suseconds_t usec;
    float usecs;
    int count, loops, i;

    if (argc != 3) {
        printf("Usage: antcalib loopcount loops\n");
        return 1;
    }

    count = atoi(argv[ 1 ]);
    loops = atoi(argv[ 2 ]);
    printf("Loopcount = %d\n", count);
    printf("Number of loops = %d\n\n", loops);
    gettimeofday(&start, NULL);
    for (i = 0; i < loops; i++)
        microsec_wait(count);
    gettimeofday(&end, NULL);
    sec = end.tv_sec - start.tv_sec;
    if (end.tv_usec < start.tv_usec) {
        sec--;
        usec = 1000000 + end.tv_usec - start.tv_usec;
    } else
        usec = end.tv_usec - start.tv_usec;
    usecs = sec * 1000000 + usec;
    printf("Elapsed time --> %ld sec %ld usec\n", sec, usec);
    printf("Mean time --> %f usec/loop\n", usecs / loops);
    return 0;
}
