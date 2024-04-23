struct Passenger {
    int luggage_wt;
    int body_wt;
};

struct Plane {
    int id;
    int type;
    int num_passengers;
    int weight;
    int dept;
    int arriv;
};

struct Cargo {
    int n_items;
    int avg_wt;
};

struct PlaneMessage {
    long mtype;
    struct Plane plane;
};

struct ThreadArgs {
    struct Runway runways[11];
    int n;               // actual number of runways including the backup runway
    int msgid;           // msg queue identifier
    struct Plane plane;  // reference to the actual plane landing/taking off
};

struct Runway {
    int runwayID;
    int capacity;
    pthread_mutex_t mutex;
};

enum MSG {
    PLANE_TAKEOFF = 1,
    ATC_INFORM_DEPT = 2,  // use as 2*100 + airport_id
    DEPT_INFORM_ATC = 3,
    ATC_INFORM_ARRIV = 4,  // use as 2*100 + airport_id
    ARRIV_INFORM_ATC = 5,
    PLANE_EXIT_CLEANUP = 6,  // use as 6*100 + plane_id
    CLEANUP_EXIT_ATC = 7,
};