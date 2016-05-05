#include "./inc/Console.h"

extern u32 console_read_line(FILE* stream, char* buffer, u32 length) {
  u32 i = 0;
  int readC;
  while (i < length - 1) {
    readC = fgetc(stream);
    if (readC == '\n' || readC == EOF) {
      break;
    }
    buffer[i++] = (char)readC;
  }

  buffer[i] = '\0';
  return i;
}
