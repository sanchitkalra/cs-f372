#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    char choice;
    // key_t admin_id='A';
    key_t key = ftok("admin", 'A');
    int shmid = shmget(key, sizeof(int) * 1024, 0666 | IPC_CREAT);

    int *flag = (int *)shmat(shmid, NULL, 0);

    while (1) {
        printf("Do you want to close the hotel? Enter Y for Yes and N for No.");
        scanf(" %c", &choice);

        if (choice == 'Y' || choice == 'y') {
            flag[0] = 1;
            printf("Informing hotel manager to close the hotel...\n");
            while (flag[0] != 0) {
                // Wait for acknowledgment
            }
            break;
        } else if (choice == 'N' || choice == 'n') {
            printf("Continuing operation...\n");

        } else {
            printf("Invalid input. Please enter Y or N.\n");
            continue;
        }
    }
    shmctl(shmid, IPC_RMID, NULL);
    shmdt(flag);

    return 0;
}
