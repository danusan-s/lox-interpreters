#include "chunk.h"
#include "debug.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
  Chunk chunk;
  initChunk(&chunk);
  printf("Chunk initialized.\n");

  for (int i = 0; i < 300; i++) {
    writeConstant(&chunk, i, 123);
  }
  printf("%dn", chunk.lcount);
  writeChunk(&chunk, OP_RETURN, 123);

  printf("Dissassembling chunk...\n");
  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);

  return 0;
}
