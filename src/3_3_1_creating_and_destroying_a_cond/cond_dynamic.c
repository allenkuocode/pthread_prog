#include <pthread.h>
#include "error.h"
#include "utils.h"
/*
 * Define a structure, with a mutex and condition variable.
*/
typedef struct my_struct_tag {
  pthread_mutex_t mutex; /*Protects access to variable */
  pthread_cond_t cond;   /* Signals change to variable */
  int value;             /* Access protected by mutex */
} my_struct_t;

int main (int argc, char * argv[])
{
  my_struct_t * data; 
  int status;

  data = Malloc(sizeof(my_struct_t));
  
  status = pthread_mutex_init (&data->mutex, NULL);
  if (status != 0) {err_abort(status, "Init mutex");}
  
  status = pthread_cond_init(&data->cond, NULL);
  if (status != 0) {err_abort(status,"Init condition");}
  
  status = pthread_cond_destroy(&data->cond);
  if(status != 0){ err_abort(status, "Destroy condition");}

  status = pthread_mutex_destroy(&data->mutex);
  if(status != 0){err_abort(status, "Destroy mutex");}
  
  // it is safe to destroy the condition var and mutex before freeing
  (void)free (data);
  return status;
}
