
typedef struct {
  void* elems;
  int logLength;
  int elemSize;
  int allocLength; 
  void (*freeFunc)(void*)
} stack;

void StackNew( stack* s, int elem_size);
void StackDispose( stack* s);
void StackPush( stack* s, void* elemAddr);
void StackPop(stack* S , void* elemAddr);


void StackNew( stac * s, int elem_size)
{
  assert(elem_size > 0);
  s-> elemSize = elem_size;
  s->logLength = 0;
  s-> allocLength = 4;
  elems = malloc( s->allocLength * elem_size )
  assert ( elems != NULL);

}

static void StackGrow( stack* s)
{
  void* new_address = realloc( s, s->allocLength* s->elemSize*2);
  assert(new_address != NULL);
  s->allocLength *=2;
  s->elems = new_address;
}
void StackPush( Stack* s, void* elemAddr )
{ 
  // if there isn't enough room
  if (s->logLength == s->allocLength)
    StackGrow(s);
  
  void* target = (char*) s->elems + s->logLength * s->elemSize;

  // copy the bit pattern
  memcpy(target, elemAddr, s->elemSize);

  s->logLength++
}

void StackDispose( Stack* s )
{
  // if freeFunc is Null, there's no heap memory to free; 
  if (s->freeFunc != NULL) 
  {

    for ( int i = 0 ; i < s->logLength ; i++)
    {
      s->freeFunc( (char*)s->elems + i*s->elemSize ) ;
    }
  }

  s->free(elems);
}

// Client code
const char *frineds[] = {"Nancy", "Carl", "Tim"};
stack StringStack
StackNew( &StringStack, sizeof(char*), StringFree );
for (i 0:2) {
  char *copy = strdup(friends[i]);
  StackPush(&StringStack, &copy); // note it's the address of copy.  We're copying whatever is at the address. 

}

void StringFree( void* elem )
{
  // each elem is actually a pointer to a char*.  IOW a char**)
  free(*(char**)elem);
}