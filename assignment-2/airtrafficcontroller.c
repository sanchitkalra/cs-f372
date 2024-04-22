#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "structs.h"

int main() {
    int n_airports;
    printf(
        "Enter the number of airports to be handled/managed (2-10 "
        "inclusive): ");
    scanf("%d", &n_airports);

    int key = ftok("airtrafficcontroller.c", "A");
    int msgid = msgget(key, 0666 | IPC_CREAT);

    int termination_req_rev = 0;  // 1 when received
    int numOfPlanes = 0;

    while (1) {
        struct PlaneMessage recMsg;
        int recCode =
            msgrcv(msgid, &recMsg, sizeof(struct PlaneMessage), 0, IPC_NOWAIT);

        // break out of this infinite loop, and end prog execution
        if (numOfPlanes == 0 && termination_req_rev == 1) break;

        if (recCode < 0) {
            // msg rcv error
            printf("Some error fetching message");
        } else {
            switch (recMsg.mtype) {
                case PLANE_TAKEOFF:
                    // new plane is ready for takeoff
                    if (termination_req_rev == 0) {
                        // only accept new plane requests if termination req has
                        // not been received
                        numOfPlanes++;

                        // notify dept airport to begin boarding and takeoff
                        struct PlaneMessage msgDeptTakeoff;
                        msgDeptTakeoff.mtype =
                            ATC_INFORM_DEPT * 100 + recMsg.plane.dept;
                        msgDeptTakeoff.plane = recMsg.plane;
                        int resp = msgsnd(msgid, &msgDeptTakeoff,
                                          sizeof(msgDeptTakeoff), IPC_NOWAIT);
                        if (resp < 0) {
                            // msg to dept airport err
                            printf("Some error sending plane to dept airport");
                        }
                    }
                    break;
                case DEPT_INFORM_ATC:
                    // dept has completed loading/boarding and takeoff

                    // notify arrival airport to begin landing & deboarding
                    struct PlaneMessage msgArrivLanding;
                    msgArrivLanding.mtype =
                        ATC_INFORM_ARRIV * 100 + recMsg.plane.dept;
                    msgArrivLanding.plane = recMsg.plane;
                    int resp = msgsnd(msgid, &msgArrivLanding,
                                      sizeof(msgArrivLanding), IPC_NOWAIT);
                    if (resp < 0) {
                        // msg to dept airport err
                        printf("Some error sending plane to arriv airport");
                    }
                    break;
                case ARRIV_INFORM_ATC:
                    // plane has landed at arriv airport, plane process now
                    // needs to exit
                    struct PlaneMessage msgPlaneExit;
                    msgPlaneExit.mtype =
                        PLANE_EXIT_CLEANUP * 100 + recMsg.plane.id;
                    msgPlaneExit.plane = recMsg.plane;
                    int resp = msgsnd(msgid, &msgPlaneExit,
                                      sizeof(msgPlaneExit), IPC_NOWAIT);
                    if (resp < 0) {
                        // msg to dept airport err
                        printf("Some error sending plane kill request");
                    }
                    numOfPlanes--;
                    break;
                case CLEANUP_EXIT_ATC:
                    termination_req_rev = 1;
                    break;
                default:
                    // NOTA
                    // Insert message back into the queue
                    int resp =
                        msgsnd(msgid, &recMsg, sizeof(recMsg), IPC_NOWAIT);
                    if (resp < 0) {
                        printf("Some error sending message back into queue");
                    }
                    break;
            }
        }
    }

    return 0;
}