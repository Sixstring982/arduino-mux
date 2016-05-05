#ifndef SRC_INC_CONSOLE_H
#define SRC_INC_CONSOLE_H

#include <stdio.h>

#include "./Types.h"

extern u32 console_read_line(FILE* stream, char* buffer, u32 length);

#endif  /* SRC_INC_CONSOLE_H */
