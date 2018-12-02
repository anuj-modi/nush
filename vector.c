#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"

// allocates memry for the vector and returns a pointer to it
vector*
make_vector()
{
  vector* v = malloc(sizeof(vector));
  v->size = 0;
  v->capacity = 4;
  v->array = malloc(4 * sizeof(char*));
  memset(v->array, 0, 4 * sizeof(char*));
  return v;
}

// frees the memory allocated for the vector
void
free_vector(vector* v)
{
  for (int i = 0; i < v->size; i++) {
    if (v->array[i] != 0) {
      free(v->array[i]);
    }
  }
  free(v->array);
  free(v);
}

// adds an item to the end of the vector.
// it will resize the underlying array if size == capacity
void
add_to_vector(vector* v, char* str)
{
  if (v->size == v->capacity) {
    v->capacity *= 2;
    v->array = (char**) realloc(v->array, v->capacity * sizeof(char*));
  }
  v->array[v->size] = strdup(str);
  v->size++;
}

// returns the pointer at the specified index
char*
get_from_vector(vector* v, int index)
{
  assert(index >= 0 && index < v->size);
  return v->array[index];
}

// checks if the vector contains the specified item
int
vector_contains(vector* v, char* str)
{
  for (int i = 0; i < v->size; i++) {
    if (strcmp(get_from_vector(v, i), str) == 0) {
      return 1;
    }
  }
  return 0;
}

// returns a new vector including all the elements from low to high excluding
// high but including low
vector*
subvector(vector* v, int low, int high)
{
  vector* result = make_vector();

  for(int i = low; i < high; i++) {
    add_to_vector(result, get_from_vector(v, i));
  }
  return result;
}

// returns the index of the first occurence of the given item in the vector
// returns -1 if the item is not in the vector
int
index_of(vector* v, char* str)
{
  for (int i = 0; i < v->size; i++) {
    if (strcmp(get_from_vector(v, i), str) == 0) {
      return i;
    }
  }

  return -1;
}

// returns the last of the first occurence of the given item in the vector
// returns -1 if the item is not in the vector
int
last_index_of(vector* v, char* str)
{
  for (int i = v->size - 1; i >= 0; i--) {
    if (strcmp(get_from_vector(v, i), str) == 0) {
      return i;
    }
  }

  return -1;
}

