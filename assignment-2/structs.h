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
    int runways[11];
    int n;
    int msgid;  // msg queue identifier
    struct Plane plane;
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