#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <getopt.h>



#define F_NAME_LEN 128
#define BUF_SIZE 4096

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
  long i = 0, l = 0;

  *featureLen = wc(file);
  
  if((*feature = calloc(sizeof(int *), *featureLen)) == NULL){
    perror("error(calloc) feature");
    exit(EXIT_FAILURE);
  }

  if((*label = calloc(sizeof(int), *featureLen)) == NULL){
    perror("error(calloc) label");
    exit(EXIT_FAILURE);
  }

  if((fp = fopen(file, "r")) == NULL){
    fprintf(stderr, "error: fdopen %s\n%s\n",
	    file, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while(fgets(buf, BUF_SIZE, fp)){
    /** memory allocation */
    if(((*feature)[l] = calloc(sizeof(int), featureDim)) == NULL){
      fprintf(stderr, "error(calloc) fature[%ld]\n%s\n",
	      l, strerror(errno));
      exit(EXIT_FAILURE);
    }

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

int main_sub(const char *file,
	     const unsigned long n, 
	     const unsigned long dim,
	     const char *outDir){

  unsigned long dataNum;
  int **data;
  int *label;

  unsigned long i, l;

  readFeature(file, dim, &label, &data, &dataNum);

  for(l = 0; l < dataNum; l++){
    fprintf(stderr, "%d\t", label[l]);
    for(i = 0; i < dim; i++){
      fprintf(stderr, "%d\t", data[l][i]);
    }
    fprintf(stderr, "\n");
  }

  return 0;
}


int show_usage(const char *progName){
  printf("usage:\n\n");
  printf("example: %s -f %s -n %ld -r %ld, -o %s\n", 
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
    errflag += 1;
  }else{
    printf("Output dir:    %s\n", outDir);
  }

  if(errflag != 0){
    show_usage(progName);
    exit(EXIT_FAILURE);
  }

  return 0;
}


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
      printf("version: 0.10\n");
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

