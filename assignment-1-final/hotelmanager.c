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

#define HOTEL_MANAGER_SHM_NAME "hotel_manager_memory"

typedef struct {
    int table_number;
    int total_bill;
} WaiterData;

void write_earnings(int table_number, int total_bill) {
    FILE *fp = fopen("earnings.txt", "a");
    if (fp == NULL) {
        perror("Error opening earnings file");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "Earning from Table %d: %d INR\n", table_number, total_bill);
    fclose(fp);
}

void write_message(const char *message) {
    FILE *fp = fopen("earnings.txt", "a");
    if (fp == NULL) {
        perror("Error opening earnings file");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%s\n", message);
    fclose(fp);
}

int main() {
    int total_tables;
    int totalAmt = 0;
    WaiterData *ptr2;

    // key_t admin_id = 'A';             //Connect To Admin
    int key2 = ftok("admin", 'A');
    int shmid3 = shmget(key2, sizeof(int) * 1024, 0666 | IPC_CREAT);

    int *flag = (int *)shmat(shmid3, NULL, 0);

    printf("Enter the Total Number of Tables at the Hotel: ");
    scanf("%d", &total_tables);

    while (flag[0] == 0) {
    }

    if (flag[0] == 1) {
        // Read totalAmt from shared memory

        for (int i = 0; i < total_tables; i++) {
            char *key_seed = (char *)malloc(4 * sizeof(char));
            sprintf(key_seed, "manager_%d", i + 1);

            int key3 = ftok(key_seed, 'B');
            // Connect to WwaiterIDaiter
            int shmid4 = shmget(key3, 1024, 0666 | IPC_CREAT);
            WaiterData *ptr2 = (WaiterData *)shmat(shmid4, NULL, 0);

            totalAmt += ptr2[i].table_number;
            ptr2[i].total_bill = i + 1;
            write_earnings(ptr2[i].total_bill, ptr2[i].table_number);
        }

        char earnings_message[100];
        sprintf(earnings_message, "Total Earnings of Hotel: %d INR", totalAmt);
        write_message(earnings_message);

        int wages = totalAmt * 0.4;
        int profit = totalAmt - wages;
        char wages_message[100];
        sprintf(wages_message, "Total Wages of Waiters: %d INR", wages);
        write_message(wages_message);

        char profit_message[100];
        sprintf(profit_message, "Total Profit: %d INR", profit);
        write_message(profit_message);

        flag[0] = 0;
    }

    printf("Thank you for visiting the Hotel!\n");

    if (shmdt(flag) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
