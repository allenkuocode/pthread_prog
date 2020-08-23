#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "utils.h"

/* Declares a structure with a mutex, 
 * This is the same as using pthread_mutex_init(), with the default attribtues.
 * It is useful to keep the mutex with the data it protects in the same struct
 */
typedef struct my_struct_tag
{
  pthread_mutex_t mutex; /* Protects access to value */
  int             value; /* Access protected by mutex*/
} my_struct_t;

// Statically initialized mutex does not need to be destroyed
my_struct_t static_data = {PTHREAD_MUTEX_INITIALIZER, 0};


int main()
{
  my_struct_t *dynamic_data;
  dynamic_data = Malloc(sizeof(*dynamic_data));

  printf("Alloc successful\n");

  int status = pthread_mutex_init(&dynamic_data->mutex, NULL);
  if(status != 0)
  {
    err_abort(status, "Init mutex");
  }
  status = pthread_mutex_destroy(&dynamic_data->mutex);
  if(status !=0)
  {
    err_abort(status, "Destroy mutex");
  }
  
  // safe to unlock and destroy mutex before freeing
  (void)free(dynamic_data);
  return status;
}

