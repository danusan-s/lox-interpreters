#include "chunk.h"
#include "vm.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
  initVM();

  Chunk chunk;
  initChunk(&chunk);
  printf("Chunk initialized.\n");

  writeConstant(&chunk, 1.2, 123);
  writeChunk(&chunk, OP_NEGATE, 123);

  writeConstant(&chunk, 3.4, 123);

  writeChunk(&chunk, OP_ADD, 123);

  writeConstant(&chunk, 5.6, 123);

  writeChunk(&chunk, OP_DIVIDE, 123);

  for (int i = 0; i < 300; i++) {
    writeConstant(&chunk, i, 300);
  }

  writeChunk(&chunk, OP_RETURN, 123);

  interpret(&chunk);

  freeVM();
  freeChunk(&chunk);

  return 0;
}
