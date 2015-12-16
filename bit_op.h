#ifndef __bit_op_H__
#define __bit_op_H__ 

#include <stdio.h>

inline unsigned int get_bit(const unsigned int *data, 
			    const unsigned long bit){
  return ((data[bit / (8 * sizeof(unsigned int))]) >> ((8 * sizeof(unsigned int)) - 1 - (bit % (8 * sizeof(unsigned int))))) & 1;  
}


int show_bit_pattern(const unsigned int n){
  unsigned long i;
  for(i = 0; i < 8 * sizeof(unsigned int); i++){
    printf("%d", get_bit(&n, i));
  }
  return 0;
}

int get_bit_test(){
  int i;
  unsigned int xi[2];
  xi[0] = 3;
  xi[1] = 15 << 10;

  printf("sizeof(unsigned int) : %ld\n", sizeof(unsigned int));

  for(i = 0; i < 2; i++){
    show_bit_pattern(xi[i]);
  }
  printf("\n");

  return 0;

}


#endif
