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

    int waiterID = 1;

    printf("Enter Waiter ID: ");
    scanf("%d", &waiterID);

    char *key_seed = (char *)malloc(4 * sizeof(char));
    sprintf(key_seed, "WT_%d", waiterID);

    int key = ftok(key_seed, 'B');

    int shmid = shmget(key, sizeof(int) * (57), 0666 | IPC_CREAT);

    int *ptr;
    ptr = (int *)shmat(shmid, NULL, 0);

    int *controlPointers = ptr;

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
        while (controlPointers[1] != k || controlPointers[2] < 0) {
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

    printf("total bill (waiter): %d\n", totalAmt);

    controlPointers[4] = totalAmt;
    controlPointers[5] = 1;

    shmdt((void *)ptr);

    return 0;
}

// // #include <Kernel/string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/shm.h>
// #include <unistd.h>

// #include "sll.h"
// #include "util.h"

// char *itoa(int val) {
//     static char buf[32] = {0};

//     int i = 30;

//     for (; val && i; --i, val /= 10) buf[i] = "0123456789"[val % 10];

//     return &buf[i + 1];
// }

// int is_int(char const *p) { return strcmp(itoa(atoi(p)), p) == 0; }

// int validateOrder(struct Node *head, int itemsInMenu) {
//     struct Node *temp = head;
//     // printf("\n\nList elements are - \n");
//     while (temp != NULL) {
//         // printf("%d --->", temp->data);
//         if (temp->data > itemsInMenu && temp->data <= 0) {
//             // printf("All orders are OK!!");
//             return 0;  // false
//         }
//         temp = temp->next;
//     }
//     return 1;  // true
// }

// int main() {
//     int itemsInMenu = 4;
//     int order[itemsInMenu];

//     int waiterID = 1;

//     // printf("Enter Waiter ID: ");
//     // scanf("%d", &waiterID);

//     FILE *fptr;
//     fptr = fopen("menu.txt", "r");
//     char fileContent[1024];
//     int i = 0;
//     char *delim = " ";
//     int prices[itemsInMenu];
//     while (fgets(fileContent, 1024, fptr)) {
//         char *ptr = strtok(fileContent, delim);
//         while (ptr != NULL) {
//             // printf("%s is digit: %d\n", ptr, is_int(ptr));
//             if (is_int(ptr)) {
//                 // totalAmt += ((a)->data) * atoi(ptr);
//                 prices[i] = atoi(ptr);
//             }
//             ptr = strtok(NULL, delim);
//         }
//         i++;
//     }
//     fclose(fptr);

//     char *key_seed = (char *)malloc(4 * sizeof(char));
//     sprintf(key_seed, "WT_%d", waiterID);

//     int key = ftok(key_seed, 'B');

//     int shmid = shmget(key, sizeof(struct Node) * (6), 0666 | IPC_CREAT);

//     struct Node *a = (struct Node *)shmat(shmid, NULL, 0);

//     for (int k = 0; k < 6; k++) {
//         (a + k)->data = -1;
//         (a + k)->next = NULL;
//     }

//     while ((a)->data <= 0) {
//     }

//     int numOfCustomer = (a)->data;

//     for (int k = 0; k < numOfCustomer; k++) {
//         while ((a + 2)->data != 1) {
//         }

//         // traverse and check for errors now
//         struct Node *v = a;
//         v++;

//         if (validateOrder(v, itemsInMenu) == 0) {
//             // invalid order
//             (a + 5)->data = -1;
//             k = 0;
//         }
//     }

//     // parse to find total order amt
//     int totalAmt = 0;

//     struct Node *v = a;
//     v++;
//     while (v != NULL) {
//         printf("%d amt is %d\n", v->data, v->data - 1);
//         totalAmt += prices[v->data - 1];
//         v = v->next;
//     }

//     (a + 3)->data = totalAmt;
//     (a + 4)->data = 1;

//     // (a + itemsInMenu - 1 + 2) = totalAmt;
//     // (a + itemsInMenu - 1 + 3) = 1;

//     shmdt((void *)a);

//     return 0;
// }