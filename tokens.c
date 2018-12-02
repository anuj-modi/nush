
// You'll want to compile this to a binary called "tokens".

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vector.h"

// determines if the character could be part of a token that isn't an operator
int
is_word_letter(char character)
{
  return isalnum(character) || character == '/' || character == '.'
  || character == '_' || character == '-';
}

// reads through the given string until it reaches the end of the token and
// returns that word
char*
read_word(char* string, int index)
{
  int i = 0;
  while (is_word_letter(string[index + i])){
    i++;
  }
  char* result = malloc((i + 1) * sizeof(char));
  memcpy(result, string + index, i);
  result[i] = 0;
  return result;
}

// tokenizes a string into a vector so that each token is an item in the vector
vector*
tokenize(char* input)
{
  vector* v = make_vector();
  int size = strlen(input);
  int i = 0;
  while (i < size) {
    if (isspace(input[i])) {
      i++;
    } else if (is_word_letter(input[i])) {
      char* word = read_word(input, i);
      add_to_vector(v, word);
      i += strlen(word);
      free(word);
    } else if (input[i] == '<' || input[i] == '>' || input[i] == ';') {
      char op[2] = "x";
      op[0] = input[i];
      add_to_vector(v, op);
      i++;
    } else if (input[i] == input[i + 1]) { // if it is a && or ||
      char ops[3] = "xx";
      ops[0] = input[i];
      ops[1] = input[i];
      add_to_vector(v, ops);
      i += 2;
    } else { // if it is a & or |
      char op[2] = "x";
      op[0] = input[i];
      add_to_vector(v, op);
      i++;
    }
  }
  return v;
}

// int
// main(int _argc, char* _argv[])
// {
//     char line[100];
//     while (1) {
//       printf("tokens$ ");
//         char* input = fgets(line, 96, stdin);
//
//         if(!input) {
//           printf("\n");
//           break;
//         }
//
//         vector* tokenized = tokenize(input);
//
//         // prints the item in the vector item by item
//         for (int i = 0; i < tokenized->size; i++) {
//           printf("%s\n", get_from_vector(tokenized, i));
//         }
//
//         free_vector(tokenized);
//
//     }
//     return 0;
// }
