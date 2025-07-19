#include "chunk.h"
#include <stdio.h>
#include <stdlib.h>

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
  if (chunk->count + 1 > chunk->capacity) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = oldCapacity < 8 ? 8 : oldCapacity * 2;
    chunk->code = realloc(chunk->code, sizeof(uint8_t) * chunk->capacity);
    if (chunk->code == NULL) {
      printf("Memory allocation failed\n");
      exit(EXIT_FAILURE);
    }
    chunk->lines = realloc(chunk->lines, sizeof(int) * chunk->capacity);
    if (chunk->lines == NULL) {
      printf("Memory allocation failed\n");
      exit(EXIT_FAILURE);
    }
  }
  if (chunk->lcount + 3 > chunk->lcapacity) {
    int oldLCapacity = chunk->lcapacity;
    chunk->lcapacity = oldLCapacity < 8 ? 8 : oldLCapacity * 2;
    chunk->lines = realloc(chunk->lines, sizeof(int) * chunk->lcapacity);
    if (chunk->lines == NULL) {
      printf("Memory allocation failed\n");
      exit(EXIT_FAILURE);
    }
  }
  if (chunk->lcount == 0) {
    chunk->lines[chunk->lcount] = line;
    chunk->lcount++;
    chunk->lines[chunk->lcount] = 1;
  } else if (chunk->lines[chunk->lcount - 1] != line) {
    chunk->lcount++;
    chunk->lines[chunk->lcount] = line;
    chunk->lcount++;
    chunk->lines[chunk->lcount] = 1;
  } else {
    chunk->lines[chunk->lcount]++;
  }
  chunk->code[chunk->count] = byte;
  chunk->count++;
}

int addConstant(Chunk *chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

void writeConstant(Chunk *chunk, Value value, int line) {
  int constantIndex = addConstant(chunk, value);
  if (constantIndex < 256) {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, (uint8_t)constantIndex, line);
  } else {
    writeChunk(chunk, OP_CONSTANT_LONG, line);
    writeChunk(chunk, (uint8_t)(constantIndex & 0xFF), line);
    writeChunk(chunk, (uint8_t)((constantIndex >> 8) & 0xFF), line);
    writeChunk(chunk, (uint8_t)((constantIndex >> 16) & 0xFF), line);
  }
}

int getLine(const Chunk *chunk, int offset) {
  if (offset < 0 || offset >= chunk->count) {
    printf("ERROR: Offset out of bounds\n");
    return -1; // Error value
  }
  for (int i = 0; i < chunk->lcount; i += 2) {
    offset -= chunk->lines[i + 1];
    if (offset < 0) {
      return chunk->lines[i];
    }
  }
  return -1; // Error value if no line found
}

void freeChunk(Chunk *chunk) {
  free(chunk->code);
  free(chunk->lines);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}
