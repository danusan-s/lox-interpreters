#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.h"
#include "value.h"

typedef struct {
  Chunk *chunk;
  uint8_t *ip;
  int stackCapacity;
  int stackTop;
  Value *stack;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk *chunk);
void push(Value value);
Value pop();

#endif // CLOX_VM_H
