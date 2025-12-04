#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  vm.objects = NULL;
  initTable(&vm.globals);
  initTable(&vm.strings);
}

static void freeObject(Obj *object) {
  switch (object->type) {
    case OBJ_STRING: {
      ObjString *string = (ObjString *)object;
      free(string->chars);
      free(string);
      break;
    }
  }
}

void freeObjects() {
  Obj *object = vm.objects;
  while (object != NULL) {
    Obj *next = object->next;
    freeObject(object);
    object = next;
  }
}

void freeVM() {
  freeTable(&vm.globals);
  freeTable(&vm.strings);
  freeObjects();
}

static void concatenate() {
  ObjString *b = AS_STRING(pop());
  ObjString *a = AS_STRING(pop());

  int length = a->length + b->length;
  char *chars = (char *)malloc(sizeof(char) * (length + 1));
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString *result = takeString(chars, length);
  push(OBJ_VAL(result));
}

static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
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
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concatenate();
          break;
        }
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
      case OP_NOT: {
        Value value = pop();
        push(BOOL_VAL(isFalsey(value)));
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
      case OP_EQUAL: {
        Value b = pop();
        Value a = pop();
        push(BOOL_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_GREATER: {
        BINARY_OP(BOOL_VAL, >);
        break;
      }
      case OP_LESS: {
        BINARY_OP(BOOL_VAL, <);
        break;
      }
      case OP_PRINT: {
        printValue(pop());
        printf("\n");
        break;
      }
      case OP_POP: {
        pop();
        break;
      }
      case OP_DEFINE_GLOBAL: {
        ObjString *name = AS_STRING(pop());
        tableSet(&vm.globals, name, peek(0));
        pop();
        break;
      }
      case OP_SET_GLOBAL: {
        ObjString *name = AS_STRING(pop());
        if (tableSet(&vm.globals, name, peek(0))) {
          // Delete since we set without declaring
          tableDelete(&vm.globals, name);
          runtimeError("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        // don't pop since assignments evaluate to the assigned value
        break;
      }
      case OP_GET_GLOBAL: {
        ObjString *name = AS_STRING(pop());
        Value value;
        if (!tableGet(&vm.globals, name, &value)) {
          runtimeError("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        push(value);
        break;
      }
      case OP_GET_LOCAL: {
        uint8_t slot = READ_BYTE();
        push(vm.stack[slot]);
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        vm.stack[slot] = peek(0);
        // don't pop since assignments evaluate to the assigned value
        break;
      }
      case OP_JUMP_IF_FALSE: {
        uint16_t offset = (READ_BYTE() << 8);
        offset |= READ_BYTE();
        if (isFalsey(peek(0))) {
          vm.ip += offset;
        }
        break;
      }
      case OP_JUMP: {
        uint16_t offset = (READ_BYTE() << 8);
        offset |= READ_BYTE();
        vm.ip += offset;
        break;
      }
      case OP_LOOP: {
        uint16_t offset = (READ_BYTE() << 8);
        offset |= READ_BYTE();
        vm.ip -= offset;
        break;
      }
      case OP_RETURN: {
        return INTERPRET_OK;
      }
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
