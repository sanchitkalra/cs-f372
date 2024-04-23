#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "structs.h"

#define MSG_SIZE_PIPE 48

int main() {
    struct Plane plane;
    printf("Enter plane ID (inclusive 1-10): ");
    scanf("%d", &plane.id);
    printf("Enter type of plane (0 for cargo and 1 for passenger): ");
    scanf("%d", &plane.type);

    if (plane.type == 0) {
        // cargo
        int n_cargo, av_wt;

        printf("Enter number of cargo items: ");
        scanf("%d", &n_cargo);

        printf("Enter average weight of cargo items: ");
        scanf("%d", &av_wt);

        int wt = n_cargo * av_wt;
        wt += 2 * 75;

        plane.weight = wt;

    } else {
        // passenger
        printf("Enter number of occupied seats: ");
        scanf("%d", &plane.num_passengers);

        int key = ftok("plane.c", 'A');
        int pipePtrs[2];

        int wt = 0;

        for (int k = 0; k < plane.num_passengers; k++) {
            if (pipe(pipePtrs) < 0) {
                exit(EXIT_FAILURE);
            }
            int pid = fork();

            if (pid == 0) {
                // child
                close(pipePtrs[0]);  // close read end of pipe

                int wt, luggage_wt;
                char msg[MSG_SIZE_PIPE];

                printf("Enter weight of luggage: ");
                scanf("%d", &luggage_wt);

                sprintf(msg, "%d", luggage_wt);
                write(pipePtrs[1], msg, MSG_SIZE_PIPE);

                printf("Enter your body weight: ");
                scanf("%d", &wt);

                sprintf(msg, "%d", wt);
                write(pipePtrs[1], msg, MSG_SIZE_PIPE);

                close(pipePtrs[1]);
            } else if (pid > 0) {
                // parent

                close(pipePtrs[1]);
                char msg[MSG_SIZE_PIPE];

                while (read(pipePtrs[0], msg, MSG_SIZE_PIPE) > 0) {
                    wt += atoi(msg);
                }

                close(pipePtrs[0]);
                wait(NULL);
            } else {
                // err
                exit(EXIT_FAILURE);
            }
        }

        wt += 75 * 7;

        plane.weight = wt;
    }

    printf("Enter Airport Number for Departure (1-10 inclusive): ");
    scanf("%d", &plane.dept);

    printf("Enter Airport Number for Arrival (1-10 inclusive): ");
    scanf("%d", &plane.arriv);

    int key = ftok("airtrafficcontroller.c", 'A');
    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct PlaneMessage planeMsg;
    planeMsg.mtype = PLANE_TAKEOFF;
    planeMsg.plane = plane;

    msgsnd(msgid, &planeMsg, sizeof(planeMsg), 0);

    // wait for takeoff
    // once takeoff, sleep for 30 sec to simulate flight time

    // printf("Plane waiting for sleep sign \n");

    struct PlaneMessage sleepMsg;
    msgrcv(msgid, &sleepMsg, sizeof(struct PlaneMessage),
           ATC_PLANE_SLEEP * 100 + plane.id, 0);

    // printf("Plane got sleep sign \n");

    sleep(30);  // sim flight time

    // printf("Plane sleep over \n");

    // flight now over
    // send signal to ATC

    struct PlaneMessage informATCSleepOver;
    informATCSleepOver.mtype = PLANE_INFROM_ATC_SLEEP_OVER;
    informATCSleepOver.plane = plane;

    msgsnd(msgid, &informATCSleepOver, sizeof(informATCSleepOver), 0);

    // printf("sleep done informed \n");

    // wait exit signal
    // mtype to listen for is PLANE_EXIT_CLEANUP * 100 + plane.id, for this will
    // be the type for message to this particular plane

    struct PlaneMessage killSignal;
    msgrcv(msgid, &killSignal, sizeof(struct PlaneMessage),
           PLANE_EXIT_CLEANUP * 100 + plane.id, 0);

    char op[1000];
    sprintf(
        op,
        "Plane %d has successfully travelled from Airport %d to Airport %d.",
        plane.id, plane.dept, plane.arriv);

    return 0;
}
