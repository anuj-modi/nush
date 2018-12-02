#ifndef VECTOR_H
#define VECTOR_H

// a resizable array implementation
typedef struct vector {
	int capacity;
	int size;
	char** array;
} vector;

// allocates memry for the vector and returns a pointer to it
vector* make_vector();

// frees the memory allocated for the vector
void free_vector(vector* v);

// adds an item to the end of the vector.
// it will resize the underlying array if size == capacity
void add_to_vector(vector* v, char* str);

// returns the pointer at the specified index
char* get_from_vector(vector* v, int index);

// checks if the vector contains the specified item
int vector_contains(vector* v, char* str);

// returns a new vector including all the elements from low to high excluding
// high but including low
vector* subvector(vector* v, int low, int high);

// returns the index of the first occurence of the given item in the vector
// returns -1 if the item is not in the vector
int index_of(vector* v, char* str);

// returns the index of the rst occurence of the given item in the vector
// returns -1 if the item is not in the vector
int last_index_of(vector* v, char* str);

#endif
