#ifndef CLOX_OBJECT_H
#define CLOX_OBJECT_H

#include "chunk.h"
#include "common.h"
#include "value.h"

// get pointer to obj allocated on the heap and get type through pointer
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
// Get the object pointer and cast to ObjString pointer which will then read
// that heap location as an ObjString
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
  OBJ_STRING,
  OBJ_FUNCTION,
} ObjType;

// Object base struct which other objects will inherit/extend
// Place as first member of other object structs which will allow those objects
// to be casted to Obj* and access type
struct Obj {
  ObjType type;
  struct Obj *next;
};

struct ObjString {
  Obj obj;
  int length;
  char *chars;
  uint32_t hash;
};

typedef struct {
  Obj obj;
  int arity;
  Chunk chunk;
  ObjString *name;
} ObjFunction;

// use function instead of putting directly in the macro
// this is because value is being used twice in the macro
// let's say we pass in a function call that returns a Value into the value
// field of the macro then the function would be called twice which is not what
// we want
static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ObjString *copyString(const char *chars, int length);
ObjString *takeString(char *chars, int length);

ObjFunction *newFunction();

void printObject(Value value);

#endif // CLOX_OBJECT_H
