#include "chunk.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

static void repl() {
  char line[1024];
  while (1) {
    printf(">> ");
    if (!fgets(line, sizeof(line), stdin)) {
      break; // Exit on EOF
    }
    if (line[0] == '\n')
      continue; // Skip empty lines

    InterpretResult result = interpret(line);
  }
}

static char *readFile(const char *path) {
  FILE *file = fopen(path, "rb");
  if (!file) {
    fprintf(stderr, "Could not open file %s\n", path);
    exit(74);
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    fprintf(stderr, "Could not allocate memory for file %s\n", path);
    exit(74);
  }

  size_t bytesRead = fread(buffer, sizeof(char), size, file);
  buffer[bytesRead] = '\0';
  fclose(file);

  return buffer;
}

static void runFile(const char *path) {
  char *file = readFile(path);
  InterpretResult result = interpret(file);
  free(file);

  if (result == INTERPRET_COMPILE_ERROR) {
    fprintf(stderr, "Compile error.\n");
    exit(65);
  } else if (result == INTERPRET_RUNTIME_ERROR) {
    fprintf(stderr, "Runtime error.\n");
    exit(70);
  }
}

int main(int argc, const char *argv[]) {
  initVM();

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: %s [script]\n", argv[0]);
    return 1;
  }

  freeVM();
  return 0;
}
