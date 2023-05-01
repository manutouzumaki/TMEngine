#include <tm_defines.h>
#include <tm_window.h>
#include <tm_renderer.h>
#include <tm_ui.h>
#include <utils/tm_hashmap.h>
#include <utils/tm_math.h>
#include <tm_input.h>

struct ShaderMatrix {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
};

static float MetersToPixel = 100;

TMVec4 Texture(TMHashmap *hashmap, const char *filepath) {
    TMVec4 result = *((TMVec4 *)TMHashmapGet(hashmap, filepath));
    return result; 
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
        "../../assets/images/paddle_2.png"
    };

    TMHashmap *absUVs = TMHashmapCreate(sizeof(TMVec4));
    TMTexture *texture = TMRendererTextureCreateAtlas(renderer, images, ARRAY_LENGTH(images), 1024*2, 1024*2, absUVs);

    TMRenderBatch *renderBatch = TMRendererRenderBatchCreate(renderer, shader, texture, 100);

    TMUIElement *root = TMUIElementCreate({0, 0}, {3, 6}, {1, 0, 0, 1}, TM_UI_ORIENTATION_VERTICAL,   TM_UI_TYPE_BUTTON, renderBatch);

    TMUIElementAddChildButton(root, TM_UI_ORIENTATION_VERTICAL ,  {0, 1, 0, 0.5}, renderBatch);
    TMUIElementAddChildButton(root, TM_UI_ORIENTATION_VERTICAL ,  {1, 1, 0, 0.8}, renderBatch);
    TMUIElementAddChildButton(root, TM_UI_ORIENTATION_VERTICAL ,  {0, 1, 1, 0.2}, renderBatch);
    TMUIElementAddChildButton(root, TM_UI_ORIENTATION_HORIZONTAL, {0, 0, 1, 1},   renderBatch);
    TMUIElementAddChildButton(root, TM_UI_ORIENTATION_HORIZONTAL, {1, 0, 1, 1},   renderBatch);
    
    TMUIElement *child = TMUIElementGetChild(root, 1);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {1, 1, 0, 1},     renderBatch);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 1, 1, 1},     renderBatch);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.5, 1, 0.2, 1}, renderBatch);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {1, 0.2, 0.5, 1}, renderBatch);


    child = TMUIElementGetChild(root, 3);
    for(int i = 0; i < ARRAY_LENGTH(images); ++i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, Texture(absUVs, images[i]), renderBatch);
    }

    TMRendererDepthTestDisable(renderer);

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
        
        TMUIElementProcessInput(root, pos.x, pos.y, (float)width, (float)height, proj, view);


        TMRendererClear(renderer, 0.6, 0.6, 0.9, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);

        TMUIElementDraw(root);
        TMRendererRenderBatchDraw(renderBatch);


        TMRendererPresent(renderer, 1);
        TMWindowPresent(window);
    }

    TMUIElementDestroy(root);
    TMHashmapDestroy(absUVs);
    TMRendererRenderBatchDestroy(renderer, renderBatch);
    TMRendererTextureDestroy(renderer, texture);
    TMRendererShaderDestroy(renderer, shader);
    TMRendererDestroy(renderer);
    TMWindowDestroy(window);

    return 0;
}
