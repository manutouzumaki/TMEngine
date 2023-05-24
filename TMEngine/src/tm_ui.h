#ifndef _TM_UI_H_
#define _TM_UI_H_

#include "tm_defines.h"
#include "utils/tm_math.h"

#include <stdlib.h>

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


struct TMUIElement;
struct TMShader;
struct TMTexture;
struct TMRenderer;

typedef void (*PFN_OnClick) (TMUIElement *element);

struct TMUIElement {
    TMUIType type;
    TMUIOrientation orientation;
    TMShader *shader;
    TMTexture *texture;
 
    TMVec2 position;
    TMVec2 size;
    TMVec4 color;
    TMVec4 oldColor;
    TMVec4 absUVs;
    TMVec4 relUVs;
    int textureIndex;

    const char *text;
    int index;
    bool isHot;

    PFN_OnClick onCLick;
    TMUIElement *childs;

    void *userData;
};

TM_EXPORT void TMUIMouseIsHot(TMUIElement *element, bool *result);
TM_EXPORT void TMUIUpdateProjMatrix(TMRenderer *renderer, float MetersToPixel);

TM_EXPORT void TMUIInitialize(TMRenderer *renderer, float MetersToPixel);
TM_EXPORT void TMUIShutdown(TMRenderer *renderer);


TM_EXPORT TMUIElement *TMUIElementCreateButton(TMUIOrientation orientation, TMVec2 position, TMVec2 size, TMVec4 color,
                                               PFN_OnClick onCLick = NULL, void *userData = NULL);
TM_EXPORT TMUIElement *TMUIElementCreateImageButton(TMUIOrientation orientation, TMVec2 position, TMVec2 size,
                                                    TMTexture *texture, TMVec4 absUVs, TMVec4 relUVs,
                                                    PFN_OnClick onCLick = NULL, void *userData = NULL);
TM_EXPORT TMUIElement *TMUIElementCreateLabel(TMUIOrientation orientation, TMVec2 position, TMVec2 size,
                                              const char *text, TMVec4 color,
                                              PFN_OnClick onCLick = NULL, void *userData = NULL);

TM_EXPORT void TMUIElementAddChildButton(TMUIElement *parent, TMUIOrientation orientation, TMVec4 color,
                                         PFN_OnClick onCLick = NULL, void *userData = NULL);
TM_EXPORT void TMUIElementAddChildImageButton(TMUIElement *parent, TMUIOrientation orientation,
                                              TMTexture *texture, TMVec4 absUVs, TMVec4 relUVs,
                                              PFN_OnClick onCLick = NULL, void *userData = NULL, int textureIndex = 0);
TM_EXPORT void TMUIElementAddChildLabel(TMUIElement *parent, TMUIOrientation orientation,
                                        const char *text, TMVec4 color,
                                        PFN_OnClick onCLick = NULL, void *userData = NULL);

TM_EXPORT TMUIElement *TMUIElementGetChild(TMUIElement *element, int index);

TM_EXPORT void TMUIElementRecalculateChilds(TMUIElement *element);

TM_EXPORT void TMUIElementDestroy(TMUIElement *element);
TM_EXPORT void TMUIElementProcessInput(TMUIElement *element, float offsetX, float offsetY, float width, float height, float scale);
TM_EXPORT void TMUIElementDraw(TMRenderer *renderer, TMUIElement *element, float increment);

#endif
