#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "structs.h"

int main() {
    while (1) {
        printf(
            "Do you want the Air Traffic Control System to terminate? Y for "
            "yes and N for no: ");
        char c;
        scanf("%c", &c);
        if (c == 'Y' || c == 'y') {
            int key = ftok("airtrafficcontroller.c", 'A');
            int msgid = msgget(key, 0666 | IPC_CREAT);

            struct PlaneMessage msg;
            struct Plane plane;
            msg.mtype = CLEANUP_EXIT_ATC;
            msg.plane = plane;

            int resp = msgsnd(msgid, &msg, sizeof(msg), IPC_NOWAIT);

            if (resp < 0) {
                printf("Some error sending message to cleanup");
            } else {
                break;
            }
        }
    }
}