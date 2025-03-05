#ifndef UTIL_H_
#define UTIL_H_

#include<stdint.h>
#include<stdlib.h>


void* myMalloc(size_t size);
void* myRealloc(void* ptr,size_t size);
void myFree(void* ptr);




#endif
