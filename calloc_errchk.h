#ifndef __calloc_errchk_H__
#define __calloc_errchk_H__ 

#include <stdlib.h>
#include <errno.h>

void *calloc_errchk(size_t count,
		    size_t size,
		    const char *errmsg){
  void *mem;
  if((mem = calloc(count, size)) == NULL){
    perror(errmsg);
    exit(EXIT_FAILURE);
  }else{
    return mem;
  }
}

#endif
