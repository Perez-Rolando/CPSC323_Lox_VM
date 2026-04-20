//Value is now a runtime representation
//Now supports multiple types: numbers, booleans, and nil
//Uses a tagged union for efficient memory usage
#ifndef clox_value_h
#define clox_value_h

#include "common.h"

//The type of a value

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
} ValueType;

//tagged union representing any Lox value 
//Contains:
//type- The type of the value
//as  - Union of all possible value types
typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
} Value;

//Type checking macros
#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)

//Type conversion macros - Wrapping
#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)

//Value construction macros - Unwrapping
#define BOOL_VAL(value)   ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL           ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

///dynamic array of values (the constant pool)
typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

//Check if two values are equal
bool valuesEqual(Value a, Value b);

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif