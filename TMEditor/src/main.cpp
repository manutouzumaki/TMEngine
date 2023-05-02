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
};

static float MetersToPixel = 100;
static EditorState editorState;

TMVec4 Texture(TMHashmap *hashmap, const char *filepath) {
    TMVec4 result = *((TMVec4 *)TMHashmapGet(hashmap, filepath));
    return result; 
}


void OptionSelected(int index, TMVec4 vec4) {
    editorState.option = index;
}

void Stub(int index, TMVec4 vec4) {

}

int main() {

    TMWindow *window = TMWindowCreate(1280, 720, "TMEditor");

    TMRenderer *renderer = TMRendererCreate(window);

    TMShader *shader = TMRendererShaderCreate(renderer,
                                              "../../assets/shaders/batchVert.hlsl",
                                              "../../assets/shaders/batchFrag.hlsl");

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

    TMRenderBatch *renderBatch = TMRendererRenderBatchCreate(renderer, shader, texture, 100);

    TMUIElement *options = TMUIElementCreate({0, 2}, {3, 0.4}, {0.1f, 0.1f, 0.1f, 1}, TM_UI_ORIENTATION_HORIZONTAL,   TM_UI_TYPE_BUTTON, renderBatch);
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL,
                             Texture(absUVs, "../../assets/images/font.png"), fontUVs,
                             " Textures ",
                             renderBatch, OptionSelected);
    TMUIElementAddChildLabel(options, TM_UI_ORIENTATION_VERTICAL,
                             Texture(absUVs, "../../assets/images/font.png"), fontUVs,
                             " Colors ",
                             renderBatch, OptionSelected);


    TMUIElement *Blocks = TMUIElementCreate({0, 0}, {6, 2}, {0.1f, 0.4f, 0.1f, 1}, TM_UI_ORIENTATION_VERTICAL, TM_UI_TYPE_BUTTON, renderBatch);
    TMUIElementAddChildButton(Blocks, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, renderBatch, Stub);
    TMUIElementAddChildButton(Blocks, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, renderBatch, Stub);
    TMUIElement *child = TMUIElementGetChild(Blocks, 0);
    for(int i = 0; i < ARRAY_LENGTH(images); ++i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, Texture(absUVs, images[i]), renderBatch, Stub);
    }
    child = TMUIElementGetChild(Blocks, 1);
    for(int i = ARRAY_LENGTH(images) - 1; i >= 0;  --i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, Texture(absUVs, images[i]), renderBatch, Stub);
    }


    TMUIElement *Prefabs = TMUIElementCreate({0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1}, TM_UI_ORIENTATION_VERTICAL, TM_UI_TYPE_BUTTON, renderBatch);
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, renderBatch, Stub);
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, renderBatch, Stub);
    TMUIElementAddChildButton(Prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1}, renderBatch, Stub);
    child = TMUIElementGetChild(Prefabs, 0);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.1f + 0.1f*(float)i, 0, 0, 1}, renderBatch, Stub);
    }
    child = TMUIElementGetChild(Prefabs, 1);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.1f + 0.1f*(float)i, 0, 1}, renderBatch, Stub);
    }
    child = TMUIElementGetChild(Prefabs, 2);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0, 0.1f + 0.1f*(float)i, 1}, renderBatch, Stub);
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
        
        TMUIElementProcessInput(options, pos.x, pos.y, (float)width, (float)height, proj, view);
        if(editorState.option == 0) {
            TMUIElementProcessInput(Blocks, pos.x, pos.y, (float)width, (float)height, proj, view);
        }
        else {
            TMUIElementProcessInput(Prefabs, pos.x, pos.y, (float)width, (float)height, proj, view);
        }


        TMRendererClear(renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);

        TMUIElementDraw(options, 0.0f);
        if(editorState.option == 0) {
            TMUIElementDraw(Blocks, 0.0f);
        }
        else {
            TMUIElementDraw(Prefabs, 0.0f);
        }

        TMRendererRenderBatchDraw(renderBatch);


        TMRendererPresent(renderer, 1);
        TMWindowPresent(window);
    }

    free(fontUVs);
    TMUIElementDestroy(options);
    TMHashmapDestroy(absUVs);
    TMRendererRenderBatchDestroy(renderer, renderBatch);
    TMRendererTextureDestroy(renderer, texture);
    TMRendererShaderDestroy(renderer, shader);
    TMRendererDestroy(renderer);
    TMWindowDestroy(window);

    return 0;
}
