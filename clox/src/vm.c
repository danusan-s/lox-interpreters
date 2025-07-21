#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

VM vm;

static void resetStack() {
  vm.stack = NULL;
  vm.stackTop = 0;
  vm.stackCapacity = 0;
}

void push(Value value) {
  if (vm.stackTop + 1 > vm.stackCapacity) {
    int oldCapacity = vm.stackCapacity;
    vm.stackCapacity = oldCapacity < 256 ? 256 : oldCapacity * 2;
    vm.stack = realloc(vm.stack, sizeof(Value) * vm.stackCapacity);
    if (vm.stack == NULL) {
      fprintf(stderr, "Memory allocation failed for stack\n");
      exit(EXIT_FAILURE);
    }
  }
  vm.stack[vm.stackTop] = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return vm.stack[vm.stackTop];
}

void initVM() { resetStack(); }

void freeVM() {
  free(vm.stack);
  resetStack();
  vm.chunk = NULL;
  vm.ip = NULL;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define BINARY_OP(op)                                                          \
  double b = pop();                                                            \
  double a = pop();                                                            \
  push(a op b);

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (int slot = 0; slot < vm.stackTop; slot++) {
      printf("[ ");
      printValue(vm.stack[slot]);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_RETURN: {
        printf("<< ");
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      }
      case OP_CONSTANT: {
        int constant_ind = READ_BYTE();
        Value constant = vm.chunk->constants.values[constant_ind];
        push(constant);

        printf(">> ");
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_CONSTANT_LONG: {
        int constant_ind =
            READ_BYTE() | (READ_BYTE() << 8) | (READ_BYTE() << 16);
        Value constant = vm.chunk->constants.values[constant_ind];
        push(constant);

        printf(">> ");
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_NEGATE: {
        vm.stack[vm.stackTop - 1] *= -1;
        break;
      }
      case OP_ADD: {
        BINARY_OP(+);
        break;
      }
      case OP_SUBTRACT: {
        BINARY_OP(-);
        break;
      }
      case OP_MULTIPLY: {
        BINARY_OP(*);
        break;
      }
      case OP_DIVIDE: {
        BINARY_OP(/);
        break;
      }
    }
  }
#undef READ_BYTE
#undef BINARY_OP
}

InterpretResult interpret(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = chunk->code;
  return run();
}
