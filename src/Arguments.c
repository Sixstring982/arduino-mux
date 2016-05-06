#include "./inc/Arguments.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "./inc/arduino-serial-lib.h"
#include "./inc/Console.h"

typedef enum {
  UNSPECIFIED,
  ARDUINOS
} ReadSection;

static void arguments_usage(char* progname) {
  printf("%s [configFilename]\n"
         " Arguments:\n"
         "  configFilename: String\n"
         "    The filename of the config file to open. See below\n"
         "    for config file structure.\n"
         "\n"
         " Config file structure:\n"
         "  Arduino Section: Starts with 'ARDUINOS'\n"
         "   then, once per line, is an ordered list of\n"
         "   arduino serial port names. The program will then\n"
         "   open them, and read from stdin sending bytes to each.\n"
         "\n"
         " Serial communication:\n"
         "  Once all Arduinos have been opened, bytes can begin to\n"
         "  be sent via stdin to this program. The protocol works\n"
         "  like this:\n"
         "\n"
         "  Offset - Meaning\n"
         "  0      - Arduino to receive packet.\n"
         "  1      - Size of packet in bytes.\n"
         "  2      - Packet. Size must be equal to byte at offset 1.\n"
         "\n"
         "  This program will run as long as EOF is not sent to"
         "stdin.\n"
         ,
         progname
    );
}

static bool open_arduino(Arguments* args,
                         char* portName) {
  int fd;
  if ((fd = serialport_init(portName, 9600)) == -1) {
    return false;
  }
  args->arduinoOutputs[args->arduinoCount++] = fd;
  return true;
}

static bool parse_config_file(Arguments* args,
                              const char* configFilename) {
  const u32 LINE_SIZE = 1024;
  FILE* fp = fopen(configFilename, "r");
  char line[LINE_SIZE];
  char* line_start;
  ReadSection section = UNSPECIFIED;
  bool noProblems = true;
  u32 line_num = 0;
  if (fp == NULL) {
    return false;
  }

  while (console_read_line(fp, line, LINE_SIZE) > 0) {
    line_num++;
    line_start = line;
    /* Let's skip preceding whitespace */
    while (isspace(*line_start)) {
      line_start++;
      /* Is this line just a bunch of spaces?
         who does that? */
      if ((line_start - line) >= LINE_SIZE) {
        continue;
      }
    }
    /* Let's ignore comments */
    if (line_start[0] == '#') {
      continue;
    }
    /* See if we're switching sections */
    if (!strcmp("ARDUINOS", line)) {
      section = ARDUINOS;
    } else {
      /* See which section we're in, act accordingly.*/
      switch (section) {
      case ARDUINOS:
        if (!open_arduino(args, line)) {
          fprintf(stderr,
                  "%s:%u:%td error: "
                  "Arduino at port <%s> could not be opened.\n",
                  configFilename, line_num,
                  line_start - line, line_start);
          noProblems = false;
        }
        break;
      default:
        fprintf(stderr,
                "%s:%u%td error: "
                "Unspecified config file section. Expected \n"
                "section header before line: <%s>.\n",
                  configFilename, line_num,
                  line_start - line, line_start);
        noProblems = false;
        break;
      }
    }
  }

  fclose(fp);
  return noProblems;
}

bool arguments_init(Arguments* args, int argc, char** argv) {
  const char* configFilename = NULL;
  args->arduinoCount = 0;
  args->arduinoOutputs = malloc(sizeof(int) * 255);

  if (argc < 2) {
    arguments_usage(argv[0]);
    return false;
  }

  configFilename = argv[1];
  return parse_config_file(args, configFilename);
}

void arguments_print_status(Arguments* args, FILE* fp) {
  fprintf(fp, "Connected to %u arduinos, ready for input.\n",
          args->arduinoCount
  );
}

void arguments_destroy(Arguments* args) {
  u32 i;
  for (i = 0; i < args->arduinoCount; i++) {
    serialport_close(args->arduinoOutputs[i]);
  }
  free(args->arduinoOutputs);
}
