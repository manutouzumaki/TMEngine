#include "tm_json.h"
#include "tm_darray.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <string.h>

// TODO(manuto): try to create the tokenizer but with out allocating memory for the string
// use pointer to the file instead ...

enum JsonTokenType {
    TM_LEFT_PAREN, TM_RIGHT_PAREN,
    TM_LEFT_BRACE, TM_RIGHT_BRACE,
    TM_LEFT_BRACK, TM_RIGHT_BRACK,
    TM_COMMA, 
    TM_COLOM, 
    TM_SPECIAL,
    TM_STRING,
    TM_NUMBER,
    TM_EOF
};

const char *JsonTokenTypeStrings[] = {
    "TM_LEFT_PAREN", "TM_RIGHT_PAREN",
    "TM_LEFT_BRACE", "TM_RIGHT_BRACE",
    "TM_LEFT_BRACK", "TM_RIGHT_BRACK",
    "TM_COMMA", 
    "TM_COLOM", 
    "TM_SPECIAL",
    "TM_STRING",
    "TM_NUMBER",
    "TM_EOF"
};

struct JsonToken {
    JsonTokenType type;
    const char *literal;
    size_t literalSize;
    int line;
};

struct JsonScanner {
    const char *source;
    JsonToken *tokens;
    int tokensCount;
    int start;
    int current;
    int line;
    int sourceCount;
};


static bool ScannerIsAtEnd(JsonScanner *scanner) {
    return scanner->current >= scanner->sourceCount;
}

static char ScannerAdvance(JsonScanner *scanner) {
    scanner->current++;
    return scanner->source[scanner->current - 1];
}

static char ScannerPeek(JsonScanner *scanner) {
    if(ScannerIsAtEnd(scanner)) return '\0';
    return scanner->source[scanner->current];
}

static char ScannerPeekNext(JsonScanner *scanner) {
    if((scanner->current + 1) >= scanner->sourceCount) return '\0';
    return scanner->source[scanner->current + 1];
}

static const char *ScannerSubstring(JsonScanner *scanner, int start, int end, size_t *outLen) {
    assert(start < scanner->sourceCount); 
    assert(end <= scanner->sourceCount);
    // get offset to the start and end location of the new string
    char *startPtr = (char *)scanner->source + start;
    char *endPtr = (char *)scanner->source + end;
    // calculate the substring lenght
    size_t stringLenght = endPtr - startPtr; 
    char *substring = startPtr;
    *outLen = stringLenght;
    return (const char *)substring;
}

static void ScannerAddToken(JsonScanner *scanner, JsonTokenType type) {
    
    size_t valueSize = 0;
    const char *value = ScannerSubstring(scanner, scanner->start, scanner->current, &valueSize);

    JsonToken token{};
    token.type = type;
    token.literal = value;
    token.literalSize = valueSize;
    token.line = scanner->line;
    TMDarrayPush(scanner->tokens, token, JsonToken);
    scanner->tokensCount++;
}

static void ScannerAddStringToken(JsonScanner *scanner) {
    while(ScannerPeek(scanner) != '"' && !ScannerIsAtEnd(scanner)) {
        if(ScannerPeek(scanner) == '\n') {
            scanner->line++;
        }
        ScannerAdvance(scanner);
    }
    if(ScannerIsAtEnd(scanner)) {
        printf("ERROR: Unterminated String.\n");
        return;
    }
    ScannerAdvance(scanner);
    size_t valueSize = 0;
    const char *value = ScannerSubstring(scanner, scanner->start + 1, scanner->current - 1, &valueSize);

    JsonToken token{};
    token.type = TM_STRING;
    token.literal = value;
    token.literalSize = valueSize;
    token.line = scanner->line; 
    TMDarrayPush(scanner->tokens, token, JsonToken);
    scanner->tokensCount++;
}

static bool ScannerIsDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool ScannerIsNegativeDigit(char c, char n) {
    return (c == '-' && ScannerIsDigit(n));
}

static void ScannerAddNumberToken(JsonScanner *scanner) {
    while(ScannerIsDigit(ScannerPeek(scanner))) {
        ScannerAdvance(scanner);
    }
    // Look for a fractional part.
    if(ScannerPeek(scanner) == '.' && ScannerIsDigit(ScannerPeekNext(scanner))) {
        ScannerAdvance(scanner);
        while(ScannerIsDigit(ScannerPeek(scanner))) ScannerAdvance(scanner);
    }
    size_t valueSize = 0;
    const char *value = ScannerSubstring(scanner, scanner->start, scanner->current, &valueSize);
    JsonToken token{};
    token.type = TM_NUMBER;
    token.literal = value;
    token.literalSize = valueSize;
    token.line = scanner->line;
    TMDarrayPush(scanner->tokens, token, JsonToken);
    scanner->tokensCount++;
}

static void ScanToken(JsonScanner *scanner) {
    char c = ScannerAdvance(scanner);
    switch(c) {
        case '(': ScannerAddToken(scanner, TM_LEFT_PAREN); break;
        case ')': ScannerAddToken(scanner, TM_RIGHT_PAREN); break;
        case '{': ScannerAddToken(scanner, TM_LEFT_BRACE); break;
        case '}': ScannerAddToken(scanner, TM_RIGHT_BRACE); break;
        case '[': ScannerAddToken(scanner, TM_LEFT_BRACK); break;
        case ']': ScannerAddToken(scanner, TM_RIGHT_BRACK); break;
        case ',': ScannerAddToken(scanner, TM_COMMA); break;
        case ':': ScannerAddToken(scanner, TM_COLOM); break;
        case ' ':
        case '\r':
        case '\t':
        // Ignore whitespace.
            break;
        case '\n':
            scanner->line++;
            if(scanner->line > 2124) {
                int stopHere = 0;
            };
            break;
        case '"': ScannerAddStringToken(scanner); break;
        default: {
            if(ScannerIsDigit(c)) {
                ScannerAddNumberToken(scanner);
            }
            else if(ScannerIsNegativeDigit(c, ScannerPeek(scanner))) {
                ScannerAddNumberToken(scanner);
            }
            else {
                printf("Unexpected character. ERROR at line %d\n", scanner->line);
            }
        } break;
   } 
}

static void ScannerPrintTokens(JsonScanner *scanner) {
    for(int i = 0; i < scanner->tokensCount; ++i) {
        JsonToken *token = scanner->tokens + i;
        char *tokenBuffer = (char *)malloc(token->literalSize + 1);
        memcpy((void *)tokenBuffer, (void *)token->literal, token->literalSize);
        tokenBuffer[token->literalSize] = '\0';
        printf("%s ""%s"" ", JsonTokenTypeStrings[token->type], tokenBuffer);
        printf("at line: %d\n", token->line);
        free(tokenBuffer);
    } 
}

static void ScanTokens(JsonScanner *scanner, const char *source, int sourceCount) {
    scanner->source = source;
    scanner->sourceCount = sourceCount;
    scanner->line = 1;

    while(!ScannerIsAtEnd(scanner)) {
        scanner->start = scanner->current;
        ScanToken(scanner);
    }
    ScannerAddToken(scanner, TM_EOF);

    //ScannerPrintTokens(scanner);
}

static JsonToken *ParserTokens(TMJsonObject *parent, JsonToken *token);

static JsonToken *ParserArray(TMJsonObject *object, JsonToken *token) {
    for(;;) {
        JsonToken *value = token;
        if(value->type == TM_STRING || value->type == TM_NUMBER){
            TMJsonValue jsonValue{};
            TMDarrayPush(object->values, jsonValue, TMJsonValue);
            object->values[object->valuesCount].value = value->literal;
            object->values[object->valuesCount].size = value->literalSize;
            object->valuesCount++;
            object->type = TM_JSON_VALUE;
        }
        if(token->type == TM_LEFT_BRACE) {
            TMJsonObject jsonObject{};
            TMDarrayPush(object->objects, jsonObject, TMJsonObject);
            token = ParserTokens(&object->objects[object->valuesCount], token + 1);
            object->valuesCount++;
            object->type = TM_JSON_OBJECT;
        }
        if(token->type == TM_RIGHT_BRACK) {
            return token + 1;
        }
        token += 1;
    }
}

// Sorry ;)
static JsonToken *ParserTokens(TMJsonObject *parent, JsonToken *token) {
    for(;;) {
        if(token->type ==  TM_COLOM) {
            JsonToken *name = token - 1; 
            TMJsonObject jsonObject{};
            TMDarrayPush(parent->childs, jsonObject, TMJsonObject);
            parent->childs[parent->childsCount].name = name->literal;
            parent->childs[parent->childsCount].nameSize = name->literalSize;
            JsonToken *value = token + 1;
            if(value->type == TM_LEFT_BRACE) {
               token = ParserTokens(&parent->childs[parent->childsCount], token + 1);
            }
            if(value->type == TM_LEFT_BRACK) {
                token = ParserArray(&parent->childs[parent->childsCount], token + 1);
            }
            if(value->type == TM_STRING || value->type == TM_NUMBER) {
                TMJsonObject *child = &parent->childs[parent->childsCount];
                TMJsonValue jsonValue{};
                TMDarrayPush(child->values, jsonValue, TMJsonValue);
                child->values[child->valuesCount].value = value->literal;
                child->values[child->valuesCount].size = value->literalSize;
                child->valuesCount++;
                child->type = TM_JSON_VALUE;
            }
            parent->childsCount++;
        }
        if(token->type == TM_RIGHT_BRACE) {
            return token + 1;
        }
        token += 1;
    }
}

static void JsonObjectFree(TMJsonObject *object) {
    if(object->type == TM_JSON_OBJECT) {
        if(object->valuesCount) {
            for(int i = 0; i < object->valuesCount; ++i) {
                TMJsonObject *child = object->objects + i;
                JsonObjectFree(child); 
            }
            TMDarrayDestroy(object->objects);
        }
    }
    if(object->type == TM_JSON_VALUE) {
        if(object->valuesCount) {
            TMDarrayDestroy(object->values);
        }
    }
    if(object->childsCount) {
        for(int i = 0; i < object->childsCount; ++i) {
            TMJsonObject *child = object->childs + i;
            JsonObjectFree(child);
        }
        TMDarrayDestroy(object->childs);
    }
}

static void PrintJsonObject(TMJsonObject *object) {
    if(object->name) {
        char *name = (char *)malloc(object->nameSize + 1);
        memcpy((void *)name, (void *)object->name, object->nameSize);
        name[object->nameSize] = '\0';
        printf("%s: ", name);
        free(name);
    }
    if(object->values) {
        if(object->type == TM_JSON_VALUE) {
            for(int i = 0; i < object->valuesCount; ++i) {
                const char *objectValue = object->values[i].value;
                size_t objectValueSize  = object->values[i].size;
                char *value = (char *)malloc(objectValueSize + 1);
                memcpy((void *)value, (void *)objectValue, objectValueSize);
                value[objectValueSize] = '\0';
                printf("%s\n", value);
                free(value);
            }
        }
        if(object->type == TM_JSON_OBJECT) {
            for(int i = 0; i < object->valuesCount; ++i) {
                TMJsonObject *objectValue = &object->objects[i];
                PrintJsonObject(objectValue);
            }
        }
    }
    printf("child count: %d\n", (unsigned int)object->childsCount);
    for(int i = 0; i < object->childsCount; ++i) {
        PrintJsonObject(object->childs + i);
    }

}

TMJson *TMJsonOpen(const char *filepath) {
    TMJson *json = (TMJson *)malloc(sizeof(TMJson));
    memset(json, 0, sizeof(TMJson));
    json->file = TMFileOpen(filepath);

    JsonScanner scanner{};
    ScanTokens(&scanner, (const char *)json->file.data, json->file.size);

    ParserTokens(&json->root, scanner.tokens);
    TMDarrayDestroy(scanner.tokens);
    PrintJsonObject(&json->root);
    
    return json;
}

void TMJsonClose(TMJson *json) {
    JsonObjectFree(&json->root);
    TMFileClose(&json->file);
    free(json);
}


// for now we do a linear search ...
// TODO: change this to use a hashtable
TMJsonObject *TMJsonFindChildByName(TMJsonObject *object, const char *name) {
    if(object->type == TM_JSON_NULL) {
        for(int i = 0; i < object->childsCount; ++i) {
            TMJsonObject *child = object->childs + i;
            if(child->name && (strncmp(name, child->name, strlen(name)) == 0)) {
                return child;
            }
        }
    }
    else if(object->type == TM_JSON_OBJECT) {
        for(int i = 0; i < object->valuesCount; ++i) {
            TMJsonObject *child = object->objects + i;
            if(child->name && (strncmp(name, child->name, strlen(name)) == 0)) {
                return child;
            }
        }
    }
    TMJsonObject *result = NULL;
    if(object->type == TM_JSON_NULL) {
        for(int i = 0; i < object->childsCount; ++i) {
            TMJsonObject *child = object->childs + i;
            if(child->type != TM_JSON_VALUE) {
                result = TMJsonFindChildByName(child, name);
                if(result != NULL) break;
            }
        }       
    }
    else if(object->type == TM_JSON_OBJECT) {
        for(int i = 0; i < object->valuesCount; ++i) {
            TMJsonObject *child = object->objects + i;
            if(child->type != TM_JSON_VALUE) {
                result = TMJsonFindChildByName(child, name);
                if(result != NULL) break;
            }
        }
    }
    return result;
}
