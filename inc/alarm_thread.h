#ifndef ALARM_THREAD
#define ALARM_THREAD

#include "error.h"
#include <time.h>

#define ALARM_MSG_MAX_LEN 64 

typedef struct alarm_tag
{
  struct alarm_tag *link;
  int seconds;
  time_t time;
  char message[ALARM_MSG_MAX_LEN];
} alarm_t;

void* alarm_thread_detach(void * arg);

#endif 
