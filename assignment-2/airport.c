#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "structs.h"

int compareByCapacity(const void *a, const void *b) {
    return ((struct Runway *)a)->capacity - ((struct Runway *)b)->capacity;
}

void *thread_func_dept(void *arg) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)arg;

    // find optimal runway
    int index;  // index of the array element corrsp to the optimal runway
    for (int k = 0; k < threadArgs->n; k++) {
        if (threadArgs->runways[k].capacity >= threadArgs->plane.weight) {
            index = k;
            break;
        }
    }

    // Acquire mutex to use optimal runway
    pthread_mutex_lock(&(threadArgs->runways[index].mutex));

    // Runway is now available
    // Board/Load the plane

    sleep(3);  // boarding

    // Plane has been boarded/loaded

    // now simulate actual takeoff

    sleep(2);  // taking off

    // Release mutex lock
    pthread_mutex_unlock(&(threadArgs->runways[index].mutex));

    // now send message to ATC that plane has taken off

    struct PlaneMessage planeMsg;
    planeMsg.mtype = DEPT_INFORM_ATC;
    planeMsg.plane = threadArgs->plane;
    int resp =
        msgsnd(threadArgs->msgid, &planeMsg, sizeof(planeMsg), IPC_NOWAIT);
    if (resp < 0) {
        printf("Msg to ATC failed");
    }

    // Print message saying boarding has completed and plane taken off
    printf(
        "Plane %d has completed boarding/loading and taken off from Runway No. "
        "%d of Airport No. %d.\n",
        threadArgs->plane.id, threadArgs->runways[index].runwayID,
        threadArgs->plane.dept);
}

void *thread_func_arriv(void *arg) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)arg;

    // find optimal runway
    int index;  // index of the array element corrsp to the optimal runway
    for (int k = 0; k < threadArgs->n; k++) {
        if (threadArgs->runways[k].capacity >= threadArgs->plane.weight) {
            index = k;
            break;
        }
    }

    // Acquire mutex to use optimal runway
    pthread_mutex_lock(&(threadArgs->runways[index].mutex));

    // Runway is now available
    // Land the plane

    sleep(2);  // landing

    // Plane has landed

    // now simulate actual deboarding

    sleep(3);  // deboarding

    // Release mutex lock
    pthread_mutex_unlock(&(threadArgs->runways[index].mutex));

    // now send message to ATC that plane has landed & deboarded

    struct PlaneMessage planeMsg;
    planeMsg.mtype = ARRIV_INFORM_ATC;
    planeMsg.plane = threadArgs->plane;
    int resp =
        msgsnd(threadArgs->msgid, &planeMsg, sizeof(planeMsg), IPC_NOWAIT);
    if (resp < 0) {
        printf("Msg to ATC failed");
    }

    // Print message saying landing & deboarding has completed
    printf(
        "Plane %d has has landed on Runway No. "
        "%d of Airport No. %d.\n",
        threadArgs->plane.id, threadArgs->runways[index].runwayID,
        threadArgs->plane.arriv);
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

    struct Runway loadCap[n_runways +
                          1];  // n_runways  + 1 -> accomodate the backup runway

    for (int k = 0; k < n_runways; k++) {
        scanf("%d", loadCap[k].capacity);  // set capacity inside struct
        pthread_mutex_init(&loadCap[k].mutex,
                           NULL);     // init that runway's mutex lock
        loadCap[k].runwayID = k + 1;  // set runway ID
    }

    // Add backup runway
    loadCap[n_runways].capacity = 15000;
    pthread_mutex_init(&loadCap[n_runways].mutex, NULL);
    loadCap[n_runways].runwayID = n_runways + 1;

    // sort this array of structs now
    qsort(loadCap, n_runways + 1, sizeof(struct Runway), compareByCapacity);

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
            struct ThreadArgs threadArgs;
            threadArgs.msgid = msgid;
            threadArgs.runways = loadCap;
            threadArgs.n = n_runways + 1;
            threadArgs.plane = recMsg.plane;

            int rc = pthread_create(&tid, NULL, thread_func_dept, &threadArgs);
            // we will not wait for this thread so as to allow concurrent
            // execution
        } else if (mtype == ATC_INFORM_ARRIV * 100 + airport_num) {
            // message for arriv airport

            // put it in a thread
            pthread_t tid;
            struct ThreadArgs threadArgs;
            threadArgs.msgid = msgid;
            threadArgs.runways = loadCap;
            threadArgs.n = n_runways + 1;
            threadArgs.plane = recMsg.plane;

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