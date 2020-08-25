#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "error.h"
#include "utils.h"
#include "alarm_thread.h"

#define USER_INPUT_MAX_LEN 128 

int main(int argc, char *argv[])
{
  int status;
  char line[USER_INPUT_MAX_LEN];
  alarm_t *alarm; 
  pthread_t thread;

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
      status = pthread_create(&thread, NULL, 
          &alarm_thread_detach, alarm);
      if(status != 0)
      {
        err_abort(status, "Create alarm thread");
      }
    }

  }
  // exit() will terminate the main thread and the spawned threads.
  // use pthread_exit() to terminate only the main thread.
  exit(0); 
}
