#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <getopt.h>

#define F_NAME_LEN 128
#define BUF_SIZE 4096

#include "calloc_errchk.h"
#include "adaboost.h"
#include "adaboost_io.h"
#include "bit_op.h"

void *calloc_errchk(size_t, size_t, const char *);

int adaboost_learn(const unsigned int **x, 
		   const int *y,
		   const unsigned long T,
		   const unsigned long N,
		   const unsigned long dim,
		   unsigned long **lernerAxis,
		   int **lernerPred,
		   double **beta);

int adaboost_apply(const unsigned long *lernerAxis,
		   const int *lernerPred,
		   const double *beta,
		   const unsigned long T,
		   const unsigned long N,
		   const unsigned int **x,
		   int **pred);

int main_sub(const char *file,
	     const unsigned long n, 
	     const unsigned long dim,
	     const char *outDir){
  FILE *fp;
  char *outFileNameBuf;

  unsigned long dataNum, l, t;
  unsigned int **data;
  int *label;

  unsigned long *lernerAxis;
  int *lernerPred;
  double *beta;

  int *pred;
  
  readFeature(file, dim, &label, &data, &dataNum);

  adaboost_learn((const unsigned int **)data,  
		 (const int *)label,
		 n, (const unsigned long)dataNum, dim,
		 &lernerAxis,
		 &lernerPred,
		 &beta);

  pred = calloc_errchk(dataNum, sizeof(int), "calloc pred");

  adaboost_apply((const unsigned long *)lernerAxis, (const int *)lernerPred,
		 (const double *)beta,
		 n, (const unsigned long)dataNum, 
		 (const unsigned int **)data,
		 &pred);

  if(outDir == NULL){
    dump_all(stderr, (const unsigned int **)data, (const int *)label, (const int *)pred, dataNum, dim);
  }else{
    outFileNameBuf = calloc_errchk(F_NAME_LEN, sizeof(char), "calloc outFileNameBuf");
    /* dump all */
    {
      sprintf(outFileNameBuf, "%sout", outDir);
      if((fp = fopen(outFileNameBuf, "w")) == NULL){
	fprintf(stderr, "error fpotn %s\n%s\n",
		outFileNameBuf, strerror(errno));
      }
      dump_all(fp, (const unsigned int **)data, (const int *)label, (const int *)pred, dataNum, dim);
      fclose(fp);
    }
    /* lerned model : axis */
    {
      sprintf(outFileNameBuf, "%sT%ld.axis", outDir, n);
      if((fp = fopen(outFileNameBuf, "w")) == NULL){
	fprintf(stderr, "error fpotn %s\n%s\n",
		outFileNameBuf, strerror(errno));
      }
      for(t = 0; t < n; t++){
	fprintf(fp, "%ld\n", lernerAxis[t]);       
      }
      fclose(fp);
    }
    /* lerned model : pred */
    {
      sprintf(outFileNameBuf, "%sT%ld.pred", outDir, n);
      if((fp = fopen(outFileNameBuf, "w")) == NULL){
	fprintf(stderr, "error fpotn %s\n%s\n",
		outFileNameBuf, strerror(errno));
      }
      for(t = 0; t < n; t++){
	fprintf(fp, "%d\n", lernerPred[t]);       
      }
      fclose(fp);
    }
    /* lerned model : beta */
    {
      sprintf(outFileNameBuf, "%sT%ld.beta", outDir, n);
      if((fp = fopen(outFileNameBuf, "w")) == NULL){
	fprintf(stderr, "error fpotn %s\n%s\n",
		outFileNameBuf, strerror(errno));
      }
      for(t = 0; t < n; t++){
	fprintf(fp, "%e\n", beta[t]);       
      }
      fclose(fp);
    }
    free(outFileNameBuf);
  }
  for(l = 0; l < dataNum; l++){
    free(data[l]);
  }
  free(data);
  free(label);
  free(lernerAxis);
  free(lernerPred);
  free(beta);
  free(pred);
  
  return 0;
}


int show_usage(const char *progName){
  printf("usage:\n\n");
  printf("example: %s -f %s -n %ld -d %ld, -o %s\n", 
	 progName,
	 "<file name>",
	 3L,
	 4L,
	 "./data/");
  return 0;
}

int check_params(const char *file,
		 const unsigned long n, 
		 const unsigned long d,
		 const char *outDir, 
		 const char *progName){
  int errflag = 0;
  if(file == NULL){
    fprintf(stderr, "input file is not specified\n");
    errflag += 1;
  }else{
    printf("input file:    %s\n", file);
  }

  if(n <= 0){
    fprintf(stderr, "number of classifier is not specified\n");
    errflag += 1;
  }else{
    printf("n: %ld\n", n);
  }

  if(d <= 0){
    fprintf(stderr, "dimension of the data is not specified\n");
    errflag += 1;
  }else{
    printf("d: %ld\n", d);
  }

  if(outDir == NULL){
    fprintf(stderr, "output directory is not specified\n");
    fprintf(stderr, "results will be written to stderr\n");
  }else{
    printf("Output dir:    %s\n", outDir);
  }

  if(errflag != 0){
    show_usage(progName);
    exit(EXIT_FAILURE);
  }

  return 0;
}

#if 1
int main(int argc, char **argv){
  char *file = NULL, *outDir = NULL;
  unsigned long d = 0, n = 0;

  int opt = 0, opt_idx = 0;
  struct option long_opts[] = {
    {"help",    no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    /* options */
    {"file",    required_argument, NULL, 'f'},
    {"dim",     required_argument, NULL, 'd'},
    {"num",     required_argument, NULL, 'n'},
    {"out",     required_argument, NULL, 'o'},
    {0, 0, 0, 0}
  };

  while((opt = getopt_long(argc, argv, "hvf:d:n:o:",
			   long_opts, &opt_idx)) != -1){
    switch (opt){
    case 'h': /* help */
      show_usage(argv[0]);
      exit(EXIT_SUCCESS);
    case 'v': /* version*/
      printf("version: 1.02\n");
      exit(EXIT_SUCCESS);
    case 'f': /* file */
      file = optarg;
      break;
    case 'd': /* dim */
      d = atol(optarg);
      break;
    case 'n': /* num */
      n = atol(optarg);
      break;
    case 'o': /* out */
      outDir = optarg;
      break;
    }
  }

  check_params(file, n, d, outDir, argv[0]);
  main_sub(file, n, d, outDir);
 
  return 0;
}

#endif

#if 0
int main(void){
  get_bit_test();
  return 0;
}
#endif
