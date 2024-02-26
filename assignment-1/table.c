#include <Kernel/string.h>
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

    int order[itemsInMenu];
    memset(order, 0, itemsInMenu * sizeof(int));

    int cost = 0;

    for (int k = 0; k < customers; k++) {
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

            char orderByCustomer[MSG_SIZE];
            while (read(pipePtrs[0], orderByCustomer, MSG_SIZE) > 0) {
                // printf("rec: %s\n", orderByCustomer);

                // handle errors, if any
                if (atoi(&orderByCustomer[0]) > itemsInMenu &&
                    atoi(&orderByCustomer[0]) < 1) {
                    // invalid item ID
                } else {
                    // add to itemsOrdered and update total cost
                    order[atoi(&orderByCustomer[0]) - 1]++;
                }
            }
            close(pipePtrs[0]);  // Close the read end of the pipe in the parent
                                 // process
            wait(NULL);          // Wait for child process to finish
        }
    }

    char *key_seed = (char *)malloc(4 * sizeof(char));
    sprintf(key_seed, "WT_%d", tableNumber);

    int key = ftok(key_seed, 'B');

    int shmid = shmget(key, sizeof(int) * (itemsInMenu + 3), 0666 | IPC_CREAT);
    int *a;
    a = (int *)shmat(shmid, NULL, 0);
    int k = 0;
    for (; k < itemsInMenu; k++) {
        *(a + k) = order[k];
    }
    *(a + k) = 1;

    while (*(a + itemsInMenu - 1 + 3) != 1) {
    }

    printf("The total bill amount is %d INR.\n", *(a + itemsInMenu - 1 + 2));

    shmctl(shmid, IPC_RMID, NULL);

    shmdt((void *)a);

    return EXIT_SUCCESS;
}
