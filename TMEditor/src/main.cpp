#include <tm_defines.h>
#include <tm_window.h>
#include <tm_renderer.h>
#include <tm_ui.h>
#include <tm_debug_renderer.h>
#include <utils/tm_hashmap.h>
#include <utils/tm_math.h>
#include <utils/tm_darray.h>
#include <utils/tm_json.h>
#include <tm_input.h>
#include <stdio.h>
#include <math.h>

struct ConstBuffer {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
};

struct Entity {
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
    TMVec2 position;
    TMVec2 size;
    TMShader *shader;
    TMTexture *texture;
};

enum ModifyOption {
    MODIFY_NONE,
    MODIFY_TRANSLATE,
    MODIFY_SCALE
};

struct EditorState {
    int option;
    TMUIElement *element;
    Entity *entities;
    Entity *selectedEntity;

    TMBuffer *vertexBuffer;
    TMShaderBuffer *shaderBuffer;
    TMShader *colorShader;
    TMShader *spriteShader;

    ModifyOption modifyOption;


};

static float MetersToPixel = 100;
static EditorState gEditorState;

static TMVertex vertices[] = {
        TMVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec3{0, 0, 0}}, // 0
        TMVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}, TMVec3{0, 0, 0}}, // 1
        TMVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec3{0, 0, 0}}, // 2
        TMVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}, TMVec3{0, 0, 0}} // 3
};

static unsigned int indices[] = {
        1, 0, 2, 2, 0, 3
};

static TMVec3 gCameraPos = {0, 0, 0};
static bool gMouseIsHot;

TMVec4 Texture(TMHashmap *hashmap, const char *filepath) {
    TMVec4 result = *((TMVec4 *)TMHashmapGet(hashmap, filepath));
    return result; 
}

float Max(float a, float b) {
    if(a > b) return a;
    return b;
}

float Min(float a, float b) {
    if(a < b) return a;
    return b;
}

void OptionSelected(TMUIElement *element) {
    printf("Option selected\n");
    gEditorState.option = element->index;
    gEditorState.modifyOption = MODIFY_NONE;
    gEditorState.selectedEntity = NULL;
}

void ClearSelected(TMUIElement *element) {
    printf("Clear selected\n");
    gEditorState.element = NULL;
}

void ElementSelected(TMUIElement *element) {
    printf("Element selected\n");
    gEditorState.element = element;
    gEditorState.modifyOption = MODIFY_NONE;
    gEditorState.selectedEntity = NULL;
}

void AddEntity(float posX, float posY) {
    printf("Entity added\n");
    TMUIElement *element = gEditorState.element;
    Entity entity = {};
    entity.color = element->oldColor;
    entity.absUVs = element->absUVs;
    entity.relUVs = element->relUVs;
    entity.position = {floorf(posX) + 0.5f, floorf(posY) + 0.5f};
    entity.size = {1, 1};
    entity.texture = element->texture;
    if(element->type == TM_UI_TYPE_BUTTON) {
        entity.shader = gEditorState.colorShader;
    }
    else {
        entity.shader = gEditorState.spriteShader;
    }
    TMDarrayPush(gEditorState.entities, entity, Entity);

}

void TranslateEntity(TMUIElement *element) {
    gEditorState.modifyOption = MODIFY_TRANSLATE;
}

void ScaleEntity(TMUIElement *element) {
    gEditorState.modifyOption = MODIFY_SCALE;
}

void MouseToWorld(float *mouseX, float *mouseY, float width, float height) {
    float x = (float)TMInputMousePositionX();
    float y = height - (float)TMInputMousePositionY();

    float worldMouseX = (x / width)  * (width/MetersToPixel);
    float worldMouseY = (y / height) * (height/MetersToPixel);

    *mouseX = worldMouseX + gCameraPos.x;
    *mouseY = worldMouseY + gCameraPos.y;
}

void LastMouseToWorld(float *mouseX, float *mouseY, float width, float height) {
    float x = (float)TMInputMouseLastPositionX();
    float y = height - (float)TMInputMouseLastPositionY();

    float worldMouseX = (x / width)  * (width/MetersToPixel);
    float worldMouseY = (y / height) * (height/MetersToPixel);

    *mouseX = worldMouseX + gCameraPos.x;
    *mouseY = worldMouseY + gCameraPos.y;
}


void SaveScene(TMUIElement *element) {
    printf("Scene Saved\n");

    TMJsonObject jsonRoot = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonRoot, "Root");

    TMJsonObject jsonScene = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonScene, "Scene");

    for(int i = 0; i < TMDarraySize(gEditorState.entities); ++i) {
        Entity *entity = gEditorState.entities + i;

        TMJsonObject jsonEntity = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonEntity, "Entity");
        
        // Save Graphic Component
        {
            TMJsonObject jsonGraphic = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonGraphic, "Graphics");

            // graphics type
            TMJsonObject jsonType = TMJsonObjectCreate();

            TMJsonObjectSetName(&jsonType, "Type");
            if(entity->shader == gEditorState.colorShader) {
                TMJsonObjectSetValue(&jsonType, 0.0f);
            }
            else if(entity->shader == gEditorState.spriteShader) {
                TMJsonObjectSetValue(&jsonType, 1.0f);
            }

            // position
            TMJsonObject jsonPosition = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonPosition, "Position");
            TMJsonObject xPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&xPos, "X");
            TMJsonObjectSetValue(&xPos, entity->position.x);
            TMJsonObject yPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&yPos, "Y");
            TMJsonObjectSetValue(&yPos, entity->position.y);
            TMJsonObjectAddChild(&jsonPosition, &xPos);
            TMJsonObjectAddChild(&jsonPosition, &yPos);

            // size
            TMJsonObject jsonSize = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonSize, "Size");
            TMJsonObject xSiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&xSiz, "X");
            TMJsonObjectSetValue(&xSiz, entity->size.x);
            TMJsonObject ySiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&ySiz, "Y");
            TMJsonObjectSetValue(&ySiz, entity->size.y);
            TMJsonObjectAddChild(&jsonSize, &xSiz);
            TMJsonObjectAddChild(&jsonSize, &ySiz);

            // color
            TMJsonObject jsonColor = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonColor, "Color");
            TMJsonObject r = TMJsonObjectCreate();
            TMJsonObjectSetName(&r, "R");
            TMJsonObjectSetValue(&r, entity->color.x);
            TMJsonObject g = TMJsonObjectCreate();
            TMJsonObjectSetName(&g, "G");
            TMJsonObjectSetValue(&g, entity->color.y);
            TMJsonObject b = TMJsonObjectCreate();
            TMJsonObjectSetName(&b, "B");
            TMJsonObjectSetValue(&b, entity->color.z);
            TMJsonObject a = TMJsonObjectCreate();
            TMJsonObjectSetName(&a, "A");
            TMJsonObjectSetValue(&a, entity->color.w);
            TMJsonObjectAddChild(&jsonColor, &r);
            TMJsonObjectAddChild(&jsonColor, &g);
            TMJsonObjectAddChild(&jsonColor, &b);
            TMJsonObjectAddChild(&jsonColor, &a);
            
            // Texture
            TMJsonObject jsonAbsUVs = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonAbsUVs, "AbsUvs");
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[0]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[1]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[2]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[3]);
            TMJsonObject jsonRelUVs = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonRelUVs, "RelUvs");
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[0]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[1]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[2]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[3]);

            TMJsonObjectAddChild(&jsonGraphic, &jsonType);
            TMJsonObjectAddChild(&jsonGraphic, &jsonPosition);
            TMJsonObjectAddChild(&jsonGraphic, &jsonSize);
            TMJsonObjectAddChild(&jsonGraphic, &jsonColor);
            TMJsonObjectAddChild(&jsonGraphic, &jsonAbsUVs);
            TMJsonObjectAddChild(&jsonGraphic, &jsonRelUVs);
        
            TMJsonObjectAddChild(&jsonEntity, &jsonGraphic);

        }

        TMJsonObjectAddChild(&jsonScene, &jsonEntity);

    }

    TMJsonObjectAddChild(&jsonRoot, &jsonScene);

    int bytesCount = 0;
    TMJsonObjectStringify(&jsonRoot, NULL, &bytesCount);
    
    char *buffer = (char *)malloc(bytesCount + 1);
    int bytesWriten = 0;
    TMJsonObjectStringify(&jsonRoot, buffer, &bytesWriten);
    printf("%s", buffer);
    
    TMFileWriteText("../../assets/json/testScene.json", buffer, bytesWriten);
    free(buffer);

    TMJsonObjectFree(&jsonRoot);
}

int main() {

    TMWindow *window = TMWindowCreate(1280, 720, "TMEditor");

    TMRenderer *renderer = TMRendererCreate(window);

    gEditorState.spriteShader = TMRendererShaderCreate(renderer,
                                                       "../../assets/shaders/defaultVert.hlsl",
                                                       "../../assets/shaders/spriteFrag.hlsl");

    gEditorState.colorShader = TMRendererShaderCreate(renderer,
                                                      "../../assets/shaders/defaultVert.hlsl",
                                                      "../../assets/shaders/colorFrag.hlsl");

    TMDebugRendererInitialize(renderer, 100);

    TMUIInitialize(renderer, MetersToPixel);
    int width = TMRendererGetWidth(renderer);
    int height = TMRendererGetHeight(renderer);
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    ConstBuffer constBuffer{};
    constBuffer.view = TMMat4LookAt(gCameraPos, gCameraPos + tar, up);
    constBuffer.proj = TMMat4Ortho(0, width/MetersToPixel, 0, height/MetersToPixel, 0.1f, 100.0f);
    constBuffer.world = TMMat4Identity();
    gEditorState.shaderBuffer = TMRendererShaderBufferCreate(renderer, &constBuffer, sizeof(ConstBuffer), 0);

    gEditorState.vertexBuffer = TMRendererBufferCreate(renderer,
                                                       vertices, ARRAY_LENGTH(vertices),
                                                       indices, ARRAY_LENGTH(indices),
                                                       gEditorState.colorShader);

    const char *images[] = {
        "../../assets/images/moon.png",
        "../../assets/images/paddle_1.png",
        "../../assets/images/characters_packed.png",
        "../../assets/images/clone.png",
        "../../assets/images/player.png",
        "../../assets/images/paddle_2.png",
        "../../assets/images/font.png"
    };
    
    int fontCount = 0;
    float *fontUVs = TMGenerateUVs(128, 64, 7, 9, &fontCount);

    TMHashmap *absUVs = TMHashmapCreate(sizeof(TMVec4));
    TMTexture *texture = TMRendererTextureCreateAtlas(renderer, images, ARRAY_LENGTH(images), 1024*2, 1024*2, absUVs);

    TMUIElement *options = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0, 2}, {4, 0.4}, {0.1, 0.1, 0.1, 1});
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL, " Textures ", {1, 1, 1, 1}, OptionSelected);
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL, " Colors ",   {1, 1, 1, 1}, OptionSelected);
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL, " Clear Brush ",   {1, 1, 1, 1}, ClearSelected);

    TMUIElement *Blocks = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.4f, 0.1f, 1});
    TMUIElementAddChildButton(Blocks, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(Blocks, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElement *child = TMUIElementGetChild(Blocks, 0);
    for(int i = 0; i < ARRAY_LENGTH(images); ++i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, texture, {0, 0, 1, 1}, Texture(absUVs, images[i]), ElementSelected);
    }
    child = TMUIElementGetChild(Blocks, 1);
    for(int i = ARRAY_LENGTH(images) - 1; i >= 0;  --i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, texture, {0, 0, 1, 1}, Texture(absUVs, images[i]), ElementSelected);
    }

    TMUIElement *Prefabs = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    child = TMUIElementGetChild(Prefabs, 0);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.1f + 0.1f*(float)i, 0, 0, 1}, ElementSelected);
    }
    child = TMUIElementGetChild(Prefabs, 1);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.1f + 0.1f*(float)i, 0, 1}, ElementSelected);
    }
    child = TMUIElementGetChild(Prefabs, 2);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0, 0.1f + 0.1f*(float)i, 1}, ElementSelected);
    }

    TMUIElement *Modify = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {8.8, 0}, {4, 1}, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildLabel(Modify, TM_UI_ORIENTATION_VERTICAL, " Scale ", {1, 1, 1, 1}, ScaleEntity);
    TMUIElementAddChildLabel(Modify, TM_UI_ORIENTATION_VERTICAL, " Translate ", {1, 1, 1, 1}, TranslateEntity);

TM_EXPORT TMUIElement *TMUIElementCreateLabel(TMUIOrientation orientation, TMVec2 position, TMVec2 size,
                                              const char *text, TMVec4 color,
                                              PFN_OnClick onCLick = NULL);
    
    TMUIElement *saveScene = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0.0f, (float)height/MetersToPixel - 0.25f}, {2.5, 0.25}, {0.1f, 0.1f, 0.1f, 1.0f});
    TMUIElementAddChildLabel(saveScene, TM_UI_ORIENTATION_VERTICAL, " Save Scene ", {1, 1, 1, 1}, SaveScene);

    // TODO: add a way to enable depth test for z-index
    TMRendererDepthTestDisable(renderer);

    while(!TMWindowShouldClose(window)) {
        TMWindowFlushEventQueue(window);

        if(!gMouseIsHot) {
            if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_RIGHT)) {
                gCameraPos = {
                    gCameraPos.x - (float)(TMInputMousePositionX() - TMInputMouseLastPositionX()) / MetersToPixel,
                    gCameraPos.y + (float)(TMInputMousePositionY() - TMInputMouseLastPositionY()) / MetersToPixel,
                    -1
                };
                constBuffer.view = TMMat4LookAt(gCameraPos, gCameraPos + tar, up);
                TMRendererShaderBufferUpdate(renderer, gEditorState.shaderBuffer, &constBuffer);
            }

        }
 
        TMVec3 pos = gCameraPos;
        TMUIElementProcessInput(options, pos.x, pos.y, (float)width, (float)height, MetersToPixel);
        if(gEditorState.option == 0) {
            TMUIElementProcessInput(Blocks, pos.x, pos.y, (float)width, (float)height, MetersToPixel);
        }
        else {
            TMUIElementProcessInput(Prefabs, pos.x, pos.y, (float)width, (float)height, MetersToPixel);
        }
        if(!gEditorState.element && gEditorState.selectedEntity) {
            TMUIElementProcessInput(Modify, pos.x, pos.y, (float)width, (float)height, MetersToPixel);
        }
        TMUIElementProcessInput(saveScene, pos.x, pos.y, (float)width, (float)height, MetersToPixel);

        gMouseIsHot = false;
        TMUIMouseIsHot(options,   &gMouseIsHot);
        TMUIMouseIsHot(Blocks,    &gMouseIsHot);
        TMUIMouseIsHot(Prefabs,   &gMouseIsHot);
        TMUIMouseIsHot(saveScene, &gMouseIsHot);
        if(!gEditorState.element && gEditorState.selectedEntity) {
            TMUIMouseIsHot(Modify, &gMouseIsHot);
        }

        float mouseX;
        float mouseY;
        MouseToWorld(&mouseX, &mouseY, width, height);
        printf("mouse X: %f Y: %f\n", mouseX, mouseY);


        if(!gMouseIsHot && TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT) && gEditorState.element) {
            AddEntity(mouseX, mouseY);
        }

        if(!gMouseIsHot && !gEditorState.element && gEditorState.entities) {
            if(TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT)) {
                for(int i = 0; i < TMDarraySize(gEditorState.entities); ++i) {

                    Entity *entity = gEditorState.entities + i;
                    float minX = entity->position.x - entity->size.x*0.5f;
                    float maxX = minX + entity->size.x;
                    float minY = entity->position.y - entity->size.y*0.5f;
                    float maxY = minY + entity->size.y;

                    float mouseX;
                    float mouseY;
                    MouseToWorld(&mouseX, &mouseY, width, height);

                    if(mouseX > minX && mouseX <= maxX &&
                       mouseY > minY && mouseY <= maxY) {
                        gEditorState.selectedEntity = entity;
                    }

                }
            }
        }

        if(!gMouseIsHot && gEditorState.selectedEntity && TMInputMousButtonIsDown(TM_MOUSE_BUTTON_LEFT)) {
            float mouseX, mouseY;
            float lastMouseX, lastMouseY;
            MouseToWorld(&mouseX, &mouseY, width, height);
            LastMouseToWorld(&lastMouseX, &lastMouseY, width, height);
            float offsetX = mouseX - lastMouseX;
            float offsetY = mouseY - lastMouseY;

            if(gEditorState.modifyOption == MODIFY_TRANSLATE) {
                Entity *entity = gEditorState.selectedEntity;
                entity->position.x += offsetX;
                entity->position.y += offsetY;
            }
            else if (gEditorState.modifyOption == MODIFY_SCALE){
                Entity *entity = gEditorState.selectedEntity;
                entity->size.x += offsetX;
                entity->size.y += offsetY;
                entity->position.x += offsetX*0.5f;
                entity->position.y += offsetY*0.5f;
            }
        }


        TMRendererClear(renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);
 
        for(int y = 0; y < (height/MetersToPixel) + 2; ++y) {
            int offsetY = (int)pos.y;
            TMDebugRendererDrawLine(0 + pos.x, y  + offsetY, width/MetersToPixel + pos.x, y + offsetY, 0xFF666666);
        }
        for(int x = 0; x < (width/MetersToPixel) + 2; ++x) {
            int offsetX = (int)pos.x;
            TMDebugRendererDrawLine(x + offsetX, 0 + pos.y, x + offsetX, height/MetersToPixel + pos.y, 0xFF666666);
        }
 
        if(gEditorState.entities) {
            for(int i = 0; i < TMDarraySize(gEditorState.entities); ++i) {
                Entity *entity = gEditorState.entities + i;
                TMRendererBindShader(renderer, entity->shader);
                if(entity->texture) TMRendererTextureBind(renderer, entity->texture, entity->shader, "uTexture", 0);

                    TMMat4 trans = TMMat4Translate(entity->position.x, entity->position.y, 2.0f);
                    TMMat4 scale = TMMat4Scale(entity->size.x, entity->size.y, 1.0f);
                    constBuffer.world = trans * scale;
                    constBuffer.color = entity->color;
                    constBuffer.absUVs = entity->absUVs;
                    constBuffer.relUVs = entity->relUVs;
                    TMRendererShaderBufferUpdate(renderer, gEditorState.shaderBuffer, &constBuffer);
                    TMRendererDrawBufferElements(renderer, gEditorState.vertexBuffer);
            }
        }


        if(gEditorState.selectedEntity) {
            Entity *entity = gEditorState.selectedEntity;
            TMDebugRendererDrawQuad(entity->position.x, entity->position.y, entity->size.x, entity->size.y, 0, 0xFF00FF00);
        }

        TMDebugRenderDraw();

        TMUIElementDraw(renderer, options, 0.0f);
        TMUIElementDraw(renderer, saveScene, 0.0f);

        if(gEditorState.option == 0) {
            TMUIElementDraw(renderer, Blocks, 0.0f);
        }
        else {
            TMUIElementDraw(renderer, Prefabs, 0.0f);
        }

        if(!gEditorState.element && gEditorState.selectedEntity) {
            TMUIElementDraw(renderer, Modify, 0.0f);
        }

        if(gEditorState.element) {

            TMUIElement *viewport = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {10.8, 0}, {2, 2}, {0.1f, 0.1f, 0.1f, 1});
            if(gEditorState.element->type == TM_UI_TYPE_IMAGE_BUTTON) {
                TMUIElement *element = gEditorState.element;
                TMUIElementAddChildImageButton(viewport, TM_UI_ORIENTATION_VERTICAL, texture, element->absUVs, element->relUVs);
            }
            else if (gEditorState.element->type == TM_UI_TYPE_BUTTON) {
                TMUIElement *element = gEditorState.element;
                TMUIElementAddChildButton(viewport, TM_UI_ORIENTATION_HORIZONTAL, element->oldColor);
            }
            TMUIElementDraw(renderer, viewport, 0.0f);
            TMUIElementDestroy(viewport);

        }

        TMRendererPresent(renderer, 1);
        TMWindowPresent(window);
    }

    TMUIShutdown(renderer);
    TMDebugRendererShutdown();
    free(fontUVs);
    TMUIElementDestroy(saveScene);
    TMUIElementDestroy(options);
    TMHashmapDestroy(absUVs);
    TMRendererTextureDestroy(renderer, texture);
    TMRendererBufferDestroy(renderer, gEditorState.vertexBuffer);
    TMRendererShaderBufferDestroy(renderer, gEditorState.shaderBuffer);
    TMRendererShaderDestroy(renderer, gEditorState.spriteShader);
    TMRendererShaderDestroy(renderer, gEditorState.colorShader);
    TMRendererDestroy(renderer);
    TMWindowDestroy(window);

    return 0;
}
