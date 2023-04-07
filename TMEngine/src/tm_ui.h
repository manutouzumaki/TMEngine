#ifndef _TM_UI_H_
#define _TM_UI_H_

#include "tm_defines.h"
#include "utils/tm_math.h"

struct TMRenderBatch;

enum TMUIType {
    TM_UI_TYPE_UNDEFINE,
    TM_UI_TYPE_BUTTON,
    TM_UI_TYPE_IMAGE_BUTTON,
    TM_UI_TYPE_LABEL
};

enum TMUIOrientation {
    TM_UI_ORIENTATION_HORIZONTAL,
    TM_UI_ORIENTATION_VERTICAL
};

struct TMUIElement {
    TMUIElement *childs;
    TMRenderBatch *renderBatch;
    
    TMVec2 position;
    TMVec2 size;
    TMVec4 color;

    TMUIOrientation orientation;
    TMUIType type;
    int index;
    bool isHot;
};


TM_EXPORT TMUIElement *TMUIElementCreate(TMVec2 position, TMVec2 size, TMVec4 color,
                              TMUIOrientation orientation,
                              TMUIType type, TMRenderBatch *renderBatch);
TM_EXPORT void TMUIElementDestroy(TMUIElement *element);
TM_EXPORT void TMUIElementAddChild(TMUIElement *parent, TMUIOrientation orientation, TMVec4 color, TMRenderBatch *renderBatch);
TM_EXPORT TMUIElement *TMUIElementGetChild(TMUIElement *element, int index);
TM_EXPORT void TMUIElementDraw(TMUIElement *element);

#endif
