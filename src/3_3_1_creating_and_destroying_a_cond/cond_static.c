#include <pthread.h>
#include "error.h"

/*
 * Declares a sturcture, with a mutex and condition variable,
 * statically initailized. This is the same as using
 * pthread_mutex_init and pthread_cond_init, with the default attribute
 */

// Good practice to declaring the condition variable and predicate together
//
typedef struct my_struct_tag
{
  pthread_mutex_t mutex; /* Portects access to value*/
  pthread_cond_t cond;   /* Signals change to value*/
  int value;
} my_struct_t; 
// A condition variable should always be associate with a predicate,
// A mutex can be associated with multiple condition variables
// E.G one mutex to lock the queue, and 2 predicate 
// queue_is_full and queue_is_empty are associated with 2 condition variables
// to signal other thread about these states.

// stastically initialized condition vars/ mutex does not need to be destroyed
my_struct_t data = {PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER,0};

int main(int argc, char* argv[])
{
  return 0;
}
