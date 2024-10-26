# 1 "inc/vector.h"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "inc/vector.h"
# 42 "inc/vector.h"
void* malloc(size_t);

typedef struct VecInt { 
  typeof(int)* data; 
  size_t size; 
  size_t cap; 
} VecInt;

VecInt VecIntCreate(void);
void VecIntDestroy(VecInt* vec);
void VecIntPush(VecInt* vec, typeof(int) val);

struct VecInt;

VecInt VecIntCreate(void){ 
  VecInt vec = { 
    .data = realloc(sizeof(typeof(int)) * 4), 
    .size = 0, 
    .cap = 4, 
  };

  return vec; 
} 

void VecIntDestroy(VecInt* vec){ 
  free(vec->data);
}

void VecIntPush(VecInt* vec, typeof(int) val){
  if(vec->cap <= vec->size){ 
    vec->cap *= 2; 
    vec->data = realloc(vec->data, vec->cap); 
  }
  vec->data[vec->size++] = val;
};
