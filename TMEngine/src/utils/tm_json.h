#ifndef _TM_JSON_H_
#define _TM_JSON_H_

#include "tm_file.h"

 // TODO(manuel):
 // - add booleans to the json parser
 // - add cientific notation ej: 1.862645149230957e-09
    

enum TMValueType {
    TM_JSON_NULL = 0,
    TM_JSON_VALUE = 1,
    TM_JSON_OBJECT = 2
};

struct TMJsonValue {
    const char *value;
    size_t size;
};

struct TMJsonObject {
    const char *name;
    size_t nameSize;
    union {
        TMJsonValue *values;
        TMJsonObject *objects;
    };
    size_t valuesCount;
    TMValueType type;

    TMJsonObject *childs;
    size_t childsCount;
};

struct TMJson {
    TMFile file;
    TMJsonObject root;
};

TMJson *TMJsonOpen(const char *filepath);
void TMJsonClose(TMJson *json);
TMJsonObject *TMJsonFindChildByName(TMJsonObject *object, const char *name);

#endif
