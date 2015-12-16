#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <getopt.h>



#define F_NAME_LEN 128
#define BUF_SIZE 4096

#define TP 0
#define FP 1
#define FN 2
#define TN 3

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

int adaboost_calloc(const unsigned long T,		    
		    const unsigned long N,
		    const unsigned long dim,
		    double **w,
		    double **p,
		    double **err,
		    unsigned long **lernerAxsis,
		    int **lernerPred,
		    double **beta){
  *lernerAxsis = calloc_errchk(T, sizeof(unsigned long), "calloc lernerAxsis");
  *lernerPred = calloc_errchk(T, sizeof(int), "calloc lernerPred");
  *w = calloc_errchk(N, sizeof(double), "calloc w");
  *p = calloc_errchk(N, sizeof(double), "calloc p");
  *beta = calloc_errchk(T, sizeof(double), "calloc beta");
  *err = calloc_errchk(dim, sizeof(double), "calloc count");
  return 0;
}

		           	    
int adaboost_learn(const int **x, 
		   const int *y,
		   const unsigned long T,
		   const unsigned long N,
		   const unsigned long dim,
		   unsigned long **lernerAxis,
		   int **lernerPred,
		   double **beta){

  double *w, *p, *err;
  double wsum, epsilon, min, max;

  unsigned long i, t, d, argmind, argmaxd;

  adaboost_calloc(T, N, dim, &w, &p, &err, lernerAxis, lernerPred, beta);		  		  

  for(i = 0; i < N; i++){
    w[i] = 1.0 / N;
  }

  for(t = 0; t < T; t++){
    /* step 1 : compute normalized weights p[] */
    {
      wsum = 0;
      for(i = 0; i < N; i++){
	wsum += w[i];
      }
      for(i = 0; i < N; i++){
	p[i] = w[i] / wsum;
      }
    }

    /* step 2 : find the most appropriate weak lerner */
    {
      for(d = 0; d < dim; d++){
	err[d] = 0;
      }
      for(i = 0; i < N; i++){
	for(d = 0; d < dim; d++){
	  if(x[i][d] != y[i]){
	    err[d] += p[i];
	  }
	}
      }

      {
	max = min = err[0];
	argmaxd = argmind = 0;
	for(d = 1; d < dim; d++){
	  if(err[d] < min){
	    min = err[d];
	    argmind = d;
	  }else if(err[d] > max){
	    max = err[d];
	    argmaxd = d;
	  }
	}
      }

      {
	if(max + min > 1.0){
	  (*lernerAxis)[t] = argmaxd;
	  (*lernerPred)[t] = 1;
	  epsilon = 1 - max;
	}else{
	  (*lernerAxis)[t] = argmind;
	  (*lernerPred)[t] = 0;
	  epsilon = min;       
	}      
      }
    }

    /* step 3: compute new weithgts */
    {
      (*beta)[t] = epsilon / (1 - epsilon);
      for(i = 0; i < N; i++){
	if(((*lernerPred)[t] == 0 && x[i][(*lernerAxis)[t]] == y[i]) ||
	   ((*lernerPred)[t] == 1 && x[i][(*lernerAxis)[t]] != y[i])){
	  w[i] *= (*beta)[t];
	}
      }
    }
    
  }

  free(w);
  free(p);
  free(err);
  return 0;
}

int dump_data(const int **x, 
	      const int *y,
	      const unsigned long n,
	      const unsigned long dim){

  unsigned long i, l;

  for(l = 0; l < n; l++){
    fprintf(stderr, "%d\t", y[l]);
    for(i = 0; i < dim; i++){
      fprintf(stderr, "%d\t", x[l][i]);
    }
    fprintf(stderr, "\n");
  }

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

int adaboost_apply(const unsigned long *lernerAxis,
		   const int *lernerPred,
		   const double *beta,
		   const unsigned long T,
		   const unsigned long N,
		   const int **x,
		   int **pred){

  unsigned int i, t;
  double threshold = 0, sum;
  {
    for(t = 0; t < T; t++){
      threshold -= log(beta[t]);
    }
    threshold /= 2;
  }
  for(i = 0; i < N; i++){
    sum = 0;
    for(t = 0; t < T; t++){
      if(lernerPred[t] != x[i][lernerAxis[t]]){
	sum -= log(beta[t]);
      }
    }
    (*pred)[i] = ((sum >= threshold) ? 1 : 0);
  }
  return 0;
}

int main_sub(const char *file,
	     const unsigned long n, 
	     const unsigned long dim,
	     const char *outDir){
  FILE *fp;
  char *outFileNameBuf;

  unsigned long dataNum, l, t;
  int **data;
  int *label;

  unsigned long *lernerAxis;
  int *lernerPred;
  double *beta;

  int *pred;
  
  readFeature(file, dim, &label, &data, &dataNum);

  adaboost_learn((const int **)data,  
		 (const int *)label,
		 n, (const unsigned long)dataNum, dim,
		 &lernerAxis,
		 &lernerPred,
		 &beta);

  pred = calloc_errchk(dataNum, sizeof(int), "calloc pred");

  adaboost_apply((const unsigned long *)lernerAxis, (const int *)lernerPred,
		 (const double *)beta,
		 n, (const unsigned long)dataNum, 
		 (const int **)data,
		 &pred);

  if(outDir == NULL){
    dump_all(stderr, (const int **)data, (const int *)label, (const int *)pred, dataNum, dim);
  }else{
    outFileNameBuf = calloc_errchk(F_NAME_LEN, sizeof(char), "calloc outFileNameBuf");
    /* dump all */
    {
      sprintf(outFileNameBuf, "%sout", outDir);
      if((fp = fopen(outFileNameBuf, "w")) == NULL){
	fprintf(stderr, "error fpotn %s\n%s\n",
		outFileNameBuf, strerror(errno));
      }
      dump_all(fp, (const int **)data, (const int *)label, (const int *)pred, dataNum, dim);
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
      printf("version: 1.01\n");
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

