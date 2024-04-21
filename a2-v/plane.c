#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define MAX_PASSENGERS 10
#define MAX_CARGO_ITEMS 100
#define MAX_WEIGHT 100
#define MAX_LUGGAGE_WEIGHT 25
#define MAX_AIRPORTS 10
#define CREW_WEIGHT 75
#define PASSENGER_CREW 7
#define CARGO_CREW 2
#define SLEEP_TIME 3
#define JOURNEY_TIME 30

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
    int luggage_weight;
    int body_weight;
} Passenger;


typedef struct {
    int num_items;
    int avg_weight;
} Cargo;

typedef struct {
    long msg_type;
    PlaneMessage plane;
} Message;

int main() {
    PlaneMessage plane;
    // Create a plane process
    pid_t pid = fork();
    if (pid == 0) {
        // This is the plane process
        PlaneMessage plane;
        printf("Enter Plane ID: ");
        scanf("%d", &plane.plane_id);
        printf("Enter Type of Plane: ");
        scanf("%d", &plane.plane_type);
        if (plane.plane_type == 1) {
            // This is a passenger plane
            printf("Enter Number of Occupied Seats: ");
            scanf("%d", &plane.num_passengers);
            int total_luggage_weight = 0;
            int total_body_weight = 0;
            for (int i = 0; i < plane.num_passengers; i++) {
                Passenger passenger;
                printf("Enter Weight of Your Luggage: ");
                scanf("%d", &passenger.luggage_weight);
                printf("Enter Your Body Weight: ");
                scanf("%d", &passenger.body_weight);
                total_luggage_weight += passenger.luggage_weight;
                total_body_weight += passenger.body_weight;
            }
            plane.total_weight = total_luggage_weight + total_body_weight + PASSENGER_CREW * CREW_WEIGHT;
        } else {
            // This is a cargo plane
            Cargo cargo;
            printf("Enter Number of Cargo Items: ");
            scanf("%d", &cargo.num_items);
            printf("Enter Average Weight of Cargo Items: ");
            scanf("%d", &cargo.avg_weight);
            plane.total_weight = cargo.num_items * cargo.avg_weight + CARGO_CREW * CREW_WEIGHT;
        }
        printf("Enter Airport Number for Departure: ");
        scanf("%d", &plane.departure_airport);
        printf("Enter Airport Number for Arrival: ");
        scanf("%d", &plane.arrival_airport);
        // Send a message to the air traffic controller with these details
        Message message;
        message.msg_type = 1;
        message.plane = plane;
      

        int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
        msgsnd(msgid, &message, sizeof(message), 0);
        // Wait for the completion message from the air traffic controller
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        printf("Plane %d has successfully traveled from Airport %d to Airport %d!\n", plane.plane_id, plane.departure_airport, plane.arrival_airport);
    } else {
        // This is the parent process
        wait(NULL);
    }
int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    PlaneMessage message;
    message.msg_type = 1;  // Message type 1 for plane messages
    message.plane_id = plane.plane_id;
    message.plane_type = plane.plane_type;
    message.num_passengers = plane.num_passengers;
    message.total_weight = plane.total_weight;
    message.departure_airport = plane.departure_airport;
    message.arrival_airport = plane.arrival_airport;

    msgsnd(msgid, &message, sizeof(PlaneMessage), 0);

    // Wait for a response from the air traffic controller
    msgrcv(msgid, &message, sizeof(PlaneMessage), 1, 0);

    if (message.msg_type == 2) {  // Termination request
        printf("No further departures will happen. Plane %d terminates.\n", plane.plane_id);
        return 0;
    }

    printf("Flight for Plane %d was successful.\n", plane.plane_id);

    return 0;
}