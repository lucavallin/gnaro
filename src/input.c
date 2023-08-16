#include "input.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// Create a new input buffer
InputBuffer *input_new_buffer(void) {
  log_debug("creating new input buffer...");
  InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

// Print a prompt to the user
void input_prompt(char *text) { printf("%s > ", text); }

// Read input from stdin
int input_read(InputBuffer *input_buffer) {
  log_debug("reading input...");
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    log_debug("error reading input with getline()");
    return -1;
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
  return 0;
}

// Close an input buffer
void input_close_buffer(InputBuffer *input_buffer) {
  log_debug("closing input buffer...");
  free(input_buffer->buffer);
  free(input_buffer);
}
