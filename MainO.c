#include "pthread_sleep.c" 
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <time.h> 
#include <sys/time.h> 
#include <string.h> 
#include <queue>
using namespace std;

/* Plane struct is used throughout our implementation:
*	ID: unique identification number for a plane.
*	arrival_time: the time in our simulation when the plane thread is created.
*	runway_time: the time where the plane reaches at the end of the runway.
*	wait_at_front_time: the time where how much the plane waits when it is at the front of the queue (used for handling
*	starvation)
*	lock and cond: used for signaling and waiting of the plane threads.
*/
struct Plane { int ID;
time_t arrival_time; time_t wait_at_front_time; pthread_mutex_t lock; pthread_cond_t cond;
};

queue<Plane> landing_queue; queue<Plane> departing_queue; queue<Plane> emergency_queue;

queue<int> landing_queue_of_ID; queue<int> emergency_queue_of_ID; queue<int> departing_queue_of_ID;

char planes_log[350][57];

pthread_mutex_t atc_lock; pthread_cond_t atc_cond;

int unique_departing_plane_id = 1; int unique_landing_plane_id = 0;

FILE *fp = fopen("./planes.log", "w");

int simulation_duration; float p;
int n; int seed;


int total_planes_in_sim = 0; time_t current_time;
time_t start_time; int t = 1;


void *landing_func(void *emergency_arg) { bool emergency = (bool) emergency_arg;

struct Plane plane; pthread_mutex_init(&plane.lock, NULL); pthread_cond_init(&plane.cond, NULL); total_planes_in_sim++;
plane.ID = unique_landing_plane_id; char plane_id_string[3];
sprintf(plane_id_string, "%d", unique_landing_plane_id); if (unique_landing_plane_id >= 100) {
planes_log[unique_landing_plane_id][0] = plane_id_string[0]; planes_log[unique_landing_plane_id][1] = plane_id_string[1]; planes_log[unique_landing_plane_id][2] = plane_id_string[1];
} else if (unique_landing_plane_id >= 10) { planes_log[unique_landing_plane_id][0] = plane_id_string[0]; planes_log[unique_landing_plane_id][1] = plane_id_string[1];
} else {
planes_log[unique_landing_plane_id][0] = plane_id_string[0];
}

plane.arrival_time = current_time - start_time; char request_time_string[3];
sprintf(request_time_string, "%ld", plane.arrival_time); if (plane.arrival_time >= 100) {
planes_log[unique_landing_plane_id][20] = request_time_string[0]; planes_log[unique_landing_plane_id][21] = request_time_string[1]; planes_log[unique_landing_plane_id][22] = request_time_string[2];
} else if (plane.arrival_time >= 10) { planes_log[unique_landing_plane_id][20] = request_time_string[0]; planes_log[unique_landing_plane_id][21] = request_time_string[1];

} else {
planes_log[unique_landing_plane_id][20] = request_time_string[0];
}


if (emergency) { emergency_queue.push(plane); emergency_queue_of_ID.push(plane.ID); planes_log[unique_landing_plane_id][8] = '(';
planes_log[unique_landing_plane_id][9] = 'E'; planes_log[unique_landing_plane_id][10] = ')'; planes_log[unique_landing_plane_id][11] = 'L';
} else {
landing_queue.push(plane); planes_log[unique_landing_plane_id][9] = 'L'; landing_queue_of_ID.push(plane.ID);
}

if (unique_landing_plane_id == 0) { pthread_cond_signal(&atc_cond);
}
unique_landing_plane_id = unique_landing_plane_id + 2; pthread_cond_wait(&plane.cond, &plane.lock);
pthread_exit(NULL);
}


void *departing_func(void *param) {

struct Plane plane; plane.wait_at_front_time = 0; pthread_mutex_init(&plane.lock, NULL); pthread_cond_init(&plane.cond, NULL); total_planes_in_sim++;

plane.ID = unique_departing_plane_id; char plane_id_string[3];
sprintf(plane_id_string, "%d", unique_departing_plane_id); if (unique_departing_plane_id >= 100) {
planes_log[unique_departing_plane_id][0] = plane_id_string[0]; planes_log[unique_departing_plane_id][1] = plane_id_string[1];
 
 

planes_log[unique_departing_plane_id][2] = plane_id_string[1];
} else if (unique_departing_plane_id >= 10) { planes_log[unique_departing_plane_id][0] = plane_id_string[0]; planes_log[unique_departing_plane_id][1] = plane_id_string[1];
} else {
planes_log[unique_departing_plane_id][0] = plane_id_string[0];
}

plane.arrival_time = current_time - start_time; char request_time_string[3];
sprintf(request_time_string, "%ld", plane.arrival_time); if (plane.arrival_time >= 100) {
planes_log[unique_departing_plane_id][20] = request_time_string[0]; planes_log[unique_departing_plane_id][21] = request_time_string[1]; planes_log[unique_departing_plane_id][22] = request_time_string[2];
} else if (plane.arrival_time >= 10) { planes_log[unique_departing_plane_id][20] = request_time_string[0]; planes_log[unique_departing_plane_id][21] = request_time_string[1];
} else {
planes_log[unique_departing_plane_id][20] = request_time_string[0];
}

planes_log[unique_departing_plane_id][9] = 'D';


departing_queue_of_ID.push(plane.ID); departing_queue.push(plane);

if (unique_departing_plane_id == 1) { pthread_cond_signal(&atc_cond);
}


unique_departing_plane_id = unique_departing_plane_id + 2; pthread_cond_wait(&plane.cond, &plane.lock); pthread_exit(NULL);
}


void *air_traffic_control(void *param) { pthread_cond_wait(&atc_cond, &atc_lock);

current_time = time(NULL);
 
 

while (current_time < start_time + simulation_duration) { if (emergency_queue.size() > 0) {
pthread_sleep(2 * t); current_time = time(NULL);
pthread_cond_signal(&emergency_queue.front().cond); long int runway_time = current_time - start_time;

char runway_time_string[3]; sprintf(runway_time_string, "%ld", runway_time); if (runway_time >= 100) {
planes_log[emergency_queue.front().ID][33] = runway_time_string[0]; planes_log[emergency_queue.front().ID][34] = runway_time_string[1]; planes_log[emergency_queue.front().ID][35] = runway_time_string[2];
} else if (runway_time >= 10) { planes_log[emergency_queue.front().ID][33] = runway_time_string[0]; planes_log[emergency_queue.front().ID][34] = runway_time_string[1];
} else {
planes_log[emergency_queue.front().ID][33] = runway_time_string[0];
}
long int turnaround_time = runway_time - emergency_queue.front().arrival_time; char turnaround_time_string[3];
sprintf(turnaround_time_string, "%ld", turnaround_time); if (turnaround_time >= 100) {
planes_log[emergency_queue.front().ID][47] = turnaround_time_string[0]; planes_log[emergency_queue.front().ID][48] = turnaround_time_string[1]; planes_log[emergency_queue.front().ID][49] = turnaround_time_string[2];
} else if (turnaround_time >= 10) { planes_log[emergency_queue.front().ID][47] = turnaround_time_string[0]; planes_log[emergency_queue.front().ID][48] = turnaround_time_string[1];
} else {
planes_log[emergency_queue.front().ID][47] = turnaround_time_string[0];
}
emergency_queue.pop(); emergency_queue_of_ID.pop();

} else if (landing_queue.size() < 12 && departing_queue.size() > 0 && (departing_queue.front().wait_at_front_time > 2 * t || landing_queue.empty())
&&
(((current_time - start_time) % (40 * t)) != 0 || current_time - start_time == 0)) { pthread_sleep(2 * t);
current_time = time(NULL); pthread_cond_signal(&departing_queue.front().cond);

long int runway_time = current_time - start_time; char runway_time_string[3];

sprintf(runway_time_string, "%ld", runway_time); if (runway_time >= 100) {
planes_log[departing_queue.front().ID][33] = runway_time_string[0]; planes_log[departing_queue.front().ID][34] = runway_time_string[1]; planes_log[departing_queue.front().ID][35] = runway_time_string[2];
} else if (runway_time >= 10) { planes_log[departing_queue.front().ID][33] = runway_time_string[0]; planes_log[departing_queue.front().ID][34] = runway_time_string[1];
} else {
planes_log[departing_queue.front().ID][33] = runway_time_string[0];
}
long int turnaround_time = runway_time - departing_queue.front().arrival_time; char turnaround_time_string[3];
sprintf(turnaround_time_string, "%ld", turnaround_time);
if (turnaround_time >= 100) {
planes_log[departing_queue.front().ID][47] = turnaround_time_string[0]; planes_log[departing_queue.front().ID][48] = turnaround_time_string[1]; planes_log[departing_queue.front().ID][49] = turnaround_time_string[2];
} else if (turnaround_time >= 10) { planes_log[departing_queue.front().ID][47] = turnaround_time_string[0]; planes_log[departing_queue.front().ID][48] = turnaround_time_string[1];
} else {
planes_log[departing_queue.front().ID][47] = turnaround_time_string[0];
}

departing_queue.pop(); departing_queue_of_ID.pop();

} else if ((((current_time - start_time) % (40 * t)) != 0 || current_time - start_time == 0))
{
pthread_sleep(2 * t); current_time = time(NULL);
pthread_cond_signal(&landing_queue.front().cond); long int runway_time = current_time - start_time;
char runway_time_string[3]; sprintf(runway_time_string, "%ld", runway_time); if (runway_time >= 100) {
planes_log[landing_queue.front().ID][33] = runway_time_string[0]; planes_log[landing_queue.front().ID][34] = runway_time_string[1]; planes_log[landing_queue.front().ID][35] = runway_time_string[2];
} else if (runway_time >= 10) { planes_log[landing_queue.front().ID][33] = runway_time_string[0]; planes_log[landing_queue.front().ID][34] = runway_time_string[1];
} else {
planes_log[landing_queue.front().ID][33] = runway_time_string[0];
}

long int turnaround_time = runway_time - landing_queue.front().arrival_time; char turnaround_time_string[3];
sprintf(turnaround_time_string, "%ld", turnaround_time); if (turnaround_time >= 100) {
planes_log[landing_queue.front().ID][47] = turnaround_time_string[0]; planes_log[landing_queue.front().ID][48] = turnaround_time_string[1]; planes_log[landing_queue.front().ID][49] = turnaround_time_string[2];
} else if (turnaround_time >= 10) { planes_log[landing_queue.front().ID][47] = turnaround_time_string[0]; planes_log[landing_queue.front().ID][48] = turnaround_time_string[1];
} else {
planes_log[landing_queue.front().ID][47] = turnaround_time_string[0];
}
landing_queue.pop(); landing_queue_of_ID.pop();
}
departing_queue.front().wait_at_front_time = departing_queue.front().wait_at_front_time + 2 * t;
}
pthread_exit(NULL);
}
void print_debug() {
printf("At %ld sec ground: ", current_time - start_time); for (int i = 0; i < departing_queue_of_ID.size(); i++) {
int id = departing_queue_of_ID.front(); printf("%d ", id); departing_queue_of_ID.pop(); departing_queue_of_ID.push(id);
}

printf("\nAt %ld sec air: ", current_time - start_time); for (int i = 0; i < landing_queue_of_ID.size(); i++) {
int id = landing_queue_of_ID.front(); printf("%d ", id); landing_queue_of_ID.pop(); landing_queue_of_ID.push(id);
}
for (int i = 0; i < emergency_queue_of_ID.size(); i++) { int id = emergency_queue_of_ID.front();
printf("%d ", id); emergency_queue_of_ID.pop(); emergency_queue_of_ID.push(id);
}
printf("%s", "\n");
}
void log_initialization() {
for (int i = 0; i < 350; i++) { for (int j = 0; j < 56; j++) {
planes_log[i][j] = ' ';
planes_log[i][56] = '\0';
}
}
}

void print_log() {
fprintf(fp, "%s", "PlaneID Status Request Time Runway Time Turnaround Time\n"); fprintf(fp, "%s",
"		\n"); for (int i = 0; i < total_planes_in_sim; i++) {
fprintf(fp, "%s\n", planes_log[i]);
}
}
int main(int argc, char *argv[]) { for (int i = 1; i < argc; i++) {
if (strcmp(argv[i], "-s") == 0) { simulation_duration = atoi(argv[i + 1]);
} else if (strcmp(argv[i], "-p") == 0) { p = (double) atof(argv[i + 1]);
} else if (strcmp(argv[i], "-n") == 0) { n = atoi(argv[i + 1]);
} else if (strcmp(argv[i], "-seed") == 0) { seed = atoi(argv[i + 1]);
srand(seed);
}
}

log_initialization(); pthread_mutex_init(&atc_lock, NULL); pthread_cond_init(&atc_cond, NULL); pthread_t atc, departing_plane, landing_plane;
pthread_create(&atc, NULL, air_traffic_control, NULL); pthread_create(&landing_plane, NULL, landing_func, (void *) 0); pthread_create(&departing_plane, NULL, departing_func, NULL);

start_time = time(NULL); current_time = time(NULL); double random_number;
while (current_time < start_time + simulation_duration) {
 
 

random_number = (double) rand() / (RAND_MAX);
if ((((current_time - start_time) % (40 * t)) == 0) && current_time - start_time != 0) { pthread_t new_plane;
pthread_create(&new_plane, NULL, landing_func, (void *) 1);
} else if (random_number <= p) { pthread_t new_plane;
pthread_create(&new_plane, NULL, landing_func, (void *) 0);
}
if (random_number <= 1 - p) { pthread_t new_plane;
pthread_create(&new_plane, NULL, departing_func, NULL);
}
if (current_time - start_time >= n) { print_debug();
fflush(stdout);
}
pthread_sleep(t); current_time = time(NULL);
}
print_log(); fclose(fp); return 0;
}
