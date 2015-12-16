#ifndef __bit_op_H__
#define __bit_op_H__ 

#include <stdio.h>

inline unsigned int get_bit(const unsigned int *data, 
			    const unsigned long bit){
  return ((data[bit / (8 * sizeof(unsigned int))]) >> ((8 * sizeof(unsigned int)) - 1 - (bit % (8 * sizeof(unsigned int))))) & 1;  
}


int get_bit_test(){
  int i;
  unsigned int xi[2];
  xi[0] = 3;
  xi[1] = 15 << 10;

  printf("%ld\n", sizeof(unsigned int));
  for(i = 0; i < sizeof(unsigned int) * 8 * 2; i++){
    printf("%d", get_bit(xi, i));
  }
  printf("\n");

  return 0;

}


#endif
