//The test driver 
//A test to check a simple arithmetic expression: -(1.2 + 3.4) / 5.6
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
    initVM();
    
    Chunk chunk;
    initChunk(&chunk);
    
    // Build bytecode for: -(1.2 + 3.4) / 5.6
    
    // Push constant 1.2
    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    
    // Push constant 3.4
    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    
    // Add: 1.2 + 3.4 = 4.6
    writeChunk(&chunk, OP_ADD, 123);
    
    // Push constant 5.6
    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    
    // Divide: 4.6 / 5.6 = 0.821...
    writeChunk(&chunk, OP_DIVIDE, 123);
    
    // Negate: -0.821...
    writeChunk(&chunk, OP_NEGATE, 123);
    
    // Return
    writeChunk(&chunk, OP_RETURN, 123);
    
    // Disassemble before running
    disassembleChunk(&chunk, "test chunk");
    
    // Execute!
    interpret(&chunk);
    
    freeVM();
    freeChunk(&chunk);
    return 0;
}