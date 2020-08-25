#include"pthread.h"
#include"utils.h"
#include"error.h"
#include"alarm_thread.h"

// Alarm thread, main thread spawns new threads for each alarm request
void* alarm_thread_detach(void * arg)
{
  alarm_t * alarm = (alarm_t*)arg;
  int status;
  status = pthread_detach(pthread_self());
  if(status !=0)
  {
    err_abort(status, "Detach Thread");
  }
  sleep(alarm->seconds);
  printf ("Alarm Expired: (%d) %s\n", alarm->seconds, alarm->message);

  free(alarm);
  return NULL;
}
