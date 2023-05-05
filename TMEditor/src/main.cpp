#include <tm_defines.h>
#include <tm_window.h>
#include <tm_renderer.h>
#include <tm_ui.h>
#include <tm_debug_renderer.h>
#include <utils/tm_hashmap.h>
#include <utils/tm_math.h>
#include <utils/tm_darray.h>
#include <tm_input.h>
#include <stdio.h>

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


struct EditorState {
    int option;
    TMUIElement *element;
    Entity *entities;

    TMBuffer *vertexBuffer;
    TMShaderBuffer *shaderBuffer;
    TMShader *colorShader;
    TMShader *spriteShader;
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

static TMVec3 pos = {0, 0, 0};
static bool gMouseIsHot;

TMVec4 Texture(TMHashmap *hashmap, const char *filepath) {
    TMVec4 result = *((TMVec4 *)TMHashmapGet(hashmap, filepath));
    return result; 
}


void OptionSelected(TMUIElement *element) {
    printf("Option selected\n");
    gEditorState.option = element->index;
}

void ElementSelected(TMUIElement *element) {
    printf("Element selected\n");
    gEditorState.element = element;
}

void AddEntity(float posX, float posY) {
    printf("Entity added\n");
    TMUIElement *element = gEditorState.element;
    Entity entity = {};
    entity.color = element->oldColor;
    entity.absUVs = element->absUVs;
    entity.relUVs = element->relUVs;
    entity.position = {posX, posY};
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
    TMMat4 view = TMMat4LookAt(pos, pos + tar, up);
    TMMat4 proj = TMMat4Ortho(0, width/MetersToPixel, 0, height/MetersToPixel, 0.1f, 100.0f);

    ConstBuffer constBuffer{};
    constBuffer.proj = proj;
    constBuffer.view = view;
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

    TMUIElement *options = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0, 2}, {3, 0.4}, {0.1, 0.1, 0.1, 1});
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL, " Textures ", {1, 1, 1, 1}, OptionSelected);
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL, " Colors ",   {1, 1, 1, 1}, OptionSelected);

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

    TMRendererDepthTestEnable(renderer);

    while(!TMWindowShouldClose(window)) {
        TMWindowFlushEventQueue(window);
 
        TMUIElementProcessInput(options, pos.x, pos.y, (float)width, (float)height);
        if(gEditorState.option == 0) {
            TMUIElementProcessInput(Blocks, pos.x, pos.y, (float)width, (float)height);
        }
        else {
            TMUIElementProcessInput(Prefabs, pos.x, pos.y, (float)width, (float)height);
        }

        gMouseIsHot = false;
        TMUIMouseIsHot(options, &gMouseIsHot);
        TMUIMouseIsHot(Blocks,  &gMouseIsHot);
        TMUIMouseIsHot(Prefabs, &gMouseIsHot);

        if(!gMouseIsHot && TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT) && gEditorState.element) {

            float mouseX = (float)TMInputMousePositionX();
            float mouseY = height - (float)TMInputMousePositionY();

            TMMat4 invView = TMMat4Inverse(constBuffer.view);
            TMMat4 invProj = TMMat4Inverse(constBuffer.proj);

            TMVec2 screenCoord = {mouseX, mouseY};
            TMVec2 viewportPos = {pos.x, pos.y};
            TMVec2 viewportSize = {(float)width, (float)height};
            TMVec2 normalizeP = (screenCoord - viewportPos) / viewportSize;
            TMVec2 one = {1, 1};
            normalizeP = (normalizeP * 2.0f) - one;
            TMVec4 position = {normalizeP.x, normalizeP.y, 0 , 1};
            position = invView * invProj * position;

            mouseX = position.x;
            mouseY = position.y;

            AddEntity(mouseX, mouseY);
        }

        if(!gMouseIsHot) {
            if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_RIGHT)) {
                pos = {
                    pos.x - (float)(TMInputMousePositionX() - TMInputMouseLastPositionX()) / MetersToPixel,
                    pos.y + (float)(TMInputMousePositionY() - TMInputMouseLastPositionY()) / MetersToPixel,
                    0
                };
                view = TMMat4LookAt(pos, pos + tar, up);
                constBuffer.view = view;
                TMRendererShaderBufferUpdate(renderer, gEditorState.shaderBuffer, &constBuffer);
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


        TMUIElementDraw(renderer, options, 0.0f);

        if(gEditorState.option == 0) {
            TMUIElementDraw(renderer, Blocks, 0.0f);
        }
        else {
            TMUIElementDraw(renderer, Prefabs, 0.0f);
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


        TMDebugRenderDraw();
        TMRendererPresent(renderer, 1);
        TMWindowPresent(window);
    }

    TMUIShutdown(renderer);
    TMDebugRendererShutdown();
    free(fontUVs);
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
