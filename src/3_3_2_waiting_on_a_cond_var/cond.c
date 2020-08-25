#include <pthread.h>
#include <time.h>
#include "error.h"
// Each condition variable must associate with single mutex at a time
// but single mutex can have any number of cond var associated with it
//
typedef struct my_struct_tag {
  pthread_mutex_t mutex; /* Protects access to variable*/
  pthread_cond_t cond;   /*signals change to variable*/
  int value;             /*Access protected by mutex*/
} my_struct_t;

my_struct_t data = {
  PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0
};

int hibernation = 1; /* Default to 1 second */

/*
 * Thread start routine. It will set the main thread's predicate and signal
 * the condition variable.
 */

void * wait_thread(void *arg)
{
  int status; 
  sleep(hibernation);

  status = pthread_mutex_lock(&data.mutex);
  if(status != 0){err_abort(status,"Lock mutex");}
  data.value = 1; /*Set predicate */
  status = pthread_cond_signal(&data.cond);
  if(status != 0){err_abort(status, "Signal condition");}

  status = pthread_mutex_unlock(&data.mutex);
  if (status != 0){err_abort (status, "Unlock mutex");}
  
  return NULL;
}

int main(int argc, char *argv[])
{
  int status; 
  pthread_t wait_thread_id;
  struct timespec timeout;

  /*
   * If an argument is specified, interpret it as the number of seconds for
   * wait_thread to sleep before signaling the condition variable. You can play
   * with this to see the condition wait below time out or wake normally.
   */
  if (argc >1){hibernation = atoi(argv[1]);}

  /*
   * Create wait_thread.
   */
  status = pthread_create (&wait_thread_id, NULL, wait_thread, NULL);
  if(status !=0){err_abort(status, "create wait thread");}

  /*
   * Wait on the condition variable for 2 seconds, or until signaled by the 
   * wait_thread. Normally, wait_thread should signal. If you raise 
   * "hibernation" above 2 seconds, it will time out.
   */
  
  timeout.tv_sec = time(NULL) + 2;
  timeout.tv_nsec = 0;
  status = pthread_mutex_lock(&data.mutex);
  if(status!=0){err_abort(status, "Lock mutex");}

  // always check the predicate in the while loop (check before & after the
  // thread is waken up)
  //
  // To avoid these scenarios:
  // 1.Intercepted wakeups
  // 2.Loose predicates
  // 3.Spurious wakeups
  while(data.value == 0){
    // condition wait will unlock the mutex before put the thread to sleep
    status = pthread_cond_timedwait(&data.cond, &data.mutex, &timeout);
    // when the thread awakes, 
    //                     <---------1.Intercepted wake up here (it locks the
    //                     mutex, do the work, set the predicate to true again
    //                     , and unlock the mutex, if this thread doesn't 
    //                     check the flag again, it will process the work
    //                     that is already done by the intercepting thread.')
    // it will relock the mutex
    if( status == ETIMEDOUT){
      printf ("Condition wait timed out .\n");
      break;
    }
    else if (status != 0){err_abort (status, "Wait on condition");}
  }
  if(data.value != 0) {printf("condition was signaled.\n");}

  status = pthread_mutex_unlock(&data.mutex);
  if(status!=0){err_abort(status, "UnLock mutex");}
  return 0;

}


