#include <stdio.h>
#include "./inc/Arguments.h"
#include "./inc/arduino-serial-lib.h"

#define STRING(X) #X
#define STRINGIFY(X) STRING(X)

static bool send_arduino_packet(int fd, u8 size) {
  int readC;
  u32 i;
  uint8_t out_byte;
  for (i = 0; i < size; i++) {
    readC = getchar();
    if (readC == EOF) {
      printf("EOF detected. Exiting...\n");
      return false;
    }

    out_byte = (uint8_t)readC;
    serialport_writebyte(fd, out_byte);
  }
  return true;
}

static bool process_with_arduino(Arguments* args, u8 arduino) {
  int readC = getchar();
  u8 packet_size;
  if (readC == EOF) {
    printf("EOF detected. Exiting...\n");
    return false;
  }

  if (arduino >= args->arduinoCount) {
    fprintf(stderr,
            "Arduino index %u is out of bounds (>= %u). Exiting...\n",
            arduino, args->arduinoCount);
    return false;
  }

  packet_size = (u8)readC;
  return send_arduino_packet(args->arduinoOutputs[arduino], packet_size);
}

static bool process_next_input(Arguments* args) {
  int readC = getchar();
  u8 arduino_index;
  if (readC == EOF) {
    printf("EOF detected. Exiting...\n");
    return false;
  }

  arduino_index = (u8)readC;

  return process_with_arduino(args, arduino_index);
}

int main(int argc, char** argv) {
  Arguments args;
  bool running = true;
  if(!arguments_init(&args, argc, argv)) {
    return 1;
  }

  arguments_print_status(&args, stdout);

  while(running) {
    if (!process_next_input(&args)) {
      running = false;
    }
  }

  arguments_destroy(&args);
  return 0;
}
