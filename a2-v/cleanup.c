#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct {
    long msg_type;
} Message;

int main() {
    int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    while (1) {
        printf("Do you want the Air Traffic Control System to terminate?(Y for Yes and N for No)\n");
        char input;
        scanf(" %c", &input);

        if (input == 'Y' || input == 'y') {
            Message message;
            message.msg_type = 2;  // Termination request
            msgsnd(msgid, &message, sizeof(message), 0);
            break;
        }
    }

    return 0;
}