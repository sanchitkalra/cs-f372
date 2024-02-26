// #include <Kernel/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

char *itoa(int val, int base) {
    static char buf[32] = {0};

    int i = 30;

    for (; val && i; --i, val /= base) buf[i] = "0123456789abcdef"[val % base];

    return &buf[i + 1];
}

int is_int(char const *p) { return strcmp(itoa(atoi(p), 10), p) == 0; }

int main() {
    int itemsInMenu = 4;
    int order[itemsInMenu];

    int waiterID;

    printf("Enter Waiter ID: ");
    scanf("%d", &waiterID);

    char *key_seed = (char *)malloc(4 * sizeof(char));
    sprintf(key_seed, "WT_%d", waiterID);

    int key = ftok(key_seed, 'B');

    int shmid = shmget(key, sizeof(int) * (itemsInMenu + 3), 0666 | IPC_CREAT);

    int *a;
    a = (int *)shmat(shmid, NULL, 0);

    while (*(a + 4) != 1) {
    }

    // parse to find total order amt

    int totalAmt = 0;

    FILE *fptr;
    fptr = fopen("menu.txt", "r");
    char fileContent[1024];
    int i = 0;
    char *delim = " ";
    while (fgets(fileContent, 1024, fptr)) {
        char *ptr = strtok(fileContent, delim);
        while (ptr != NULL) {
            // printf("%s is digit: %d\n", ptr, is_int(ptr));
            if (is_int(ptr)) {
                totalAmt += (*(a + i)) * atoi(ptr);
            }
            ptr = strtok(NULL, delim);
        }
        i++;
    }
    fclose(fptr);

    *(a + itemsInMenu - 1 + 2) = totalAmt;
    *(a + itemsInMenu - 1 + 3) = 1;

    shmdt((void *)a);

    return 0;
}