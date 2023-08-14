#ifndef INPUT_H
#define INPUT_H

#include <stdlib.h>
#include <sys/types.h>

// InputBuffer represents the state we need to store to interact with getline()
typedef struct {
  char *buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

// Create a new InputBuffer
InputBuffer *input_new_buffer(void);

// Print a prompt
void input_prompt(char *text);

// Read a line of input
int input_read(InputBuffer *input_buffer);

// Close and free an InputBuffer
void input_close_buffer(InputBuffer *input_buffer);

#endif
