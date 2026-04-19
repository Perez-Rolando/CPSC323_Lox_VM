//The actual Virtual Machine Implementation
#include <cstdio>
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

VM vm;

static void resetStack() {  //reseting the stack to empty 
    vm.stackTop = vm.stack; //stackTop will point to where the the next value will go
}

void initVM(){
    resetStack();
}

void freeVM() {

}

void push(Value value){
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}
/**
*The execution loop of the VM which is the heart of the actual VM
*The "fetch-decode-execute" cycle which consist of:
* 1. FETCH-   Read the next byte of bytecode
* 2. DECODE-  Figure out what instruction it is
* 3. EXECUTE- Perform the operation
*/

static InterpretResult run() {
    // Macros
    
    // READ_BYTE: Fetch the next byte from the chunk and advance IP
    #define READ_BYTE() (*vm.ip++)
    
    // READ_CONSTANT: Read a constant from the constant pool
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    
    // BINARY_OP: Pop two operands, apply operator, push result
    #define BINARY_OP(op) \
        do { \
            double b = pop(); \
            double a = pop(); \
            push(a op b); \
        } while (false)

    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
            // Print the stack contents for debugging
            printf("          ");
            for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            
            // Disassemble the current instruction
            disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif
        
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_NEGATE:   push(-pop()); break;
            
            case OP_RETURN: {
                //print the top value and exit
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
    
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    Chunk chunk;
    initChunk(&chunk);
    
    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }
    
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;
    
    InterpretResult result = run();
    
    freeChunk(&chunk);
    return result;
}