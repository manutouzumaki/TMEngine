#include "tm_ui.h"
#include "tm_renderer.h"
#include "utils/tm_darray.h"


// TODO: REDO ....

#include <assert.h>
#include <stdlib.h>
#include <memory.h>


TMUIElement *TMUIElementCreate(TMVec2 position, TMVec2 size, TMVec4 color,
                              TMUIOrientation orientation,
                              TMUIType type, TMRenderBatch *renderBatch) {

    TMUIElement *element = (TMUIElement *)malloc(sizeof(TMUIElement));
    memset(element, 0, sizeof(TMUIElement));
    element->position = position;
    element->size = size;
    element->color = color;
    element->orientation = orientation;
    element->type = type;
    element->renderBatch = renderBatch;
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


void TMUIElementAddChild(TMUIElement *parent, TMUIOrientation orientation, TMVec4 color, TMRenderBatch *renderBatch) {
    TMUIElement element{};
    element.orientation = orientation;
    element.color = color;
    element.renderBatch = renderBatch;
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
            child->position.y = parent->position.y - (parent->size.y/childCount)*i;
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

void TMUIElementDraw(TMUIElement *element) {
    TMRendererRenderBatchAdd(element->renderBatch,
                             element->position.x + element->size.x*0.5f,
                             element->position.y - element->size.y*0.5f,
                             1.0f,
                             element->size.x, element->size.y, 0.0f,
                             element->color.x, element->color.y,
                             element->color.z, element->color.w);

    if(element->childs) {
        int childCount = TMDarraySize(element->childs);

        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = element->childs + i;
            TMUIElementDraw(child);
        }
    }
}
