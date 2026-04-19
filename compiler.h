//This is the compiler which actually allows us to convert the Lox source code into bytecode
//We use a single pass compiler with Pratt parring for the expressions
#ifndef clox_compiler_h
#define clox_compiler_h

#include "chunk.h"

//Compiling Lox source code into a chunk of bytecode which consist of
//source = The lox source code  
//chunk =  the chunk we are writing the byte code into

bool compile(const char* source, Chunk* chunk);

#endif