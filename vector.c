#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation) {
  
  assert(elemSize > 0);
  assert(initialAllocation >= 0);
  
  v->grow_length = (initialAllocation > 0) ? initialAllocation : 4;
  v->alloc_length = v->grow_length;
  v->log_length = 0;
  v->elem_size = elemSize;
  v->FreeFunction = freeFn;

  v->elems = malloc( elemSize * v->alloc_length );
  assert(v->elems != NULL);
}

void VectorDispose(vector *v) {
  if (v->FreeFunction != NULL)
  {
    for (int i = 0; i < v->log_length; i++) {

      v->FreeFunction( (char*)v->elems + i * v->elem_size );
    }
  }
  free(v->elems);
}

int VectorLength(const vector *v) {
  return v->log_length;
}

void *VectorNth(const vector *v, int position) { 
  assert (position >= 0);
  assert (position < v->log_length);
  return (char*)v->elems + position * v->elem_size; 
}

void VectorReplace(vector *v, const void *elemAddr, int position) {
}

void VectorInsert(vector *v, const void *elemAddr, int position) {
}

static void VectorGrow(vector *v) {
  //attempt to reallocate 
  int new_alloc_length = v->alloc_length + v->grow_length;
  void *new_elems = realloc(v->elems, v->elem_size * new_alloc_length);
  assert(new_elems != NULL);

  //passed the assert, so realloc was successful
  v->alloc_length = new_alloc_length;
  v->elems = new_elems;
}

void VectorAppend(vector *v, const void *elemAddr) {
  // check if we need to grow, and do it if we need to. 
  if (v->log_length == v->alloc_length) VectorGrow(v);

  void *target = (char*)v->elems + v->log_length * v->elem_size;
  memcpy(target, elemAddr, v->elem_size);
  v->log_length++;
}

void VectorDelete(vector *v, int position) {
}

void VectorSort(vector *v, VectorCompareFunction compare) {
  assert(compare != NULL);
  qsort(v->elems, v->log_length, v->elem_size, compare );
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData) {
  char* end_address = (char*)v->elems + v->log_length * v->elem_size;
  for( char* target = v->elems; target < end_address; target += v->elem_size) {
    mapFn(target, auxData);
  }
}

static const int kNotFound = -1;
// returns the integer position of the matching element.  Otherwise, returns -1.
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted) { 

  assert(startIndex >= 0);
  assert(startIndex <= v->log_length);
  assert(key != NULL);
  assert(searchFn != NULL);

  char *found_addr;
  if (isSorted) {
    found_addr = bsearch(key, v->elems, v->log_length, v->elem_size, searchFn);
    if (found_addr == NULL) return kNotFound;
  }
  else {
    // incrementing pointers here to avoid a multiplication (by elem_size) each iteration; 
    char* end_address = (char*)v->elems + v->log_length * v->elem_size;
    for( found_addr = (char*)v->elems + startIndex * v->elem_size ; found_addr < end_address; found_addr += v->elem_size) {
      if (searchFn(key, found_addr) == 0) break;
    }
    if (found_addr >= end_address) return kNotFound;
  }

  return (found_addr - (char*)v->elems)/v->elem_size; 
} 
