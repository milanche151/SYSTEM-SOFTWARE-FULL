#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdlib.h>
#include <stdint.h>

#define VECTOR_DECLARE(Vector, Type) \
typedef struct Vector { \
  typeof(Type)* data; \
  size_t size; \
  size_t cap; \
} Vector; \
\
Vector Vector ## Create(void); \
void Vector ## Destroy(Vector* vec); \
void Vector ## Push(Vector* vec, typeof(Type) val); \


#define DEFAULT_VECTOR_CAP 4

#define VECTOR_IMPLEMENT(Vector, Type) \
Vector Vector ## Create(void){ \
  Vector vec = { \
    .data = realloc(NULL, sizeof(typeof(Type)) * DEFAULT_VECTOR_CAP), \
    .size = 0, \
    .cap  = DEFAULT_VECTOR_CAP, \
  }; \
  return vec; \
} \
\
void Vector ## Destroy(Vector* vec){ \
  free(vec->data); \
} \
\
void Vector ## Push(Vector* vec, typeof(Type) val){ \
  if(vec->cap <= vec->size){ \
    vec->cap *= 2; \
    vec->data = realloc(vec->data, sizeof(typeof(Type)) * vec->cap); \
  } \
  vec->data[vec->size++] = val; \
} \


#endif