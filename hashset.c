#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL);
	assert(comparefn != NULL);

	//h->elem_size = elemSize;
	h->n_buckets = numBuckets;
	h->Hash = hashfn;
	h->Compare = comparefn;

	// put an array of vectors in the heap. buckets is the address of the first vector. 
	h->buckets = (vector*)malloc(sizeof(vector) * numBuckets);
	assert (h->buckets != NULL );

	// initialize each vector in the array. 
	for (int i = 0; i < numBuckets; i++) {
		// Because buckets is an array of vectors, (and we know sizeof(vector), we can use [].
		VectorNew( &(h->buckets[i]), elemSize, freefn, 4);
	}
}

void HashSetDispose(hashset *h)
{
	// call VectorDispose for each vector in the array.  The Free function was passed in VectorNew.
	// This calls the free function for each element, and frees the elems array for each vector, 
	// but doesn't free the array of vector structs that h->buckets points to. 
	for (int i = 0; i < h->n_buckets; i++){
		VectorDispose(&h->buckets[i]);
	}

	// free the vector array
	free(h->buckets);
}

int HashSetCount(const hashset *h) {
	// sum each n_elements from each bucket
	int n_total = 0;
	for (int i = 0; i < h->n_buckets; i++) {
		n_total += VectorLength(&h->buckets[i]);
	}
	return n_total; 
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData) {
	assert(h != NULL);
	assert(mapfn != NULL);

	// iterate over the buckets, calling VectorMap on each,  
	// and passing the map function to the vector each time 
	for (int i = 0; i < h->n_buckets; i++) {
		VectorMap(&h->buckets[i], mapfn, auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr) {
	assert(elemAddr != NULL);
	assert(h != NULL);
	
	//hash the element to find the right bucket. 
	int i = h->Hash(elemAddr, h->n_buckets);
	assert(i >= 0);
	assert(i < h->n_buckets);

	vector* bucket = &h->buckets[i];

	// we're not worrying about staying sorted in this implementation. 
	// search the bucket to see if there's already an element there 
	int found = VectorSearch(bucket, elemAddr, h->Compare, 0, false);
	
	if (found < 0 )
		VectorAppend(bucket, elemAddr );
	else 
		VectorReplace(bucket, elemAddr, found);

	// if we wanted to stay sorted, we could call VectorSort(), or we could 
	// VectorFind() the right index, then VectorInsert() at that index. 

}

void *HashSetLookup(const hashset *h, const void *elemAddr) { 
	assert(elemAddr != NULL);

	int i = h->Hash(elemAddr, h->n_buckets);
	assert(i >= 0);
	assert(i < h->n_buckets);

	int k = VectorSearch(&h->buckets[i], elemAddr, h->Compare, 0, false);
	
	return k >= 0 ? VectorNth( &h->buckets[i], k ) : NULL;
}
