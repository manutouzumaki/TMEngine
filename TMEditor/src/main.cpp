#include <tm_defines.h>
#include <tm_window.h>
#include <tm_renderer.h>
#include <tm_ui.h>
#include <utils/tm_hashmap.h>
#include <utils/tm_math.h>
#include <tm_input.h>
#include <stdio.h>

struct ShaderMatrix {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
};

struct EditorState {
    int option;
    TMUIElement *element;
};

static float MetersToPixel = 100;
static EditorState editorState;

TMVec4 Texture(TMHashmap *hashmap, const char *filepath) {
    TMVec4 result = *((TMVec4 *)TMHashmapGet(hashmap, filepath));
    return result; 
}


void OptionSelected(TMUIElement *element) {
    printf("Option selected\n");
    editorState.option = element->index;
}

void ElementSelected(TMUIElement *element) {
    printf("Element selected\n");
    editorState.element = element;
}

void Stub(TMUIElement *element) {}

int main() {

    TMWindow *window = TMWindowCreate(1280, 720, "TMEditor");

    TMRenderer *renderer = TMRendererCreate(window);

    TMShader *spriteShader = TMRendererShaderCreate(renderer,
                                                    "../../assets/shaders/defaultVert.hlsl",
                                                    "../../assets/shaders/spriteFrag.hlsl");

    TMShader *colorShader = TMRendererShaderCreate(renderer,
                                                   "../../assets/shaders/defaultVert.hlsl",
                                                   "../../assets/shaders/colorFrag.hlsl");

    TMUIInitialize(renderer, colorShader, MetersToPixel);


    int width = TMRendererGetWidth(renderer);
    int height = TMRendererGetHeight(renderer);
    TMVec3 pos = {0, 0, 0};
    TMVec3 tar = {0, 0, 1};
    TMVec3 up  = {0, 1, 0};
    TMMat4 view = TMMat4LookAt(pos, pos + tar, up);
    TMMat4 proj = TMMat4Ortho(0, width/MetersToPixel, 0, height/MetersToPixel, 0.1f, 100.0f);

    ShaderMatrix mats{};
    mats.proj = proj;
    mats.view = view;
    mats.world = TMMat4Identity();
    TMShaderBuffer *shaderBuffer = TMRendererShaderBufferCreate(renderer, &mats, sizeof(ShaderMatrix), 0);

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

    TMUIElement *options = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0, 2}, {3, 0.4}, {0.1, 0.1, 0.1, 1}, colorShader, NULL);
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL, " Textures ", {1, 1, 1, 1}, spriteShader, OptionSelected);
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL, " Colors ",   {1, 1, 1, 1},  spriteShader, OptionSelected);

    TMUIElement *Blocks = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.4f, 0.1f, 1}, colorShader, NULL);
    TMUIElementAddChildButton(Blocks, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, colorShader, NULL);
    TMUIElementAddChildButton(Blocks, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, colorShader, NULL);
    TMUIElement *child = TMUIElementGetChild(Blocks, 0);
    for(int i = 0; i < ARRAY_LENGTH(images); ++i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, texture, {0, 0, 1, 1}, Texture(absUVs, images[i]), spriteShader, ElementSelected);
    }
    child = TMUIElementGetChild(Blocks, 1);
    for(int i = ARRAY_LENGTH(images) - 1; i >= 0;  --i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, texture, {0, 0, 1, 1}, Texture(absUVs, images[i]), spriteShader, ElementSelected);
    }


    TMUIElement *Prefabs = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1}, colorShader, NULL);
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, colorShader, NULL);
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, colorShader, NULL);
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, colorShader, NULL);
    child = TMUIElementGetChild(Prefabs, 0);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.1f + 0.1f*(float)i, 0, 0, 1}, colorShader, ElementSelected);
    }
    child = TMUIElementGetChild(Prefabs, 1);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.1f + 0.1f*(float)i, 0, 1}, colorShader, ElementSelected);
    }
    child = TMUIElementGetChild(Prefabs, 2);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0, 0.1f + 0.1f*(float)i, 1}, colorShader, ElementSelected);
    }


    TMRendererDepthTestEnable(renderer);

    while(!TMWindowShouldClose(window)) {
        TMWindowFlushEventQueue(window);

        // TODO: enable this when implementing world scrolling
#if 0
        if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_LEFT)) {
            pos = {
                pos.x - (float)(TMInputMousePositionX() - TMInputMouseLastPositionX()) / MetersToPixel,
                pos.y + (float)(TMInputMousePositionY() - TMInputMouseLastPositionY()) / MetersToPixel,
                0
            };
            view = TMMat4LookAt(pos, pos + tar, up);
            mats.view = view;
            TMRendererShaderBufferUpdate(renderer, shaderBuffer, &mats);
        }
#endif
        
        TMUIElementProcessInput(options, pos.x, pos.y, (float)width, (float)height);
        if(editorState.option == 0) {
            TMUIElementProcessInput(Blocks, pos.x, pos.y, (float)width, (float)height);
        }
        else {
            TMUIElementProcessInput(Prefabs, pos.x, pos.y, (float)width, (float)height);
        }

        TMRendererClear(renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);

        TMUIElementDraw(renderer, options, 0.0f);

        if(editorState.option == 0) {
            TMUIElementDraw(renderer, Blocks, 0.0f);
        }
        else {
            TMUIElementDraw(renderer, Prefabs, 0.0f);
        }

        if(editorState.element) {

            TMUIElement *viewport = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {10.8, 0}, {2, 2}, {0.1f, 0.1f, 0.1f, 1}, colorShader, NULL);
            if(editorState.element->type == TM_UI_TYPE_IMAGE_BUTTON) {
                TMUIElement *element = editorState.element;
                TMUIElementAddChildImageButton(viewport, TM_UI_ORIENTATION_VERTICAL, texture, element->absUVs, element->relUVs, spriteShader, NULL);
            }
            else if (editorState.element->type == TM_UI_TYPE_BUTTON) {
                TMUIElement *element = editorState.element;
                TMUIElementAddChildButton(viewport, TM_UI_ORIENTATION_HORIZONTAL, element->oldColor, colorShader, NULL);
            }
            TMUIElementDraw(renderer, viewport, 0.0f);
            TMUIElementDestroy(viewport);

        }


        TMRendererPresent(renderer, 1);
        TMWindowPresent(window);
    }

    TMUIShutdown(renderer);
    free(fontUVs);
    TMUIElementDestroy(options);
    TMHashmapDestroy(absUVs);
    TMRendererTextureDestroy(renderer, texture);
    TMRendererShaderDestroy(renderer, spriteShader);
    TMRendererShaderDestroy(renderer, colorShader);
    TMRendererDestroy(renderer);
    TMWindowDestroy(window);

    return 0;
}
