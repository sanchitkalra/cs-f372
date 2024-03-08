#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

char *itoa(int val) {
    static char buf[32] = {0};

    int i = 30;

    for (; val && i; --i, val /= 10) buf[i] = "0123456789"[val % 10];

    return &buf[i + 1];
}

int is_int(char const *p) { return strcmp(itoa(atoi(p)), p) == 0; }

int main() {
    int itemsInMenu = 4;
    int order[itemsInMenu];

    int waiterID;

    printf("Enter Waiter ID: ");
    scanf("%d", &waiterID);

    char *key_seed = (char *)malloc(4 * sizeof(char));
    sprintf(key_seed, "WT_%d", waiterID);

    int key = ftok(key_seed, 'B');

    int shmid = shmget(key, sizeof(int) * (57), 0666 | IPC_CREAT);

    int *ptr;
    ptr = (int *)shmat(shmid, NULL, 0);

    int *controlPointers = ptr;
    int run = 1;
    while (run) {
        while (controlPointers[6] == 0) {
            printf("");
        }

        if (controlPointers[6] > 0) {
            printf("Exiting.");
            // signal to exit
            run = 0;
            break;
        }

        FILE *fptr;
        fptr = fopen("menu.txt", "r");
        char fileContent[1024];
        int i = 0;
        char *delim = " ";
        int prices[itemsInMenu];
        while (fgets(fileContent, 1024, fptr)) {
            char *ptr = strtok(fileContent, delim);
            while (ptr != NULL) {
                // printf("%s is digit: %d\n", ptr, is_int(ptr));
                if (is_int(ptr)) {
                    // totalAmt += ((a)->data) * atoi(ptr);
                    prices[i] = atoi(ptr);
                }
                ptr = strtok(NULL, delim);
            }
            i++;
        }
        fclose(fptr);

        while (controlPointers[0] <= 0) {
        }

        int customers = controlPointers[0];
        int itemsOrderedCustomers[customers];

        for (int k = 0; k < customers;) {
            // printf("cp-out %d, %d", controlPointers[1], controlPointers[2] );
            while (controlPointers[1] != k && controlPointers[2] < 0) {
                // printf("cp %d, %d , %d" , controlPointers[1],
                // controlPointers[2], customers );
            }

            int itemsOrderedN = controlPointers[2];

            int *orderPtr = ptr + 10 * k + 7;

            int flag = 0;

            while (itemsOrderedN--) {
                if (orderPtr[itemsOrderedN] <= 0 ||
                    orderPtr[itemsOrderedN] > itemsInMenu) {
                    controlPointers[3] = -1;
                    printf("");
                    flag = 1;
                    break;
                }
            }

            controlPointers[3] = 1;

            itemsOrderedCustomers[k] = controlPointers[2];

            if (flag == 0)
                k++;
            else
                k = 0;
        }

        char *key_seed = (char *)malloc(9 * sizeof(char));
        sprintf(key_seed, "manager_%d", waiterID);

        int key3 = ftok(key_seed, 'B');

        // key_t key3 = ftok("manager", 'C');  //Connect to Waiter
        int shmid4 = shmget(key3, 1024, 0666 | IPC_CREAT);
        int *ptr2 = (int *)shmat(shmid4, NULL, 0);

        // valid order for whole table rec
        // calc bill now
        int *orderPtr = ptr + 7;
        int cust = 0;
        i = 0;
        int totalAmt = 0;

        while (cust < customers) {
            while (i < itemsOrderedCustomers[cust]) {
                totalAmt += prices[orderPtr[10 * cust + i] - 1];
                i++;
            }
            i = 0;
            cust++;
        }

        // printf("total bill (waiter): %d\n", totalAmt);
        controlPointers[4] = totalAmt;
        *ptr2 = totalAmt;
        controlPointers[5] = 1;
    }

    shmctl(shmid, IPC_RMID, NULL);
    shmdt((void *)ptr);

    return EXIT_SUCCESS;
}
