#include <stdio.h>
#include <unistd.h>

int main() {
    int av = 10;
    int pid = fork();
    if (pid == 0) {
        av += 10;
    } else {
        sleep(1);
        printf("%d", av);
    }
    return 0;
}