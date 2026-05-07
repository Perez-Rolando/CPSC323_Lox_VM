//Single-pass compiler using Pratt parsing (precedence climbing).
#include <cstdio>
#include <cstdlib>
#include "common.h"
#include "compiler.h"
#include "object.h" 
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

//Parser state which tracks current and previous tokens
typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

//Precedence levels (low to hi)
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;

//Function pointer type for parse functions
typedef void (*ParseFn)(bool canAssign);

//ParseRule - Defines how to parse each token type
typedef struct {
    ParseFn prefix;        //token is in prefix position
    ParseFn infix;         //token is in infix position
    Precedence precedence; // Precedence of infix operator
} ParseRule;

// Global parser and compiler state
Parser parser;
Chunk* compilingChunk;

// Forward declarations
static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);
static uint8_t identifierConstant(Token* name);
static uint8_t parseVariable(const char* errorMessage);
static void defineVariable(uint8_t global);
static bool match(TokenType type);
static bool check(TokenType type);
static void synchronize();

//Get the chunk currently being compiled
static Chunk* currentChunk() {
    return compilingChunk;
}

//Error reporting
static void errorAt(Token* token, const char* message) {
    if (parser.panicMode) return;
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);
    
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }
    
    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void error(const char* message) {
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}

//Moving on to the next token
static void advance() {
    parser.previous = parser.current;
    
    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;
        
        errorAtCurrent(parser.current.start);
    }
}

//Consuming a token of the expected type
static void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }
    
    errorAtCurrent(message);
}

//writing bytes to the chunk
static void emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn() {
    emitByte(OP_RETURN);
}

//Adding a constant to the constant pool
static uint8_t makeConstant(Value value) {
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    
    return (uint8_t)constant;
}

static void emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static uint8_t identifierConstant(Token* name) {
    return makeConstant(OBJ_VAL(copyString(name->start, name->length)));
}

//THE END compilation
static void endCompiler() {
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

//Parsing a binary operator
static void binary(bool canAssign) {
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));
    
    switch (operatorType) {
        case TOKEN_PLUS:          emitByte(OP_ADD); break;
        case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
        default: return; // Unreachable
    }
}

//Parsing a grouped expression
static void grouping(bool canAssign) {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

//Parsing a number literal
static void number(bool canAssign) {
    double value = strtod(parser.previous.start, nullptr);
    emitConstant(NUMBER_VAL(value));
}

//Parsing a literal (true, false, nil)
static void literal(bool canAssign) {
    switch (parser.previous.type) {
        case TOKEN_FALSE: emitByte(OP_FALSE); break;
        case TOKEN_NIL:   emitByte(OP_NIL); break;
        case TOKEN_TRUE:  emitByte(OP_TRUE); break;
        default: return; // Unreachable
    }
}

//Parsing a Unary operator
static void unary(bool canAssign) {
    TokenType operatorType = parser.previous.type;
    
    // Compile the operand
    parsePrecedence(PREC_UNARY);
    
    // Emit the operator instruction
    switch (operatorType) {
        case TOKEN_BANG:  emitByte(OP_NOT); break;
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default: return; // Unreachable
    }
}

//Parse a string literal
static void string(bool canAssign) {
    // +1 and -2 to trim the leading and trailing quotes
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1,
                                    parser.previous.length - 2)));
}

//Parse a variable declaration
static void varDeclaration() {
    uint8_t global = parseVariable("Expect variable name.");
    
    if (match(TOKEN_EQUAL)) {
        expression();
    } else {
        emitByte(OP_NIL);
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    
    defineVariable(global);
}

//Parse a variable name
static uint8_t parseVariable(const char* errorMessage) {
    consume(TOKEN_IDENTIFIER, errorMessage);
    return identifierConstant(&parser.previous);
}

//Define a global variable
static void defineVariable(uint8_t global) {
    emitBytes(OP_DEFINE_GLOBAL, global);
}

//Parse an expression statement
static void expressionStatement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

//Parse a print statement
static void printStatement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

//Parse a statement
static void statement() {
    if (match(TOKEN_PRINT)) {
        printStatement();
    } else {
        expressionStatement();
    }
}

//Parse a declaration
static void declaration() {
    if (match(TOKEN_VAR)) {
        varDeclaration();
    } else {
        statement();
    }
    
    if (parser.panicMode) synchronize();
}

//Synchronize after a parse error
static void synchronize() {
    parser.panicMode = false;
    
    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON) return;
        switch (parser.current.type) {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;
            default:
                ; // Do nothing
        }
        
        advance();
    }
}

//Check if current token matches any of the given types
static bool match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

//Check if current token is of given type
static bool check(TokenType type) {
    return parser.current.type == type;
}

//Parse a variable reference or assignment
static void namedVariable(Token name, bool canAssign) {
    uint8_t arg = identifierConstant(&name);
    
    if (canAssign && match(TOKEN_EQUAL)) {
        expression();
        emitBytes(OP_SET_GLOBAL, arg);
    } else {
        emitBytes(OP_GET_GLOBAL, arg);
    }
}

//Parse an identifier (variable reference)
static void variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

//The Parse table maps token types to parse functions and is used for Pratt Parsing
ParseRule rules[] = {
    [TOKEN_LEFT_PAREN]    = {grouping, nullptr,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_COMMA]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_DOT]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_MINUS]         = {unary,    binary,   PREC_TERM},
    [TOKEN_PLUS]          = {nullptr,     binary,   PREC_TERM},
    [TOKEN_SEMICOLON]     = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_SLASH]         = {nullptr,     binary,   PREC_FACTOR},
    [TOKEN_STAR]          = {nullptr,     binary,   PREC_FACTOR},
    [TOKEN_BANG]          = {unary,    nullptr,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_EQUAL]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_GREATER]       = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_LESS]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_LESS_EQUAL]    = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_IDENTIFIER]    = {variable, nullptr,   PREC_NONE},
    [TOKEN_STRING]        = {string,  nullptr,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,   nullptr,   PREC_NONE},
    [TOKEN_AND]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_CLASS]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_ELSE]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_FALSE]         = {literal,  nullptr,   PREC_NONE},
    [TOKEN_FOR]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_FUN]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_IF]            = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_NIL]           = {literal,  nullptr,   PREC_NONE},
    [TOKEN_OR]            = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_PRINT]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_RETURN]        = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_SUPER]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_THIS]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_TRUE]          = {literal,  nullptr,   PREC_NONE},
    [TOKEN_VAR]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_WHILE]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_ERROR]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_EOF]           = {nullptr,     nullptr,   PREC_NONE},
};

//Parse an expression at the given precedence level (Pratt parsing)
static void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }
    
    bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(canAssign);
    
    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(false);
    }
    
    if (canAssign && match(TOKEN_EQUAL)) {
        error("Invalid assignment target.");
    }
}

//Getting the parse rule for a token type
static ParseRule* getRule(TokenType type) {
    return &rules[type];
}

//Parsing an expression
static void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

//Main compile function
bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = chunk;
    
    parser.hadError = false;
    parser.panicMode = false;
    
    advance();
    
    while (!match(TOKEN_EOF)) {
        declaration();
    }
    
    endCompiler();
    return !parser.hadError;
}