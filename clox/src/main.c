#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
  initVM();

  Chunk chunk;
  initChunk(&chunk);
  printf("Chunk initialized.\n");

  for (int i = 0; i < 300; i++) {
    writeConstant(&chunk, i, 123);
  }
  writeChunk(&chunk, OP_RETURN, 123);

  interpret(&chunk);

  freeVM();
  freeChunk(&chunk);

  return 0;
}
