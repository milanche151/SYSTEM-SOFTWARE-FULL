#include "util.h"

void* myMalloc(size_t size){
  void* ret;
  ret=malloc(size);
  if(ret==NULL){
    exit(-1);
  }
  return ret;
}
void* myRealloc(void* ptr,size_t size){
  void* ret;
  ret=realloc(ptr,size);
  if(ret==NULL){
    exit(-1);
  }
  return ret;
}
void myFree(void* ptr){
  free(ptr);
}