#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

#define MSG_SIZE 32

int main() {
    int pipePtrs[2];  // refer to the pipes: p[0] for reader descriptor, p[1]
                      // for writer descriptor

    printf("Enter Table Number: ");
    int tableNumber;
    scanf("%d", &tableNumber);

    printf(
        "Enter Number of Customers at Table (maximum no. of customers can be "
        "5): ");
    int customers;
    scanf("%d", &customers);

    int itemsInMenu = 0;

    FILE *fptr;
    fptr = fopen("menu.txt", "r");
    char fileContent[1024];
    while (fgets(fileContent, 1024, fptr)) {
        itemsInMenu++;
    }
    fclose(fptr);

    // int order[itemsInMenu];
    // memset(order, 0, itemsInMenu * sizeof(int));

    char *key_seed = (char *)malloc(4 * sizeof(char));
    sprintf(key_seed, "WT_%d", tableNumber);

    int key = ftok(key_seed, 'B');

    int shmid = shmget(key, sizeof(int) * (57), 0666 | IPC_CREAT);
    int *ptr;
    ptr = (int *)shmat(shmid, NULL, 0);

    int *controlPointers = ptr;
    // set 0th pointer to number of customers
    controlPointers[0] = customers;
    // set control pointers which are used as flags to 0
    controlPointers[5] = -1;  // bill amt written
    controlPointers[6] = 0;   // waiter process exit

    int cost = 0;

    for (int k = 0; k < customers;) {
        // set current customer in 1st pointer
        controlPointers[1] = k;

        // set default for number of items ordered to -1
        controlPointers[2] = -1;

        // set default for order validity to 0
        controlPointers[3] = 0;

        // create the pipe
        if (pipe(pipePtrs) < 0) {
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();

        if (pid == -1) {
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // child execution
            FILE *fptr;
            fptr = fopen("menu.txt", "r");
            char fileContent[1024];
            while (fgets(fileContent, 1024, fptr)) {
                printf("%s", fileContent);
            }
            fclose(fptr);

            close(pipePtrs[0]);  // Close the read end of the pipe in the child
                                 // process

            printf(
                "Enter the serial number(s) of the item(s) to order from the "
                "menu.Enter -1 when done: \n");
            int input;
            while (1) {
                scanf("%d", &input);
                if (input == -1) break;
                char orderByCustomer[MSG_SIZE];
                snprintf(orderByCustomer, MSG_SIZE, "%d", input);
                write(pipePtrs[1], orderByCustomer,
                      MSG_SIZE);  // Write order to pipe
            }
            close(pipePtrs[1]);  // Close the write end of the pipe in the child
                                 // process
            exit(EXIT_SUCCESS);
        } else {
            // parent execution
            close(pipePtrs[1]);  // Close the write end of the pipe in the
                                 // parent process

            int i = 0;  // refers to number of items ordered

            char orderByCustomer[MSG_SIZE];
            int *orderPointer = ptr + 10 * k + 7;

            while (read(pipePtrs[0], orderByCustomer, MSG_SIZE) > 0) {
                // printf("rec: %s\n", orderByCustomer);

                orderPointer[i] = atoi(&orderByCustomer[0]);
                i++;

                // // handle errors, if any
                // if (atoi(&orderByCustomer[0]) > itemsInMenu &&
                //     atoi(&orderByCustomer[0]) < 1) {
                //     // invalid item ID
                // } else {
                //     // add to itemsOrdered and update total cost
                //     // order[atoi(&orderByCustomer[0]) - 1]++;
                // }
            }
            close(pipePtrs[0]);  // Close the read end of the pipe in the parent
                                 // process
            controlPointers[2] = i;

            while (controlPointers[3] == 0) {
            }

            int flag = 0;

            if (controlPointers[3] < 0) {
                // order was invalid
                flag = 1;
            }

            if (flag == 0)
                k++;
            else
                k = 0;

            wait(NULL);  // Wait for child process to finish
        }
    }

    // wait for bill amount
    while (controlPointers[5] < 0) {
    }

    printf("The total bill amount is %d INR.\n", controlPointers[4]);

    shmctl(shmid, IPC_RMID, NULL);
    shmdt((void *)ptr);

    return EXIT_SUCCESS;
}

// #include <Kernel/string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/shm.h>
// #include <unistd.h>

// #include "sll.h"

// #define MSG_SIZE 32

// int main() {
//     int pipePtrs[2];  // refer to the pipes: p[0] for reader descriptor, p[1]
//                       // for writer descriptor

//     printf("Enter Table Number: ");
//     int tableNumber = 1;
//     // scanf("%d", &tableNumber);

//     printf(
//         "Enter Number of Customers at Table (maximum no. of customers can be
//         " "5): ");
//     int customers = 1;
//     // scanf("%d", &customers);

//     int itemsInMenu = 0;

//     FILE *fptr;
//     fptr = fopen("menu.txt", "r");
//     char fileContent[1024];
//     while (fgets(fileContent, 1024, fptr)) {
//         itemsInMenu++;
//     }
//     fclose(fptr);

//     int order[itemsInMenu];
//     memset(order, 0, itemsInMenu * sizeof(int));

//     int cost = 0;
// char *key_seed = (char *)malloc(4 * sizeof(char));
// sprintf(key_seed, "WT_%d", tableNumber);

// int key = ftok(key_seed, 'B');

//     int shmid = shmget(key, sizeof(struct Node) * (6), 0666 | IPC_CREAT);
//     struct Node *a = (struct Node *)shmat(shmid, NULL, 0);

//     for (int k = 0; k < 6; k++) {
//         (a + k)->data = -1;
//         (a + k)->next = NULL;
//     }

//     a->data = customers;

//     for (int k = 0; k < customers; k++) {
//         // create the pipe
//         if (pipe(pipePtrs) < 0) {
//             exit(EXIT_FAILURE);
//         }

//         if (k == 0) {
//             (a + 2)->data = 0;
//         }
//         (a + 5)->data = 0;

//         struct Node *head = a;  // init linkedlist
//         head++;

//         pid_t pid = fork();

//         if (pid == -1) {
//             exit(EXIT_FAILURE);
//         }

//         if (pid == 0) {
//             // child execution
//             FILE *fptr;
//             fptr = fopen("menu.txt", "r");
//             char fileContent[1024];
//             while (fgets(fileContent, 1024, fptr)) {
//                 printf("%s", fileContent);
//             }
//             fclose(fptr);

//             close(pipePtrs[0]);  // Close the read end of the pipe in the
//             child
//                                  // process

//             printf(
//                 "Enter the serial number(s) of the item(s) to order from the
//                 " "menu.Enter -1 when done: \n");
//             int input;
//             while (1) {
//                 scanf("%d", &input);
//                 if (input == -1) break;
//                 char orderByCustomer[MSG_SIZE];
//                 snprintf(orderByCustomer, MSG_SIZE, "%d", input);
//                 write(pipePtrs[1], orderByCustomer,
//                       MSG_SIZE);  // Write order to pipe
//             }
//             close(pipePtrs[1]);  // Close the write end of the pipe in the
//             child
//                                  // process
//             exit(EXIT_SUCCESS);
//         } else {
//             // parent execution
//             close(pipePtrs[1]);  // Close the write end of the pipe in the
//                                  // parent process

//             char orderByCustomer[MSG_SIZE];
//             while (read(pipePtrs[0], orderByCustomer, MSG_SIZE) > 0) {
//                 // printf("rec: %s\n", orderByCustomer);

//                 // head = addFront(head, atoi(&orderByCustomer[0]));
//                 addToFront(&head, atoi(&orderByCustomer[0]));

//                 // printf("new head val: %d\n", head->data);
//                 struct Node *v = head;
//                 while (v != NULL) {
//                     printf("%d, ", v->data);
//                     v = v->next;
//                 }
//                 printf("\n");

//                 // handle errors, if any
//                 // if (atoi(&orderByCustomer[0]) > itemsInMenu &&
//                 //     atoi(&orderByCustomer[0]) < 1) {
//                 //     // invalid item ID
//                 // } else {
//                 //     // add to itemsOrdered and update total cost
//                 //     order[atoi(&orderByCustomer[0]) - 1]++;
//                 // }
//             }

//             (a + 2)->data = 1;  // order for kth customer written
//             printf("a + 2 set\n");

//             close(pipePtrs[0]);  // Close the read end of the pipe in the
//                                  // parent process

//             while ((a + 5)->data != 0) {
//             }

//             if ((a + 5)->data == -1) {
//                 // incorrect order
//                 k = 0;
//             }

//             wait(NULL);  // Wait for child process to finish
//         }
//     }

//     // int k = 0;
//     // for (; k < itemsInMenu; k++) {
//     //     *(a + k) = order[k];
//     // }
//     // *(a + k) = 1;

//     // while (*(a + itemsInMenu - 1 + 3) != 1) {
//     // }

//     while ((a + 4)->data != 1) {
//     }

//     printf("The total bill amount is %d INR.\n", (a + 3)->data);

//     shmctl(shmid, IPC_RMID, NULL);

//     shmdt((void *)a);

//     return EXIT_SUCCESS;
// }
