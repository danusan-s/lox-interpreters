#include "value.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    case VAL_OBJ:
      printObject(value);
      break;
  }
}

bool valuesEqual(Value a, Value b) {
  if (a.type != b.type)
    return false;
  switch (a.type) {
    case VAL_BOOL:
      return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:
      return true; // Both are nil
    case VAL_NUMBER:
      return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_OBJ:
      // For simplicity, only compare string objects here
      if (isObjType(a, OBJ_STRING) && isObjType(b, OBJ_STRING)) {
        ObjString *strA = AS_STRING(a);
        ObjString *strB = AS_STRING(b);
        return strA->length == strB->length &&
               memcmp(strA->chars, strB->chars, strA->length) == 0;
      }
      return AS_OBJ(a) == AS_OBJ(b); // Fallback to pointer comparison
    default:
      return false; // Unsupported types
  }
}
