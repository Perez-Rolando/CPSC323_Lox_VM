//The DISASSEMBLER!!!
//Turns the byte code into a language fit for HUMANS
//Used for the actual debugging and proving that the implementation is a VM
#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"

//Disassemble ALL instructions within a chunk which contains:
// chunk - The chunk to disassemble
// name  - A name to print as the header
void disassembleChunk(Chunk* chunk, const char* name);

//Disassemble a SINGLE instruction within a chunk which contains:
// chunk  - the chunk containing the instruction
// offset - the bytr offset of the instruction
// return the offset of the next instruction
int disassembleInstruction(Chunk* chunk, int offset);

#endif