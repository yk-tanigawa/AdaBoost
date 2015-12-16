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
		unsigned int ***feature,
		unsigned long *featureLen){
  FILE *fp;
  char buf[BUF_SIZE];
  char *tok;
  const char *dlim = "\t";
  unsigned long i = 0, l = 0;

  unsigned int buffer = 0;

  *featureLen = wc(file);

  *feature = calloc_errchk(*featureLen, sizeof(int *), "calloc feature[]");
  *label = calloc_errchk(*featureLen, sizeof(int), "calloc label");

  if((fp = fopen(file, "r")) == NULL){
    fprintf(stderr, "error: fdopen %s\n%s\n",
	    file, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while(fgets(buf, BUF_SIZE, fp)){
    (*feature)[l] = calloc_errchk((featureDim + 8 * sizeof(unsigned int) - 1) / (8 * sizeof(unsigned int)),
				  sizeof(unsigned int), "calloc feature[l]");
    tok = strtok(buf, dlim);
    (*label)[l] = atoi(tok);
    tok = strtok(NULL, dlim);

    i = 0;
    buffer = 0;
    while(tok != NULL && i < featureDim){
      buffer = buffer << 1;
      buffer += (atoi(tok) & 1);
      if((i % (8 * sizeof(int))) == (8 * sizeof(int) - 1)){
	(*feature)[l][i++ / (8 * sizeof(int))] = buffer;
	buffer = 0;
      }
      tok = strtok(NULL, dlim);
    }
    buffer = buffer << ((8 * sizeof(int)) - (featureDim % (8 * sizeof(int))));

    (*feature)[l][featureDim / (8 * sizeof(int))] = buffer;
    l++;
  }
  
  fclose(fp);

  return 0;
}

int dump_all(FILE *fp,
	     const unsigned int **x, 
	     const int *y,
	     const int *pred,
	     const unsigned long n,
	     const unsigned long dim){

  unsigned long i, l;

  fprintf(fp, "%s\t%s\t%s\n", "pred", "y", "x");
  for(l = 0; l < n; l++){
    fprintf(fp, "%d\t%d\t", pred[l], y[l]);
    for(i = 0; i < dim; i++){
      fprintf(fp, "%d\t", get_bit(x[l], i));
    }
    fprintf(fp, "\n");
  }

  return 0;
}

#endif
