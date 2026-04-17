//Value = Runtime value rep
//as of now the value are just numbers
#ifndef clox_value_h
#define clox_value_h

#include "common.h"

//Value = A Lox Value 
//Only supports doubles
typedef double Value;

//ValueArray = The dynamic array of Values in other words the Constant Pool
typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);               //initialize a ValueArray
void writeValueArray(ValueArray* array, Value value); //append a value to the Array
void freeValueArray(ValueArray* array);               //free the memory used by the Value Array
void printValue(Value value);                         //print a value to stdout

#endif