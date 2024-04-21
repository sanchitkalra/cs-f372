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
    printf("Enter the number of airports to be handled/managed: ");
    scanf("%d", &n_airports);

    int key = ftok("airtrafficcontroller.c", "A");
    int msgid = msgget(key, 0666 | IPC_CREAT);

    int termination_req_rev = 0;  // 1 when received
    int numOfPlanes = 0;

    while (1) {
        struct PlaneMessage recMsg;
        int recCode =
            msgrcv(msgid, &recMsg, sizeof(struct PlaneMessage), 0, IPC_NOWAIT);

        if (recCode < 0) {
            // msg rcv error
            printf("Some error fetching message");
        } else {
            switch (recMsg.mtype) {
                case PLANE_TAKEOFF:
                    if (termination_req_rev == 0) {
                        numOfPlanes++;

                        // notify dept airport to begin boarding and takeoff
                        struct PlaneMessage msgDeptTakeoff;
                        msgDeptTakeoff.mtype = ATC_INFORM_DEPT;
                        msgDeptTakeoff.plane = recMsg.plane;
                        int resp = msgsnd(msgid, &msgDeptTakeoff,
                                          sizeof(msgDeptTakeoff), IPC_NOWAIT);
                        if (resp < 0) {
                            // msg to dept airport err
                            printf("Some error sending plane to dept airport");
                        }
                    }
                    break;
                case ATC_INFORM_DEPT:
                    // only from ATC -> DEPT
                    // not handled here
                    break;
                case DEPT_INFORM_ATC:
                    // notify arrival airport to begin landing & deboarding
                    struct PlaneMessage msgArrivLanding;
                    msgArrivLanding.mtype = ATC_INFORM_ARRIV;
                    msgArrivLanding.plane = recMsg.plane;
                    int resp = msgsnd(msgid, &msgArrivLanding,
                                      sizeof(msgArrivLanding), IPC_NOWAIT);
                    if (resp < 0) {
                        // msg to dept airport err
                        printf("Some error sending plane to arriv airport");
                    }
                    break;
                case ATC_INFORM_ARRIV:
                    // only from ATC -> ARRIV
                    // not handled here
                    break;
                case ARRIV_INFORM_ATC:
                    // plane has landed at arriv airport, plane process now
                    // needs to exit
                    struct PlaneMessage msgPlaneExit;
                    msgPlaneExit.mtype = PLANE_EXIT_CLEANUP;
                    msgPlaneExit.plane = recMsg.plane;
                    int resp = msgsnd(msgid, &msgPlaneExit,
                                      sizeof(msgPlaneExit), IPC_NOWAIT);
                    if (resp < 0) {
                        // msg to dept airport err
                        printf("Some error sending plane kill request");
                    }
                    numOfPlanes--;
                    break;
                case PLANE_EXIT_CLEANUP:
                    numOfPlanes--;
                    break;
                case CLEANUP_EXIT_ATC:
                    termination_req_rev = 1;
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}