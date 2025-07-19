#include "chunk.h"
#include <stdio.h>
#include <stdlib.h>

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
}

void writeChunk(Chunk *chunk, uint8_t byte) {
  if (chunk->count + 1 > chunk->capacity) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = oldCapacity < 8 ? 8 : oldCapacity * 2;
    chunk->code = realloc(chunk->code, sizeof(uint8_t) * chunk->capacity);
    if (chunk->code == NULL) {
      printf("Memory allocation failed\n");
      exit(EXIT_FAILURE);
    }
  }
  chunk->code[chunk->count] = byte;
  chunk->count++;
}

void freeChunk(Chunk *chunk) {
  free(chunk->code);
  initChunk(chunk);
}
