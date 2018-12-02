#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#include "tokens.h"
#include "vector.h"

void execute(vector* v);

void
run(vector* v)
{
  // The argv array for the child.
  // Terminated by a null pointer.
  char* args[v->size + 1];
  for (int i = 0; i < v->size; i++) {
    args[i] = get_from_vector(v, i);
  }

  args[v->size] = 0;

  execvp(args[0], args);
  _Exit(0);
}

void
execute_command(vector* v)
{

  if (strcmp(get_from_vector(v, 0), "cd") == 0) {
    chdir(get_from_vector(v, 1));
    return;
  } else if (strcmp(get_from_vector(v, 0), "exit") == 0) {
    exit(0);
  }

  int cpid;

  if ((cpid = fork())) {

    // Child may still be running until we wait.
    int status;
    waitpid(cpid, &status, 0);
  }
  else {
    // The argv array for the child.
    // Terminated by a null pointer.
    run(v);
  }
}

void
execute_redirect_in(vector* v)
{
  int loc = index_of(v, "<");
  vector* left = subvector(v, 0, loc);
  vector* right = subvector(v, loc + 1, v->size);

  int cpid;

  if ((cpid = fork())) {
    // Child may still be running until we wait.
    int status;
    waitpid(cpid, &status, 0);

    free_vector(left);
    free_vector(right);
  }
  else {
    FILE* inputFile = fopen(get_from_vector(right, 0), "r");
    int inputDescriptor = fileno(inputFile);
    close(0); // close stdin
    dup(inputDescriptor); // replace 0 with the file
    execute(left);
  }
}

void
execute_redirect_out(vector* v)
{
  int loc = index_of(v, ">");
  vector* left = subvector(v, 0, loc);
  vector* right = subvector(v, loc + 1, v->size);

  int cpid;

  if ((cpid = fork())) {
    // Child may still be running until we wait.
    int status;
    waitpid(cpid, &status, 0);

    free_vector(left);
    free_vector(right);
  }
  else {

    FILE* outputFile = fopen(get_from_vector(right, 0), "w");
    int outputDescriptor = fileno(outputFile);
    close(1); // close stdout
    dup(outputDescriptor); // replace 0 with the file
    execute(left);
  }
}

void
execute_background(vector* v)
{
  int cpid;

  if (!(cpid = fork())) {
    vector* cmd = subvector(v, 0, v->size - 1);
    run(cmd);
  }
}

void
execute_and(vector* v)
{
  int loc = index_of(v, "&&");
  vector* left = subvector(v, 0, loc);
  vector* right = subvector(v, loc + 1, v->size);

  int cpid;

  if ((cpid = fork())) {

    // Child may still be running until we wait.
    int status;
    waitpid(cpid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      execute(right);
    }

    free_vector(left);
    free_vector(right);
  }
  else {
    // The argv array for the child.
    // Terminated by a null pointer.
    run(left);

  }
}

void
execute_or(vector* v)
{
  int loc = index_of(v, "||");
  vector* left = subvector(v, 0, loc);
  vector* right = subvector(v, loc + 1, v->size);

  int cpid;

  if ((cpid = fork())) {

    // Child may still be running until we wait.
    int status;
    waitpid(cpid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
      execute(right);
    }

    free_vector(left);
    free_vector(right);
  }
  else {
    run(left);
  }
}

void
execute_pipe(vector* v)
{
  int loc = last_index_of(v, "|");
  vector* left = subvector(v, 0, loc);
  vector* right = subvector(v, loc + 1, v->size);

  int cpid;


  if ((cpid = fork())) {
    int status;
    waitpid(cpid, &status, 0);

    free_vector(left);
    free_vector(right);
  }
  else {

    int pipes[2];
    int val = pipe(pipes);
    assert(val != -1);

    int p_read = pipes[0];
    int p_write = pipes[1];
    
    int cpid2;

    if ((cpid2 = fork())) {
      close(p_read);
      close(1); // close stdout
      dup(p_write);
      execute(left);
      _Exit(0);

    } else {
      int status2;
      close(p_write);
      close(fileno(stdin)); // close stdin
      dup(p_read); // replace 1 with the pipe out
      execute(right);
      _Exit(0);
    }
  }
}


void
execute(vector* v)
{
  if (vector_contains(v, ";")) {
   int loc = index_of(v, ";");
   vector* left = subvector(v, 0, loc);
   execute(left);
   vector* right = subvector(v, loc + 1, v->size);
   execute(right);
   free_vector(left);
   free_vector(right);
  } else if (vector_contains(v, "|")) {
     execute_pipe(v);
  } else if (vector_contains(v, "||")) {
    execute_or(v);
  } else if (vector_contains(v, "&&")) {
    execute_and(v);
  } else if (vector_contains(v, "&") ) {
    execute_background(v);
  } else if (vector_contains(v, "<")) {
    execute_redirect_in(v);
  } else if (vector_contains(v, ">")) {
    execute_redirect_out(v);
  } else {
    execute_command(v);
  }
}


int
main(int argc, char* argv[])
{
  char cmd[256];
  vector* v;
  if (argc == 1) {
    while (1) {
      printf("nush$ ");
      fflush(stdout);
      char* line = fgets(cmd, 256, stdin);
      if (!line) {
        printf("\n");
        break;
      }

      v = tokenize(cmd);

      execute(v);
      free_vector(v);
    }
  }
  else {
    FILE* file = file = fopen(argv[1], "r");
    while (fgets(cmd, 256, file)) {
      v = tokenize(cmd);
      execute(v);
      free_vector(v);
    }
    fclose(file);

  }

  return 0;
}
