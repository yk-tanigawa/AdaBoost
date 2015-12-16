#ifndef __data_load_H__
#define __data_load_H__ 

#include <stdio.h>
#include <string.h>
#include "calloc_errchk.h"

#define BUF_SIZE 4096

void *calloc_errchk(size_t, size_t, const char *);

long wc(const char *fName){
  long lines = 0;
  FILE* fp;
  char buf[BUF_SIZE];
  
  if((fp = fopen(fName, "r")) == NULL){
    fprintf(stderr, "error: fdopen %s\n%s\n",
	    fName, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while(fgets(buf, BUF_SIZE, fp)){
    lines++;
  }

  fclose(fp);

  return lines;
}

int readFeature(const char *file,
		const unsigned long featureDim,
		int **label,
		int ***feature,
		unsigned long *featureLen){
  FILE *fp;
  char buf[BUF_SIZE];
  char *tok;
  const char *dlim = "\t";
  unsigned long i = 0, l = 0;

  *featureLen = wc(file);

  *feature = calloc_errchk(*featureLen, sizeof(int *), "calloc feature[]");
  *label = calloc_errchk(*featureLen, sizeof(int), "calloc label");

  if((fp = fopen(file, "r")) == NULL){
    fprintf(stderr, "error: fdopen %s\n%s\n",
	    file, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while(fgets(buf, BUF_SIZE, fp)){
    (*feature)[l] = calloc_errchk(featureDim, sizeof(int), "calloc feature[l]");

    tok = strtok(buf, dlim);
    (*label)[l] = atoi(tok);
    tok = strtok(NULL, dlim);

    i = 0;
    while(tok != NULL && i < featureDim){
      (*feature)[l][i++] = atoi(tok);
      tok = strtok(NULL, dlim);
    }
    l++;
  }
  
  fclose(fp);

  return 0;
}

int dump_all(FILE *fp,
	     const int **x, 
	     const int *y,
	     const int *pred,
	     const unsigned long n,
	     const unsigned long dim){

  unsigned long i, l;

  fprintf(fp, "%s\t%s\t%s\n", "pred", "y", "x");
  for(l = 0; l < n; l++){
    fprintf(fp, "%d\t%d\t", pred[l], y[l]);
    for(i = 0; i < dim; i++){
      fprintf(fp, "%d\t", x[l][i]);
    }
    fprintf(fp, "\n");
  }

  return 0;
}

#endif
