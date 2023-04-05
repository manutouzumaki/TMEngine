#ifndef _TM_JSON_H_
#define _TM_JSON_H_

#include "../tm_defines.h"
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
    union {
        const char *value;
        float valueFloat;
    };
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
    bool isFloat;
};


struct TMJson {
    TMFile file;
    TMJsonObject root;
};

TM_EXPORT TMJson *TMJsonOpen(const char *filepath);
TM_EXPORT void TMJsonClose(TMJson *json);
TM_EXPORT TMJsonObject *TMJsonFindChildByName(TMJsonObject *object, const char *name);



TM_EXPORT void TMJsonObjectSetName(TMJsonObject *object, const char *name);
TM_EXPORT void TMJsonObjectSetValue(TMJsonObject *object, const char *value);
TM_EXPORT void TMJsonObjectSetValue(TMJsonObject *object, float value);
TM_EXPORT void TMJsonObjectSetValue(TMJsonObject *object, TMJsonObject *value);
TM_EXPORT void TMJsonObjectAddChild(TMJsonObject *parent, TMJsonObject *child);

TM_EXPORT void TMJsonObjectStringify(TMJsonObject *object, char *buffer, int *position);


TM_EXPORT TMJsonObject TMJsonObjectCreate();
TM_EXPORT void TMJsonObjectFree(TMJsonObject *object);


#endif
