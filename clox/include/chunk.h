#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
  OP_CONSTANT_LONG,
  OP_NEGATE,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE
} OpCode;

typedef struct {
  int count;
  int capacity;
  uint8_t *code;
  int lcount;
  int lcapacity;
  int *lines;
  ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
void writeConstant(Chunk *chunk, Value value, int line);
int addConstant(Chunk *chunk, Value value);
int getLine(const Chunk *chunk, int offset);
void freeChunk(Chunk *chunk);

#endif // CLOX_CHUNK_H
