#include "tm_ui.h"
#include "tm_renderer.h"
#include "utils/tm_darray.h"
#include "tm_input.h"


// TODO: REDO ....

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

void OnClickStub(TMUIElement *element) {}

TMUIElement *TMUIElementCreate(TMVec2 position, TMVec2 size, TMVec4 color,
                              TMUIOrientation orientation,
                              TMUIType type, TMRenderBatch *renderBatch) {

    TMUIElement *element = (TMUIElement *)malloc(sizeof(TMUIElement));
    memset(element, 0, sizeof(TMUIElement));
    element->position = position;
    element->size = size;
    element->color = color;
    element->oldColor = color;
    element->orientation = orientation;
    element->type = type;
    element->renderBatch = renderBatch;
    element->onCLick = OnClickStub;
    return element;
}

static void Freechilds(TMUIElement *element) {
    if(element->childs) {
        int childCount = TMDarraySize(element->childs);
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = element->childs + i;
            Freechilds(child);
        }
        TMDarrayDestroy(element->childs);
    }
}

void TMUIElementDestroy(TMUIElement *element) {
    Freechilds(element);
    free(element);
}

void TMUIElementAddChildButton(TMUIElement *parent, TMUIOrientation orientation, TMVec4 color, TMRenderBatch *renderBatch, PFN_OnClick onCLick) {
    TMUIElement element{};
    element.type = TM_UI_TYPE_BUTTON;
    element.orientation = orientation;
    element.color = color;
    element.oldColor = color;
    element.renderBatch = renderBatch;
    element.onCLick = onCLick;
    TMDarrayPush(parent->childs, element, TMUIElement);
    int childCount = TMDarraySize(parent->childs);
    
    if(parent->orientation == TM_UI_ORIENTATION_HORIZONTAL) {
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = parent->childs + i;
            child->position.x = parent->position.x + (parent->size.x/childCount)*i;
            child->position.y = parent->position.y;
            child->size.x = (parent->size.x/childCount);
            child->size.y = parent->size.y;
            child->index = i;

        }
    }
    else if(parent->orientation == TM_UI_ORIENTATION_VERTICAL) {
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = parent->childs + i;
            child->position.x = parent->position.x;
            child->position.y = parent->position.y + (parent->size.y/childCount)*i;
            child->size.x = parent->size.x;
            child->size.y = (parent->size.y/childCount);
            child->index = i;

        }
    }
}

void TMUIElementAddChildImageButton(TMUIElement *parent, TMUIOrientation orientation, TMVec4 uvs, TMRenderBatch *renderBatch, PFN_OnClick onCLick) {
    TMUIElement element{};
    element.type = TM_UI_TYPE_IMAGE_BUTTON;
    element.orientation = orientation;
    element.color = {0, 0, 0, 0};
    element.oldColor = element.color;
    element.uvs = uvs;
    element.renderBatch = renderBatch;
    element.onCLick = onCLick;
    TMDarrayPush(parent->childs, element, TMUIElement);
    int childCount = TMDarraySize(parent->childs);
    
    if(parent->orientation == TM_UI_ORIENTATION_HORIZONTAL) {
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = parent->childs + i;
            child->position.x = parent->position.x + (parent->size.x/childCount)*i;
            child->position.y = parent->position.y;
            child->size.x = (parent->size.x/childCount);
            child->size.y = parent->size.y;
            child->index = i;

        }
    }
    else if(parent->orientation == TM_UI_ORIENTATION_VERTICAL) {
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = parent->childs + i;
            child->position.x = parent->position.x;
            child->position.y = parent->position.y + (parent->size.y/childCount)*i;
            child->size.x = parent->size.x;
            child->size.y = (parent->size.y/childCount);
            child->index = i;

        }
    }
}

void TMUIElementAddChildLabel(TMUIElement *parent, TMUIOrientation orientation,
                              TMVec4 font, float *fontUvs, const char *text,
                              TMRenderBatch *renderBatch, PFN_OnClick onCLick) {
    TMUIElement element{};
    element.type = TM_UI_TYPE_LABEL;
    element.orientation = orientation;
    element.color = {0, 0, 0, 0};
    element.oldColor = element.color;
    element.uvs = font;
    element.relUVs = fontUvs;
    element.text = text;
    element.renderBatch = renderBatch;
    element.onCLick = onCLick;
    TMDarrayPush(parent->childs, element, TMUIElement);
    int childCount = TMDarraySize(parent->childs);
    
    if(parent->orientation == TM_UI_ORIENTATION_HORIZONTAL) {
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = parent->childs + i;
            child->position.x = parent->position.x + (parent->size.x/childCount)*i;
            child->position.y = parent->position.y;
            child->size.x = (parent->size.x/childCount);
            child->size.y = parent->size.y;
            child->index = i;

        }
    }
    else if(parent->orientation == TM_UI_ORIENTATION_VERTICAL) {
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = parent->childs + i;
            child->position.x = parent->position.x;
            child->position.y = parent->position.y + (parent->size.y/childCount)*i;
            child->size.x = parent->size.x;
            child->size.y = (parent->size.y/childCount);
            child->index = i;

        }
    }
}

TMUIElement *TMUIElementGetChild(TMUIElement *element, int index) {
    assert(element->childs != NULL);
    int childCount = TMDarraySize(element->childs);
    assert(index < childCount);
    return element->childs + index;
}


void TMUIElementProcessInput(TMUIElement *element,
                             float offsetX, float offsetY, 
                             float width, float height,
                             TMMat4 proj, TMMat4 view) {
    float mouseX = (float)TMInputMousePositionX();
    float mouseY = height - (float)TMInputMousePositionY();

    TMMat4 invView = TMMat4Inverse(view);
    TMMat4 invProj = TMMat4Inverse(proj);

    TMVec2 screenCoord = {mouseX, mouseY};
    TMVec2 viewportPos = {offsetX, offsetY};
    TMVec2 viewportSize = {width, height};
    TMVec2 normalizeP = (screenCoord - viewportPos) / viewportSize;
    TMVec2 one = {1, 1};
    normalizeP = (normalizeP * 2.0f) - one;
    TMVec4 position = {normalizeP.x, normalizeP.y, 0 , 1};
    position = invView * invProj * position;

    mouseX = position.x;
    mouseY = position.y;

    float minX = element->position.x;
    float maxX = minX + element->size.x;
    float minY = element->position.y;
    float maxY = minY + element->size.y;

    if(mouseX > minX && mouseX <= maxX &&
       mouseY > minY && mouseY <= maxY) {
        if(element->childs) {
            int childCount = TMDarraySize(element->childs);
            for(int i = 0; i < childCount; ++i) {
                TMUIElement *child = element->childs + i;
                TMUIElementProcessInput(child, offsetX, offsetY, width, height, proj, view);
            }
        }
        else {
            element->isHot = true;
            element->color = {1, 1, 1, 1};
            if(TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT)) {
                element->onCLick(element);
            }
            if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_LEFT)) {
                element->color = {0.7, 0.7, 0.7, 1};
            }
        }

    }
    else {
        element->isHot = false;
        element->color = element->oldColor;
        if(element->childs) {
            int childCount = TMDarraySize(element->childs);
            for(int i = 0; i < childCount; ++i) {
                TMUIElement *child = element->childs + i;
                TMUIElementProcessInput(child, offsetX, offsetY, width, height, proj, view);
            }
        }
        
    }

}




void TMUIElementDraw(TMUIElement *element, float increment) {

    if(element->type == TM_UI_TYPE_BUTTON || element->isHot) {
        TMRendererRenderBatchAdd(element->renderBatch,
                                 element->position.x + element->size.x*0.5f,
                                 element->position.y + element->size.y*0.5f,
                                 1.0f + increment,
                                 element->size.x, element->size.y, 0.0f,
                                 element->color.x, element->color.y,
                                 element->color.z, element->color.w);
    }
    else {
        if(element->type == TM_UI_TYPE_IMAGE_BUTTON) {
            TMRendererRenderBatchAdd(element->renderBatch,
                                     element->position.x + element->size.x*0.5f,
                                     element->position.y + element->size.y*0.5f,
                                     1.0f + increment,
                                     element->size.x, element->size.y, 0,
                                     element->uvs.v);
        } else if (element->type == TM_UI_TYPE_LABEL) {
            int letterCount = 0;
            char *string = (char *)element->text;
            while(*string != '\0') { ++letterCount; string++; }
            float letterWidth = element->size.x / letterCount;
            int offset = 0;
            string = (char *)element->text;
            while(*string != '\0') {
                int letter = (int)*string;
                string++;
                if(letter >= 32 && letter <= 126) {
                    letter -=32;
                    int x = letter % 18;
                    int y = letter / 18;
                    int letterIndex = (y * 18 + x);
                    float elementPosX = element->position.x + letterWidth * offset;
                    TMRendererRenderBatchAdd(element->renderBatch,
                                             elementPosX + letterWidth*0.5f,
                                             element->position.y + element->size.y*0.5f,
                                             1.0f + increment,
                                             letterWidth, element->size.y, 0,
                                             element->uvs, letterIndex, element->relUVs);
                    offset++;
                }
            }
        }
    }


    if(element->childs) {
        int childCount = TMDarraySize(element->childs);

        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = element->childs + i;
            TMUIElementDraw(child, increment - 0.01f);
        }
    }
}
