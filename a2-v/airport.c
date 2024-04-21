#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct {
    long msg_type;
    int plane_id;
    int plane_weight;
    int is_departure;
} Message;

typedef struct {
    int runway_capacity;
    pthread_mutex_t mutex;
} Runway;

Runway runways[11];  // 10 runways + 1 backup runway

void* handle_plane(void* arg) {
    Message* message = (Message*) arg;

    // Find the best-fit runway
    int best_fit_index = -1;
    for (int i = 1; i <= 10; i++) {
        if (runways[i].runway_capacity >= message->plane_weight &&
            (best_fit_index == -1 || runways[i].runway_capacity < runways[best_fit_index].runway_capacity)) {
            best_fit_index = i;
        }
    }

    // If no runway is found, use the backup runway
    if (best_fit_index == -1) {
        best_fit_index = 0;
    }

    // Lock the runway
    pthread_mutex_lock(&runways[best_fit_index].mutex);

    // Handle the plane
    if (message->is_departure) {
        sleep(3);  // Simulate boarding/loading
        printf("Plane %d has completed boarding/loading and taken off from Runway No. %d of Airport No. %ld.\n",
               message->plane_id, best_fit_index, message->msg_type);
    } else {
        sleep(2);  // Simulate landing
        sleep(3);  // Simulate deboarding/unloading
        printf("Plane %d has landed on Runway No. %d of Airport No. %ld and has completed deboarding/unloading.\n",
               message->plane_id, best_fit_index, message->msg_type);
    }

    // Unlock the runway
    pthread_mutex_unlock(&runways[best_fit_index].mutex);

    free(arg);
    return NULL;
}

int main() {
    int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    // Initialize the runways
    for (int i = 0; i <= 10; i++) {
        pthread_mutex_init(&runways[i].mutex, NULL);
        if (i == 0) {
            runways[i].runway_capacity = 15000;  // Backup runway
        } else {
            printf("Enter loadCapacity of Runway No. %d: ", i);
            scanf("%d", &runways[i].runway_capacity);
        }
    }

    while (1) {
        Message* message = malloc(sizeof(Message));
        msgrcv(msgid, message, sizeof(Message), 0, 0);

        if (message->msg_type == 2) {  // Termination request
            free(message);
            break;
        }

        pthread_t thread;
        pthread_create(&thread, NULL, handle_plane, message);
        pthread_detach(thread);
    }

    return 0;
}