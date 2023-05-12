#include "editor.h"
#include <tm_window.h>

struct ConstBuffer {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
};

static ConstBuffer gConstBuffer;

static TMVertex gVertices[] = {
        TMVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec3{0, 0, 0}}, // 0
        TMVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}, TMVec3{0, 0, 0}}, // 1
        TMVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec3{0, 0, 0}}, // 2
        TMVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}, TMVec3{0, 0, 0}}  // 3
};

static unsigned int gIndices[] = {
        1, 0, 2, 2, 0, 3
};


EditorState *gState;
const char *gImages[] = {
    "../../assets/images/moon.png",
    "../../assets/images/paddle_1.png",
    "../../assets/images/characters_packed.png",
    "../../assets/images/clone.png",
    "../../assets/images/player.png",
    "../../assets/images/paddle_2.png",
    "../../assets/images/font.png"
};
float     *gFontUVs;
TMHashmap *gAbsUVs;
TMTexture *gTexture;
int gFontCount;
static int gEntityCount;


static void AddEntity(EditorState *state, float posX, float posY) {
    printf("Entity added\n");
    TMUIElement *element = state->element;
    Entity entity = {};
    entity.color = element->oldColor;
    entity.absUVs = element->absUVs;
    entity.relUVs = element->relUVs;
    entity.position = {floorf(posX) + 0.5f, floorf(posY) + 0.5f};
    entity.size = {1, 1};
    entity.texture = element->texture;
    entity.zIndex = 2;
    entity.id = gEntityCount++;
    if(element->type == TM_UI_TYPE_BUTTON) {
        entity.shader = state->colorShader;
    }
    else {
        entity.shader = state->spriteShader;
    }
    TMDarrayPush(state->entities, entity, Entity);

}

static void MouseToWorld(float *mouseX, float *mouseY, float width, float height, float meterToPixel) {

    float x = (float)TMInputMousePositionX();
    float y = height - (float)TMInputMousePositionY();

    float worldMouseX = (x / width)  * (width/meterToPixel);
    float worldMouseY = (y / height) * (height/meterToPixel);

    *mouseX = worldMouseX + gState->cameraP.x;
    *mouseY = worldMouseY + gState->cameraP.y;

}

static void LastMouseToWorld(float *mouseX, float *mouseY, float width, float height, float meterToPixel) {

    float x = (float)TMInputMouseLastPositionX();
    float y = height - (float)TMInputMouseLastPositionY();

    float worldMouseX = (x / width)  * (width/meterToPixel);
    float worldMouseY = (y / height) * (height/meterToPixel);

    *mouseX = worldMouseX + gState->cameraP.x;
    *mouseY = worldMouseY + gState->cameraP.y;

}

static void InsertionSortEntities(Entity *entities, int length)
{
    for(int j = 1; j < length; ++j)
    {
        Entity key = entities[j];
        int i = j - 1;
        while(i > -1 && entities[i].zIndex < key.zIndex)
        {
            entities[i + 1] = entities[i];
            --i;
        }
        entities[i + 1] = key;
    }
}

void EditorInitialize(EditorState *state, TMWindow *window) {
    // TODO: remove this ...
    gState = state;
    
    state->window = window;
    state->meterToPixel = 100.0f;
    state->renderer = TMRendererCreate(window);

    state->spriteShader = TMRendererShaderCreate(state->renderer,
                                                 "../../assets/shaders/defaultVert.hlsl",
                                                 "../../assets/shaders/spriteFrag.hlsl");

    state->colorShader = TMRendererShaderCreate(state->renderer,
                                                "../../assets/shaders/defaultVert.hlsl",
                                                "../../assets/shaders/colorFrag.hlsl");

    TMDebugRendererInitialize(state->renderer, 100);
    TMUIInitialize(state->renderer, state->meterToPixel);
    
    // Initialize ConstBuffer data
    int clientWidth = TMRendererGetWidth(state->renderer);
    int clientHeight = TMRendererGetHeight(state->renderer);
    TMVec3 target = {0, 0, 1};
    TMVec3 up = {0, 1, 0};
    gConstBuffer.view = TMMat4LookAt(state->cameraP, state->cameraP + target, up);
    gConstBuffer.proj = TMMat4Ortho(0, clientWidth/state->meterToPixel,
                                    0, clientHeight/state->meterToPixel,
                                    0.1f, 100.0f);
    gConstBuffer.world = TMMat4Identity();

    // create the shader buffer to store the ConstBuffer on the GPU
    state->shaderBuffer = TMRendererShaderBufferCreate(state->renderer, &gConstBuffer,
                                                       sizeof(ConstBuffer), 0);
    // create the buffer to store the vertices on the GPU
    state->vertexBuffer = TMRendererBufferCreate(state->renderer,
                                                 gVertices, ARRAY_LENGTH(gVertices),
                                                 gIndices, ARRAY_LENGTH(gIndices),
                                                 state->colorShader);

    // Initialize texture atlas
    // TODO: make a system to add and remove textures on the fly
    gFontUVs = TMGenerateUVs(128, 64, 7, 9, &gFontCount);
    gAbsUVs = TMHashmapCreate(sizeof(TMVec4));
    gTexture = TMRendererTextureCreateAtlas(state->renderer, gImages, ARRAY_LENGTH(gImages), 1024*2, 1024*2, gAbsUVs);

    EditorUIInitialize(&state->ui, (float)clientWidth, (float)clientHeight, state->meterToPixel);

}

void EditorUpdate(EditorState *state) {
    float clientWidth  = (float)TMRendererGetWidth(state->renderer);
    float clientHeight = (float)TMRendererGetHeight(state->renderer);
    EditorUIUpdate(&state->ui, clientWidth, clientHeight, state->meterToPixel);


    if(!state->mouseIsHot) {
        if(TMInputMousButtonJustDown(TM_MOUSE_BUTTON_RIGHT)) {
            TMMouseSetCapture(state->window);
        }
        if(TMInputMousButtonJustUp(TM_MOUSE_BUTTON_RIGHT)) { 
            TMMouseReleaseCapture();
        }
        if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_RIGHT)) {

            state->cameraP = {
                state->cameraP.x - (float)(TMInputMousePositionX() - TMInputMouseLastPositionX()) / state->meterToPixel,
                state->cameraP.y + (float)(TMInputMousePositionY() - TMInputMouseLastPositionY()) / state->meterToPixel,
                -1
            };
            TMVec3 target = {0, 0, 1};
            TMVec3 up = {0, 1, 0};
            gConstBuffer.view = TMMat4LookAt(state->cameraP, state->cameraP + target, up);
            TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &gConstBuffer);
        }

    }


    if(!state->mouseIsHot && TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT) && state->element) {
        float mouseX;
        float mouseY;
        MouseToWorld(&mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);
        AddEntity(state, mouseX, mouseY);
    }

    if(!state->mouseIsHot && !state->element && state->entities) {
        if(TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT)) {
            for(int i = 0; i < TMDarraySize(state->entities); ++i) {

                Entity *entity = state->entities + i;
                float minX = entity->position.x - entity->size.x*0.5f;
                float maxX = minX + entity->size.x;
                float minY = entity->position.y - entity->size.y*0.5f;
                float maxY = minY + entity->size.y;

                float mouseX;
                float mouseY;
                MouseToWorld(&mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);

                if(mouseX > minX && mouseX <= maxX &&
                   mouseY > minY && mouseY <= maxY) {
                    state->selectedEntity = entity;
                }

            }
        }
    }

    if(!state->mouseIsHot && state->selectedEntity && TMInputMousButtonIsDown(TM_MOUSE_BUTTON_LEFT)) {
        float mouseX, mouseY;
        float lastMouseX, lastMouseY;
        MouseToWorld(&mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);
        LastMouseToWorld(&lastMouseX, &lastMouseY, clientWidth, clientHeight, state->meterToPixel);
        float offsetX = mouseX - lastMouseX;
        float offsetY = mouseY - lastMouseY;

        Entity *entity = state->selectedEntity;
        if(state->modifyOption == MODIFY_TRANSLATE) {
            entity->position.x += offsetX;
            entity->position.y += offsetY;
        }
        else if (state->modifyOption == MODIFY_SCALE){
            entity->size.x += offsetX;
            entity->size.y += offsetY;
            entity->position.x += offsetX*0.5f;
            entity->position.y += offsetY*0.5f;
        }
        else if(state->modifyOption == MODIFY_INC_REL_U) {
            entity->relUVs.z -= offsetX*0.02f;
        }
        else if(state->modifyOption == MODIFY_INC_REL_V) {
            entity->relUVs.w += offsetY*0.02f;
        }
        else if(state->modifyOption == MODIFY_OFF_REL_U) {
            entity->relUVs.x += offsetX*0.02f;
            entity->relUVs.z += offsetX*0.02f;
        }
        else if(state->modifyOption == MODIFY_OFF_REL_V) {
            entity->relUVs.y += offsetY*0.02f;
            entity->relUVs.w += offsetY*0.02f;
        }
        else if(state->modifyOption == MODIFY_INC_ABS_U) {
            entity->absUVs.z -= offsetX*0.08f;
        }
        else if(state->modifyOption == MODIFY_INC_ABS_V) {
            entity->absUVs.w += offsetY*0.08f;
        }
        else if(state->modifyOption == MODIFY_OFF_ABS_U) {
            entity->absUVs.x += offsetX*0.02f;
            entity->absUVs.z += offsetX*0.02f;
        }
        else if(state->modifyOption == MODIFY_OFF_ABS_V) {
            entity->absUVs.y += offsetY*0.02f;
            entity->absUVs.w += offsetY*0.02f;
        }
    }

    // TODO: clean this up ...
    if(state->entities) {
        int id = 0;
        if(state->selectedEntity) {
            id = state->selectedEntity->id;
        }
        InsertionSortEntities(state->entities, TMDarraySize(state->entities));
        if(state->selectedEntity) {
            for(int i = 0; i < TMDarraySize(state->entities); ++i) {
                Entity *entity = state->entities + i;
                if(id == entity->id) {
                    state->selectedEntity = entity;
                }
            }
        }
    }
    // this is a test for a github problem when trying to commit my changes ...

}

void EditorRender(EditorState *state) {
    TMRendererClear(state->renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);

    int width  = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);


    TMRendererDepthTestEnable(state->renderer);

    TMVec3 pos = state->cameraP;
    for(int y = 0; y < (height/state->meterToPixel) + 2; ++y) {
        int offsetY = (int)pos.y;
        TMDebugRendererDrawLine(0 + pos.x, y  + offsetY, width/state->meterToPixel + pos.x, y + offsetY, 0xFF666666);
    }
    for(int x = 0; x < (width/state->meterToPixel) + 2; ++x) {
        int offsetX = (int)pos.x;
        TMDebugRendererDrawLine(x + offsetX, 0 + pos.y, x + offsetX, height/state->meterToPixel + pos.y, 0xFF666666);
    }

    if(state->entities) {
        for(int i = 0; i < TMDarraySize(state->entities); ++i) {
            Entity *entity = state->entities + i;
            TMRendererBindShader(state->renderer, entity->shader);
            if(entity->texture) TMRendererTextureBind(state->renderer, entity->texture, entity->shader, "uTexture", 0);

                TMMat4 trans = TMMat4Translate(entity->position.x, entity->position.y, entity->zIndex);
                TMMat4 scale = TMMat4Scale(entity->size.x, entity->size.y, 1.0f);
                gConstBuffer.world = trans * scale;
                gConstBuffer.color = entity->color;
                gConstBuffer.absUVs = entity->absUVs;
                gConstBuffer.relUVs = entity->relUVs;
                TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &gConstBuffer);
                TMRendererDrawBufferElements(state->renderer, state->vertexBuffer);
        }
    }


    TMRendererDepthTestDisable(state->renderer);

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        TMDebugRendererDrawQuad(entity->position.x, entity->position.y, entity->size.x, entity->size.y, 0, 0xFF00FF00);
    }

    TMDebugRenderDraw();

    EditorUIDraw(&state->ui, state->renderer);

    TMRendererPresent(state->renderer, 1);
}

void EditorShutdown(EditorState *state) {
    if(state->entities) TMDarrayDestroy(state->entities);
    EditorUIShutdown(&state->ui);
    TMRendererTextureDestroy(state->renderer, gTexture);
    TMHashmapDestroy(gAbsUVs);
    free(gFontUVs);
    TMRendererBufferDestroy(state->renderer, state->vertexBuffer);
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMUIShutdown(state->renderer);
    TMDebugRendererShutdown();
    TMRendererShaderDestroy(state->renderer, state->colorShader);
    TMRendererShaderDestroy(state->renderer, state->spriteShader);
    TMRendererDestroy(state->renderer);

    printf("Shuting down !!!\n");

}




