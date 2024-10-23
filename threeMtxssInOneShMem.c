#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

int main() {
    int status1, status2, status3;
    int n = 3;
    int start2 = n * n;
    int start3 = start2 * 2;
    int *shMem = (int*)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid_t ch1 = fork();

    if (ch1 == 0) { // child 1
        kill(getpid(), SIGSTOP);
        
        for (int i = 0; i < n; ++i) {
            shMem[start3 + i] = shMem[i] + shMem[start2 + i];
        }
        
    } else if (ch1 > 0) { // parent 1
        printf("\nValue first %dx%d matrix \n", n, n);

        for (int i = 0; i < 2 * (n * n); ++i) {
            if (i == n * n) {
                printf("Value second %dx%d matrix \n", n, n);
            }

            scanf("%d", &shMem[i]);
        }

        kill(ch1, SIGCONT);

        pid_t ch2 = fork();

        if (ch2 == 0) { // child 2
            for (int i = n; i < 2 * n; ++i) {
                shMem[start3 + i] = shMem[i] + shMem[start2 + i];
            }

        } else if (ch2 > 0) { // parent 2
            pid_t ch3 = fork();

            if (ch3 == 0) { // child 3
                for (int i = 2 * n; i < 3 * n; ++i) {
                    shMem[start3 + i] = shMem[i] + shMem[start2 + i];
                }

            } else if (ch3 > 0) { // parent 3
                wait(&status1);
                wait(&status2);
                wait(&status3);
                
                printf("\nResulting %dx%d matrix", n, n);
                for (int i = start3; i < start3 + start2 ; ++i) {
                    if (i % n == 0) {
                        putchar(10);
                    }
                    printf("%d ", shMem[i]);
                }

               putchar(10);
            }
        } 
    }

    munmap(shMem, 4096);
    return 0;
}
