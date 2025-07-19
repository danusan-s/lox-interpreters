#include "debug.h"
#include <stdio.h>

void disassembleChunk(const Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);
  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

int disassembleInstruction(const Chunk *chunk, int offset) {
  printf("%04d ", offset);

  if (offset >= chunk->count) {
    printf("ERROR: Offset out of bounds\n");
    return offset + 1;
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  default:
    printf("UNKNOWN OPCODE %d\n", instruction);
    return offset + 1;
  }
}

int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}
