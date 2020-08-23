#ifndef __utils_h
#define __utils_h

#include <sys/cdefs.h>
#include <stdlib.h>
#include "error.h"

//inline void * Malloc(size_t size) __result_use_check __alloc_size(1);

static inline void * Malloc(size_t size) 
{
  void * ret_ptr;
  ret_ptr = malloc(size);
  if(ret_ptr == NULL)
  {
    errno_abort("Allocate structure Failed");
  }
  return ret_ptr;
}

#endif 

