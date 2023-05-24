#include "tm_ui.h"
#include "tm_renderer.h"
#include "utils/tm_darray.h"
#include "tm_input.h"

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

struct ConstBuffer {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
};

static TMVertex vertices[] = {
        TMVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec3{0, 0, 0}}, // 0
        TMVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}, TMVec3{0, 0, 0}}, // 1
        TMVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec3{0, 0, 0}}, // 2
        TMVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}, TMVec3{0, 0, 0}} // 3
};

static unsigned int indices[] = {
        1, 0, 2, 2, 0, 3
};

// TODO: pack this into a struct
static TMShader *gColorShader;
static TMShader *gSpriteShader;
static TMBuffer *gVertexBuffer;
static TMShaderBuffer *gShaderBuffer;
static ConstBuffer gConstBuffer;
static TMTexture *gFontTexture;
static float *gFontUVs;
static int gFontCount;

void TMUIInitialize(TMRenderer *renderer, float MetersToPixel) {

    gSpriteShader = TMRendererShaderCreate(renderer,
                                           "../../assets/shaders/uiVert.hlsl",
                                           "../../assets/shaders/spriteFrag.hlsl");

    gColorShader = TMRendererShaderCreate(renderer,
                                          "../../assets/shaders/uiVert.hlsl",
                                          "../../assets/shaders/colorFrag.hlsl");

    gVertexBuffer = TMRendererBufferCreate(renderer,
                                          vertices, ARRAY_LENGTH(vertices),
                                          indices, ARRAY_LENGTH(indices),
                                          gColorShader);


    int width = TMRendererGetWidth(renderer);
    int height = TMRendererGetHeight(renderer);
    TMVec3 pos = {0, 0, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    TMMat4 view = TMMat4LookAt(pos, pos + tar, up);
    TMMat4 proj = TMMat4Ortho(0, width/MetersToPixel, 0, height/MetersToPixel, 0.1f, 100.0f);
    gConstBuffer.proj = proj;
    gConstBuffer.view = view;
    gConstBuffer.world = TMMat4Identity();
    gConstBuffer.color = {1, 1, 1, 1};
    gConstBuffer.absUVs = {0, 0, 1, 1};
    gConstBuffer.relUVs = {0, 0, 1, 1};
    gShaderBuffer = TMRendererShaderBufferCreate(renderer, &gConstBuffer, sizeof(ConstBuffer), 13);

    gFontTexture = TMRendererTextureCreate(renderer, "../../assets/images/font2.png");
    gFontUVs = TMGenerateUVs(128, 64, 7, 9, &gFontCount);
}

void TMUIUpdateProjMatrix(TMRenderer *renderer, float MetersToPixel) {
    int width = TMRendererGetWidth(renderer);
    int height = TMRendererGetHeight(renderer);
    TMMat4 proj = TMMat4Ortho(0, width/MetersToPixel, 0, height/MetersToPixel, 0.1f, 100.0f);
    gConstBuffer.proj = proj;
    gShaderBuffer = TMRendererShaderBufferCreate(renderer, &gConstBuffer, sizeof(ConstBuffer), 13);
}

void TMUIShutdown(TMRenderer *renderer) {
    free(gFontUVs);
    TMRendererTextureDestroy(renderer, gFontTexture);
    TMRendererShaderBufferDestroy(renderer, gShaderBuffer);
    TMRendererBufferDestroy(renderer, gVertexBuffer);
    TMRendererShaderDestroy(renderer, gColorShader);
    TMRendererShaderDestroy(renderer, gSpriteShader);
}


TMUIElement *TMUIElementCreateButton(TMUIOrientation orientation, TMVec2 position, TMVec2 size,
                                     TMVec4 color,
                                     PFN_OnClick onCLick, void *userData) {

    TMUIElement *element = (TMUIElement *)malloc(sizeof(TMUIElement));
    memset(element, 0, sizeof(TMUIElement));
    element->type = TM_UI_TYPE_BUTTON;
    element->orientation = orientation;
    
    element->shader = gColorShader;

    element->position = position;
    element->size = size;
    element->color = color;
    element->oldColor = color;
    
    element->onCLick = onCLick;
    
    element->userData = userData;

    return element;
}

TMUIElement *TMUIElementCreateImageButton(TMUIOrientation orientation, TMVec2 position, TMVec2 size,
                                          TMTexture *texture, TMVec4 absUVs, TMVec4 relUVs,
                                          PFN_OnClick onCLick, void *userData) {

    TMUIElement *element = (TMUIElement *)malloc(sizeof(TMUIElement));
    memset(element, 0, sizeof(TMUIElement));
    element->type = TM_UI_TYPE_IMAGE_BUTTON;
    element->orientation = orientation;
    
    element->shader = gSpriteShader;
    element->texture = texture;
    element->absUVs = absUVs;
    element->relUVs = relUVs;

    element->position = position;
    element->size = size;
    element->color = {1, 1, 1, 1};
    element->oldColor = {1, 1, 1, 1};
    
    element->onCLick = onCLick;
    
    element->userData = userData;

    return element;
}

TMUIElement *TMUIElementCreateLabel(TMUIOrientation orientation, TMVec2 position, TMVec2 size,
                                    const char *text, TMVec4 color,
                                    PFN_OnClick onCLick, void *userData) {

    TMUIElement *element = (TMUIElement *)malloc(sizeof(TMUIElement));
    memset(element, 0, sizeof(TMUIElement));
    element->type = TM_UI_TYPE_LABEL;
    element->orientation = orientation;
    
    element->shader = gSpriteShader;
    element->texture = gFontTexture;
    element->absUVs = {0, 0, 1, 1};
    element->relUVs = {0, 0, 1, 1};
    element->text = text;

    element->position = position;
    element->size = size;
    element->color = color ;
    element->oldColor = color;
    
    element->onCLick = onCLick;

    element->userData = userData;

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

void TMUIElementAddChildButton(TMUIElement *parent, TMUIOrientation orientation,
                               TMVec4 color,
                               PFN_OnClick onCLick, void *userData) {
    TMUIElement element{};
    element.type = TM_UI_TYPE_BUTTON;
    element.orientation = orientation;
    element.shader = gColorShader;
    element.color = color;
    element.oldColor = color;
    element.onCLick = onCLick;
    element.userData = userData;

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

void TMUIElementAddChildImageButton(TMUIElement *parent, TMUIOrientation orientation,
                                    TMTexture *texture, TMVec4 absUVs, TMVec4 relUVs,
                                    PFN_OnClick onCLick, void *userData, int textureIndex) {
    TMUIElement element{};
    element.type = TM_UI_TYPE_IMAGE_BUTTON;
    element.orientation = orientation;
    element.shader = gSpriteShader;
    element.texture = texture;
    element.absUVs = absUVs;
    element.relUVs = relUVs;
    element.color = {1, 1, 1, 1};
    element.oldColor = {1, 1, 1, 1};
    element.onCLick = onCLick;
    element.userData = userData;
    element.textureIndex = textureIndex;


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
                              const char *text, TMVec4 color,
                              PFN_OnClick onCLick, void *userData) {
    TMUIElement element{};
    element.type = TM_UI_TYPE_LABEL;
    element.orientation = orientation;
    element.shader = gSpriteShader;
    element.texture = gFontTexture;
    element.absUVs = {0, 0, 1, 1};
    element.relUVs = {0, 0, 1, 1};
    element.text = text;
    element.color = color;
    element.oldColor = color;
    element.onCLick = onCLick;
    element.userData = userData;

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

bool CheckChilds(TMUIElement *element) {
    if(element->childs) {
        for(int i = 0; i < TMDarraySize(element->childs); ++i) {
            TMUIElement *child = element->childs + i;
            if(child->isHot) {
                return true;
            }
            CheckChilds(child);
        }
        return false;
    }
    return false;
}

void TMUIMouseIsHot(TMUIElement *element, bool *result) {
    if(element->childs) {
        int childCount = TMDarraySize(element->childs);
        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = element->childs + i;
            TMUIMouseIsHot(child, result);
        }
    }
    else {
        if(*result == false) *result = element->isHot;
    }
}

static void MouseToWorld(float *mouseX, float *mouseY, float width, float height, float scale) {
    float x = (float)TMInputMousePositionX();
    float y = height - (float)TMInputMousePositionY();
    float worldMouseX = (x / width)  * (width/scale);
    float worldMouseY = (y / height) * (height/scale);
    *mouseX = worldMouseX;
    *mouseY = worldMouseY;
}


void TMUIElementRecalculateChilds(TMUIElement *element) {

    TMUIElement *parent = element;
    int childCount = TMDarraySize(parent->childs);

    if(parent->orientation == TM_UI_ORIENTATION_HORIZONTAL) {

        for(int i = 0; i < childCount; ++i) {
            
            TMUIElement *child = parent->childs + i;
            child->position.x = parent->position.x + (parent->size.x/childCount)*i;
            child->position.y = parent->position.y;
            child->size.x = (parent->size.x/childCount);
            child->size.y = parent->size.y;
            child->index = i;
            if(child->childs) {
                TMUIElementRecalculateChilds(child);
            }

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
            if(child->childs) {
                TMUIElementRecalculateChilds(child);
            }
        }
    }

}



void TMUIElementProcessInput(TMUIElement *element,
                             float offsetX, float offsetY,
                             float width, float height, float scale) {
    float mouseX;
    float mouseY;
    MouseToWorld(&mouseX, &mouseY, width, height, scale);

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
                TMUIElementProcessInput(child, offsetX, offsetY, width, height, scale);
            }
        }
        else {
            element->isHot = true;
            element->color = {0.3, 0.3, 0.3, 1};
            if(TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT)) {
                if(element->onCLick) element->onCLick(element);
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
                TMUIElementProcessInput(child, offsetX, offsetY, width, height, scale);
            }
        }
        
    }

}

void TMUIElementDraw(TMRenderer *renderer, TMUIElement *element, float increment) {
    
    TMRendererBindShader(renderer, element->shader);
    if(element->texture) TMRendererTextureBind(renderer, element->texture, element->shader, "uTexture", 0);

    if(element->type == TM_UI_TYPE_BUTTON || element->type == TM_UI_TYPE_IMAGE_BUTTON) {
        TMMat4 trans = TMMat4Translate(element->position.x + element->size.x*0.5f,
                                       element->position.y + element->size.y*0.5f,
                                       1.0f + increment);
        TMMat4 scale = TMMat4Scale(element->size.x, element->size.y, 1.0f);
        gConstBuffer.world = trans * scale;
        gConstBuffer.color = element->color;
        gConstBuffer.absUVs = element->absUVs;
        gConstBuffer.relUVs = element->relUVs;
        TMRendererShaderBufferUpdate(renderer, gShaderBuffer, &gConstBuffer);
        TMRendererDrawBufferElements(renderer, gVertexBuffer);
    }
    else {
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
                TMVec4 *relUVs = (TMVec4 *)gFontUVs;

                TMMat4 trans = TMMat4Translate(elementPosX + letterWidth*0.5f,
                                               element->position.y + element->size.y*0.5f,
                                               1.0f + increment);
                TMMat4 scale = TMMat4Scale(letterWidth, element->size.y, 1.0f);
                gConstBuffer.world = trans * scale;
                gConstBuffer.color = element->color;
                gConstBuffer.absUVs = element->absUVs;
                gConstBuffer.relUVs = relUVs[letterIndex];
                TMRendererShaderBufferUpdate(renderer, gShaderBuffer, &gConstBuffer);
                TMRendererDrawBufferElements(renderer, gVertexBuffer);

                offset++;
            }
        }
    }

    if(element->childs) {
        int childCount = TMDarraySize(element->childs);

        for(int i = 0; i < childCount; ++i) {
            TMUIElement *child = element->childs + i;
            TMUIElementDraw(renderer, child, increment - 0.01f);
        }
    }
}
