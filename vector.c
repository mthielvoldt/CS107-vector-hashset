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
  assert(position >= 0);
  assert(position < v->log_length);
  assert(elemAddr != NULL);

  void *target = (char*)v->elems + position * v->elem_size;
  // if a Free Function has been provided to de-allocate elements, us it before we torch this one. 
  if (v->FreeFunction != NULL)
    v->FreeFunction(target);

  memcpy(target, elemAddr, v->elem_size);
  
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

//  To Test
void VectorInsert(vector *v, const void *elemAddr, int position) {
  // asserts 
  assert(position >= 0);
  assert(position <= v->log_length);
  assert( elemAddr != NULL);

  // first, grow if we need to
  if ( v->log_length == v->alloc_length) VectorGrow(v);

  // shift all elements from <position> to the end of the vector one elem_size
  // towards the end of the vector to make room for new element at <position>.
  void* src_addr = (char*)v->elems + position * v->elem_size;
  void* dest_addr = (char*)src_addr + v->elem_size;
  int n_bytes_to_move = (v->log_length - position) * v->elem_size;
  memmove( dest_addr, src_addr, n_bytes_to_move );
  v->log_length++;

  // write the new data at <position>.  
  // We can't use VectorReplace here because that would call FreeFunction on the element 
  // that's currently still at <position>, which is now also just to the right of <position>
  // We need to just memcpy the new data in. 
  void *target = (char*)v->elems + position * v->elem_size;
  memcpy(target, elemAddr, v->elem_size);
}

void VectorAppend(vector *v, const void *elemAddr) {
  // check if we need to grow, and do it if we need to. 
  if (v->log_length == v->alloc_length) VectorGrow(v);

  void *target = (char*)v->elems + v->log_length * v->elem_size;
  memcpy(target, elemAddr, v->elem_size);
  v->log_length++;
}

// to test
void VectorDelete(vector *v, int position) {
  assert(position >= 0);
  assert(position < v->log_length);
  
  // calculate the address of the to-be-deleted element. 
  void* dest_addr = (char*)v->elems + position * v->elem_size;

  // call the free function on the to-be-deleted element. 
  if (v->FreeFunction != NULL)
    v->FreeFunction(dest_addr);
  
  // shift all elements to the right of <position> to the left by one space.
  int n_bytes_to_move = (v->log_length - position - 1) * v->elem_size;
  
  if (n_bytes_to_move > 0 ) { // we don't need to call memmove if we're deleting the last element. 
    void* src_addr = (char*)dest_addr + v->elem_size;
    memmove( dest_addr, src_addr, n_bytes_to_move );
  }    
  
  v->log_length--; 
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
int VectorSearch(const vector *v, const void *key, VectorCompareFunction compareFn, int startIndex, bool isSorted) { 

  assert(startIndex >= 0);
  assert(startIndex <= v->log_length);
  assert(key != NULL);
  assert(compareFn != NULL);

  char *found_addr;
  if (isSorted) {
    found_addr = bsearch(key, v->elems, v->log_length, v->elem_size, compareFn);
    if (found_addr == NULL) return kNotFound;
  }
  else {
    // incrementing pointers here to avoid a multiplication (by elem_size) each iteration; 
    char* end_address = (char*)v->elems + v->log_length * v->elem_size;
    for( found_addr = (char*)v->elems + startIndex * v->elem_size ; found_addr < end_address; found_addr += v->elem_size) {
      if (compareFn(key, found_addr) == 0) break;
    }
    if (found_addr >= end_address) return kNotFound;
  }
  // We still need to compute the integer position from the address. 
  return (found_addr - (char*)v->elems)/v->elem_size; 
} 
