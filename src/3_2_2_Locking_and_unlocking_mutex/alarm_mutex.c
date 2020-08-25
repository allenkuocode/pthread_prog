#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "error.h"
#include "utils.h"

#define ALARM_MSG_MAX_LEN 64
#define USER_INPUT_MAX_LEN 128 

typedef struct alarm_tag
{
  struct alarm_tag *link;
  int seconds;
  time_t time; // Unix Epoch timestamp
  char message[ALARM_MSG_MAX_LEN];
} alarm_t;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
alarm_t * alarm_list = NULL;

static void print_alarm_expired_msg( alarm_t * alarm)
{
  printf("                                                       \
      Alarm expired, MSG:%s, Scheduled %ld, Now:%ld (unix epoc)\n",
      alarm->message,
      alarm->time, time(NULL));
}

//Pthread mutex lock with err checking 
static int Pthread_mutex_lock(pthread_mutex_t* mutex, char * msg)
{
  int status = pthread_mutex_lock(mutex);
  if(status != 0 ){err_abort(status, msg);}
  return status;
}

//Pthread mutex lock with err checking
static int Pthread_mutex_unlock(pthread_mutex_t* mutex, char * msg)
{
  int status = pthread_mutex_unlock(mutex);
  if(status != 0 ){err_abort(status, msg);}
  return status;
}

#define DEBUG
// A server thread to sequentially service the alarm request 
//
void * alarm_thread()
{

  alarm_t * alarm; 
  int sleep_time;
  time_t now;
  int status;

  while(1)
  {
    // Reading alarm list, need to lock alarm_list to prevent race
    // with the main thread
    status = Pthread_mutex_lock(&alarm_mutex, "Lock Mutex");
#ifdef DEBUG
    printf("                                     alarm thread locked mutex\n");
#endif 

    alarm = alarm_list;
    if(alarm == NULL)
    {
      // If no alarm is left, unlock the mutex and wait for one second
      // so that alarm_list can be updated from main thread;
      sleep_time  = 1;
    }
    else
    {
      alarm_list = alarm_list->link;
      now = time(NULL);
      if(alarm->time <= now)
      {
        sleep_time = 0;
      }
      else
      {
        sleep_time = alarm->time - now;
      }
    }
    // Unlock the mutex before sleeping so that the new request can be pushed
    // into queue from the main thread, if sleep time is 0, then call
    // sched_yield so that main thread can proceed to update queue 
    // in case there is any outstanding command to be inserted.
#ifdef DEBUG
    printf("                                     alarm thread unlocked mutex\n");
#endif
    status = Pthread_mutex_unlock(&alarm_mutex,"Unlock Mutex");

    if(sleep_time > 0)
    { sleep(sleep_time); }
    else
    {
      sched_yield(); // if no pending command to be inserted, sched_yield
                     // returns immediately
    }
      
    // Slept N second as requested, print the expired alarm and free it
    if( alarm != NULL )
    {
      print_alarm_expired_msg(alarm);
      free(alarm);
    }
 }

}

// main thread, to sort the request and insert into the alarm request list 
// 
int main(int argc, char* argv[])
{
  int status;
  char line[USER_INPUT_MAX_LEN];
  alarm_t *alarm, **last, *next; 
  pthread_t thread;

  // Create the alarm server thread
  status = pthread_create(&thread, NULL, alarm_thread, NULL);
  if(status != 0 ){err_abort(status, "Create alarm thread");}

  // parsing user input
  while(1)
  {
    printf ("Alarm > ");
    if ( fgets(line, sizeof(line), stdin) == NULL ){ exit(0);}
    if ( strlen(line) <=1){ continue; }

    alarm = (alarm_t*) Malloc(sizeof(alarm_t)); 

    // parsing user input
    if (sscanf(line, "%d %64[^\n]", &alarm->seconds, alarm->message) < 2)
    {
      fprintf( stderr, "Bad command\n");
      free(alarm);
    }
    else
    {
      alarm -> time = time(NULL) + alarm->seconds;
      status = Pthread_mutex_lock(&alarm_mutex, "Lock mutex");
      // inserting the alarm request into the sorted request list;
      // this will discard all the alarm that expires sooner than the newly
      // request one
      last = &alarm_list;
      next = *last;
      while(next != NULL)
      {
        if(next->time >= alarm->time)
        {
          alarm->link = next;
          *last = alarm;
          break;
        }
        last = &next->link;
        next = next->link;
      }
      
      if(next == NULL)
      { 
        *last = alarm; 
        alarm->link = NULL;
      }
#ifdef DEBUG
      printf ("[list: ");
      for (next = alarm_list; next != NULL; next = next-> link)
      {
        printf("%ld(%ld)[\"%s\"]", next->time, next->time-time(NULL), 
            next->message);
      }
      printf("]\n");
#endif
      status = Pthread_mutex_unlock(&alarm_mutex, "Unlock mutex");
    }
  }
}
