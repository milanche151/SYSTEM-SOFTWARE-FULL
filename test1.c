#include <stdio.h>
#include "vector.h"

VECTOR_DECLARE(VecInt, int);

void VecIntPrint(const VecInt* vec){
  printf("data: %p, size: %lu, cap: %lu\n", vec->data, vec->size, vec->cap);
}

int main(void){
  VecInt vec = VecIntCreate();

  VecIntPrint(&vec);
  for(int i = 0; i < 10; i++){
    VecIntPush(&vec, i);
    VecIntPrint(&vec);
  }

  for(int i = 0; i < vec.size; i++){
    printf("%d ", vec.data[i]);
  }

  VecIntDestroy(&vec);
}