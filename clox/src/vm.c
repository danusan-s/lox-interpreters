#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "value.h"
#include <stdio.h>

VM vm;

static void resetStack() { vm.stackTop = vm.stack; }

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}

void initVM() { resetStack(); }

void freeVM() {}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
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
    }
  }
#undef READ_BYTE
}

InterpretResult interpret(Chunk *chunk) {
  vm.chunk = chunk;
  vm.ip = chunk->code;
  return run();
}
