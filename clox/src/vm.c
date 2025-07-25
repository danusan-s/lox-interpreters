#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

VM vm;

static void resetStack() {
  vm.stack = NULL;
  vm.stackTop = 0;
  vm.stackCapacity = 0;
}

static void runtimeError(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = vm.chunk->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}

static Value peek(int distance) {
  return vm.stack[vm.stackTop - 1 - distance];
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

void initVM() {
  resetStack();
}

void freeVM() {
  free(vm.stack);
  resetStack();
  vm.chunk = NULL;
  vm.ip = NULL;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define BINARY_OP(valueType, op)                                               \
  do {                                                                         \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {                          \
      runtimeError("Operands must be numbers.");                               \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(pop());                                               \
    double a = AS_NUMBER(pop());                                               \
    push(valueType(a op b));                                                   \
  } while (false)

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

        break;
      }
      case OP_CONSTANT_LONG: {
        int constant_ind =
            READ_BYTE() | (READ_BYTE() << 8) | (READ_BYTE() << 16);
        Value constant = vm.chunk->constants.values[constant_ind];
        push(constant);

        break;
      }
      case OP_NEGATE: {
        if (!IS_NUMBER(vm.stack[vm.stackTop - 1])) {
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        push(NUMBER_VAL(-AS_NUMBER(pop())));
        break;
      }
      case OP_ADD: {
        BINARY_OP(NUMBER_VAL, +);
        break;
      }
      case OP_SUBTRACT: {
        BINARY_OP(NUMBER_VAL, -);
        break;
      }
      case OP_MULTIPLY: {
        BINARY_OP(NUMBER_VAL, *);
        break;
      }
      case OP_DIVIDE: {
        BINARY_OP(NUMBER_VAL, /);
        break;
      }
      case OP_NIL:
        push(NIL_VAL);
        break;
      case OP_TRUE:
        push(BOOL_VAL(true));
        break;
      case OP_FALSE:
        push(BOOL_VAL(false));
        break;
    }
  }
#undef READ_BYTE
#undef BINARY_OP
}

InterpretResult interpret(char *source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  resetStack();

  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}
