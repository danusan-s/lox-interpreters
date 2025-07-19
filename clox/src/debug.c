#include "debug.h"
#include "value.h"
#include <stdio.h>

void disassembleChunk(const Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);
  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

static int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constantInstruction(const char *name, const Chunk *chunk,
                               int offset) {
  uint8_t constant_ind = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant_ind);
  printValue(chunk->constants.values[constant_ind]);
  printf("'\n");
  return offset + 2;
}

static int constantLongInstruction(const char *name, const Chunk *chunk,
                                   int offset) {
  uint32_t constant = ((uint32_t)chunk->code[offset + 1]) |
                      ((uint32_t)chunk->code[offset + 2] << 8) |
                      ((uint32_t)chunk->code[offset + 3] << 16);
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 4;
}

int disassembleInstruction(const Chunk *chunk, int offset) {
  printf("%04d ", offset);

  printf("%4d ", getLine(chunk, offset));

  if (offset >= chunk->count) {
    printf("ERROR: Offset out of bounds\n");
    return offset + 1;
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
    default:
      printf("UNKNOWN OPCODE %d\n", instruction);
      return offset + 1;
  }
}
