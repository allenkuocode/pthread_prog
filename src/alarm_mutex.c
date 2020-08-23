#include <pthread.h>
#include <time.h>
#include "error.h"
#include "alarm_thread.h"

#define ALARM_MSG_MAX_LEN 64
/* Uses mutex to coordinate read/write activity of the alarm requests*/

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
alarm_t * alarm_list = NULL;

