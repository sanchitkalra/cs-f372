#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "sorter.h"
#include "structs.h"

void *thread_func_dept(void *arg) {
    // TODO: Implement thread specific stuff

    struct ThreadArgs *threadArgs = (struct ThreadArgs *)arg;

    // TODO: find optimal runway
    // TODO: Acquire mutex to use optimal runway

    // Runway is now available
    // Board/Load the plane

    sleep(3);  // boarding

    // Plane has been boarded/loaded

    // now simulate actual takeoff

    sleep(2);  // taking off

    // TODO: Release mutex lock

    // now send message to ATC that plane has taken off

    struct PlaneMessage planeMsg;
    planeMsg.mtype = DEPT_INFORM_ATC;
    planeMsg.plane = threadArgs->plane;
    int resp =
        msgsnd(threadArgs->msgid, &planeMsg, sizeof(planeMsg), IPC_NOWAIT);
    if (resp < 0) {
        printf("Msg to ATC failed");
    }

    // TODO: Print message saying boarding has completed and plane taken off
}

void *thread_func_arriv(void *arg) {
    // TODO: Implement thread specific stuff

    struct ThreadArgs *threadArgs = (struct ThreadArgs *)arg;

    // TODO: find optimal runway
    // TODO: Acquire mutex to use optimal runway

    // Runway is now available
    // Land the plane

    sleep(2);  // landing

    // Plane has landed

    // now simulate actual deboarding

    sleep(3);  // deboarding

    // TODO: Release mutex lock

    // now send message to ATC that plane has landed & deboarded

    struct PlaneMessage planeMsg;
    planeMsg.mtype = ARRIV_INFORM_ATC;
    planeMsg.plane = threadArgs->plane;
    int resp =
        msgsnd(threadArgs->msgid, &planeMsg, sizeof(planeMsg), IPC_NOWAIT);
    if (resp < 0) {
        printf("Msg to ATC failed");
    }

    // TODO: Print message saying landing & deboarding has completed
}

int main() {
    // initialise airport

    printf("Enter airport number: ");
    int airport_num;
    scanf("%d", &airport_num);

    printf("Enter number of runways: ");
    int n_runways;
    scanf("%d", &n_runways);

    printf(
        "Enter  loadCapacity  of  Runways  (give  as  a  space  separated  "
        "list  in  a single line): ");

    int loadCap[11];
    for (int k = 0; k < n_runways; k++) {
        scanf("%d", loadCap[k]);
    }
    loadCap[n_runways] = 15000;

    // airport initialised
    // now listen for different dept & arrivals

    int key = ftok("airtrafficcontroller.c", "A");
    int msgid = msgget(key, 0666 | IPC_CREAT);

    while (1) {
        struct PlaneMessage recMsg;

        // we will listen for all kinds of message but care only about those we
        // are interested in, we'll discard the rest, and insert them back into
        // the message queue
        int recCode =
            msgrcv(msgid, &recMsg, sizeof(struct PlaneMessage), 0, IPC_NOWAIT);
        if (recCode < 0) {
            printf("Error reading message queue \n");
            continue;
        }

        long mtype = recMsg.mtype;

        if (mtype == ATC_INFORM_DEPT * 100 + airport_num) {
            // message for departure airport

            // put it in a thread
            pthread_t tid;
            struct ThreadArgs threadArgs;  // TODO: Initialise this struct
            int rc = pthread_create(&tid, NULL, thread_func_dept, &threadArgs);
            // we will not wait for this thread so as to allow concurrent
            // execution
        } else if (mtype == ATC_INFORM_ARRIV * 100 + airport_num) {
            // message for arriv airport

            // put it in a thread
            pthread_t tid;
            struct ThreadArgs threadArgs;  // TODO: Initialise this struct
            int rc = pthread_create(&tid, NULL, thread_func_arriv, &threadArgs);
            // we will not wait for this thread so as to allow concurrent
            // execution
        } else {
            // default case
            // put message back into queue

            msgsnd(msgid, &recMsg, sizeof(recMsg), IPC_NOWAIT);
        }
    }

    return 0;
}