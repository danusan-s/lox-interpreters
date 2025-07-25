#include "value.h"
#include <stdio.h>
#include <stdlib.h>

void initValueArray(ValueArray *array) {
  array->count = 0;
  array->capacity = 0;
  array->values = NULL;
}

void writeValueArray(ValueArray *array, Value value) {
  if (array->count + 1 > array->capacity) {
    int oldCapacity = array->capacity;
    array->capacity = array->capacity < 8 ? 8 : array->capacity * 2;
    array->values = realloc(array->values, sizeof(Value) * array->capacity);
  }
  array->values[array->count] = value;
  array->count++;
}

void freeValueArray(ValueArray *array) {
  free(array->values);
  initValueArray(array);
}

void printValue(Value value) {
  switch (value.type) {
    case VAL_BOOL:
      printf(AS_BOOL(value) ? "true" : "false");
      break;
    case VAL_NIL:
      printf("nil");
      break;
    case VAL_NUMBER:
      printf("%g", AS_NUMBER(value));
      break;
  }
}
