#ifndef SRC_INC_ARGUMENTS_H
#define SRC_INC_ARGUMENTS_H

#include <stdio.h>

#include "./Types.h"

typedef struct {
  u32 arduinoCount;
  int* arduinoOutputs;
} Arguments;

extern bool arguments_init(Arguments* args, int argc, char** argv);

extern void arguments_print_status(Arguments* args, FILE* fp);

extern void arguments_destroy(Arguments* args);

#endif  /* SRC_INC_ARGUMENTS_H */
