#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "chunk.h"

void disassembleChunk(const Chunk *chunk, const char *name);
int disassembleInstruction(const Chunk *chunk, int offset);
int simpleInstruction(const char *name, int offset);

#endif // CLOX_DEBUG_H
