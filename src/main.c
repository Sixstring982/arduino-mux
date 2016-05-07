#include <stdio.h>
#include <unistd.h>
#include "./inc/Arguments.h"
#include "./inc/arduino-serial-lib.h"

#define STRING(X) #X
#define STRINGIFY(X) STRING(X)

static bool send_arduino_packet(int fd, u8 size) {
  int readC;
  u32 i;
  uint8_t out_byte;
  printf("Sending %u bytes to arduino at fd=%d...\n", size, fd);
  for (i = 0; i < size; i++) {
    readC = getchar();
    if (readC == EOF) {
      printf("EOF detected. Exiting...\n");
      return false;
    }

    out_byte = (uint8_t)readC;
    printf("[0x%02x]", out_byte);
    if ((i + 1) % 8 == 0) {
      printf("\n");
      serialport_flush(fd);
    }
    serialport_writebyte(fd, out_byte);
  }
  printf("\n");
  serialport_flush(fd);
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
  u32 i;
  if (!arguments_init(&args, argc, argv)) {
    return 1;
  }

  arguments_print_status(&args, stdout);

  printf("Sleeping 5 seconds for serial warmup");
  fflush(stdout);
  for (i = 0; i < 5; i++) {
    sleep(1);
    printf(".");
    fflush(stdout);
  }
  printf("\nGood to go.\n");

  while(running) {
    if (!process_next_input(&args)) {
      running = false;
    }
  }

  arguments_destroy(&args);
  return 0;
}
