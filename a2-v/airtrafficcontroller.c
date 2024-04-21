#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_AIRPORTS 10

typedef struct {
    long msg_type;
    int plane_id;
    int plane_type;
    int num_passengers;
    int total_weight;
    int departure_airport;
    int arrival_airport;
} PlaneMessage;

typedef struct {
    long msg_type;
    PlaneMessage plane;
} Message;

int main() {
    int num_airports;
    printf("Enter the number of airports to be handled/managed: ");
    scanf("%d", &num_airports);

    int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    FILE *file = fopen("AirTrafficController.txt", "a");
    if (file == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    while (1) {
        Message message;
        msgrcv(msgid, &message, sizeof(message), 1, 0);

        if (message.msg_type == 1) {  // Plane details
            PlaneMessage plane = message.plane;
            fprintf(file, "Plane %d has departed from Airport %d and will land at Airport %d.\n", plane.plane_id, plane.departure_airport, plane.arrival_airport);
            fflush(file);

            // Send message to departure airport
            message.msg_type = plane.departure_airport;
            msgsnd(msgid, &message, sizeof(message), 0);

            // Wait for confirmation from departure airport
            msgrcv(msgid, &message, sizeof(message), plane.departure_airport, 0);

            // Send message to arrival airport
            message.msg_type = plane.arrival_airport;
            msgsnd(msgid, &message, sizeof(message), 0);

            // Wait for confirmation from arrival airport
            msgrcv(msgid, &message, sizeof(message), plane.arrival_airport, 0);

            // Send confirmation to plane
            message.msg_type = 1;
            msgsnd(msgid, &message, sizeof(message), 0);
        } else if (message.msg_type == 2) {  // Termination request
            // Send termination messages to all airports
            for (int i = 1; i <= num_airports; i++) {
                message.msg_type = i;
                msgsnd(msgid, &message, sizeof(message), 0);
            }

            // Wait for confirmation messages from all airports
            for (int i = 1; i <= num_airports; i++) {
                msgrcv(msgid, &message, sizeof(message), i, 0);
            }

            break;
        }
    }

    fclose(file);
    msgctl(msgid, IPC_RMID, NULL);
    // int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    PlaneMessage message;
    int termination_request_received = 0;  // Add this line

    // Wait for a message from a plane
    msgrcv(msgid, &message, sizeof(PlaneMessage), 1, 0);

    // Check if termination request was received
    if (message.msg_type == 2) {  // Check if the message type is 2 (termination request)
        termination_request_received = 1;  // Set termination_request_received to 1
    }

    if (termination_request_received) {
        message.msg_type = 2;  // Termination request
        msgsnd(msgid, &message, sizeof(PlaneMessage), 0);
        return 0;
    }

    // ... (inform departure and arrival airports)

    // Wait for a message from the departure airport
    msgrcv(msgid, &message, sizeof(PlaneMessage), 1, 0);

    // ... (append entry to .txt file)

    // Wait for a message from the arrival airport
    msgrcv(msgid, &message, sizeof(PlaneMessage), 1, 0);

    // Inform the plane that the flight was successful
    message.msg_type = 1;
    msgsnd(msgid, &message, sizeof(PlaneMessage), 0);

    return 0;
}