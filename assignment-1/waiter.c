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
        // while (controlPointers[6] == 0) {
        //     printf("");
        // }
        while (*(ptr + 6) == 0) {
        }

        // if (controlPointers[6] > 0) {
        //     // printf("Exiting.");
        //     // signal to exit
        //     run = 0;
        //     break;
        // }

        if (*(ptr + 6) > 0) {
            // sign to exit
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

        // while (controlPointers[0] <= 0) {
        // }

        while (*(ptr + 0) <= 0) {
        }

        // int customers = controlPointers[0];
        int customers = *(ptr + 0);
        int itemsOrderedCustomers[customers];

        for (int k = 0; k < customers;) {
            printf("k = %d\n", k);
            // printf("loop %d %d %d\n", k, controlPointers[1],
            //        controlPointers[2]);
            // while (controlPointers[1] != k || controlPointers[2] < 0) {
            //     // printf("cp %d, %d , %d" , controlPointers[1],
            //     // controlPointers[2], customers );
            // }

            // while (controlPointers[2] < 0) {
            // }
            while (*(ptr + 2) < 0) {
            }

            // int itemsOrderedN = controlPointers[2];
            int itemsOrderedN = *(ptr + 2);
            printf("itms %d\n", itemsOrderedN);

            int *orderPtr = ptr + 7 + (10 * k);

            int flag = 0;

            int j = 0;
            while (j < itemsOrderedN) {
                printf("validating for k = %d & i/p = %d \n", k,
                       *(ptr + 7 + (10 * k) + j));
                if (*(ptr + 7 + (10 * k) + j) > itemsInMenu ||
                    *(ptr + 7 + (10 * k) + j) <= 0) {
                    printf("invalid order for %d\n", j);
                    flag = 1;
                    // controlPointers[3] = -1;
                    break;
                }
                j++;
            }

            // while (itemsOrderedN--) {
            //     if (orderPtr[itemsOrderedN] <= 0 ||
            //         orderPtr[itemsOrderedN] > itemsInMenu) {
            //         controlPointers[3] = -1;
            //         printf("invalid order %d\n", orderPtr[itemsOrderedN]);
            //         flag = 1;
            //         break;
            //     }
            // }

            itemsOrderedCustomers[k] = itemsOrderedN;

            if (flag == 0) {
                k++;
                // controlPointers[3] = 1;
                *(ptr + 3) = 1;
            } else {
                k = 0;
                // controlPointers[3] = -1;
                *(ptr + 3) = -1;
            }
        }

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
        controlPointers[5] = 1;
    }

    shmctl(shmid, IPC_RMID, NULL);
    shmdt((void *)ptr);

    return EXIT_SUCCESS;
}