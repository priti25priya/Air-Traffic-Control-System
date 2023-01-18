#include <stdio.h> 
#include <pthread.h> 
#include <sys/time.h>
int pthread_sleep (int seconds)
{
pthread_mutex_t mutex; pthread_cond_t conditionvar; struct timespec timetoexpire;
if(pthread_mutex_init(&mutex,NULL))
{
return -1;
}
if(pthread_cond_init(&conditionvar,NULL))
{
return -1;
}
struct timeval tp;
//We used this to get the current time which will be added along with the simulation time
gettimeofday(&tp, NULL);
timetoexpire.tv_sec = tp.tv_sec + seconds; timetoexpire.tv_nsec = tp.tv_usec
* 1000;

pthread_mutex_lock (&mutex);
int res = pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire); pthread_mutex_unlock (&mutex);
pthread_mutex_destroy(&mutex); pthread_cond_destroy(&conditionvar);
//Upon successful completion, a value of zero shall be returned return res;
 
 

}
