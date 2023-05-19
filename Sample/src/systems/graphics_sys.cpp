#include "../entity.h"
#include <utils/tm_darray.h>
#include <tm_renderer.h>
#include <tm_debug_renderer.h>
#include "../message.h"

struct ConstBuffer {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
};


struct GraphycsSystemState {
    TMBuffer       *vertexBuffer;
    TMShaderBuffer *shaderBuffer;
    TMShader       *colorShader;
    TMShader       *spriteShader;
    float meterToPixel;
    // TODO: this should be temporal
    TMTexture *texture;
};

// TODO: create a nice lit system for all this piace of shit code
static float       *gFontUVs;
static int          gFontCount;
static ConstBuffer  gConstBuffer;
static unsigned int gIndices[] = { 1, 0, 2, 2, 0, 3 };
static TMVertex     gVertices[] = {
        TMVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec3{0, 0, 0}}, // 0
        TMVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}, TMVec3{0, 0, 0}}, // 1
        TMVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec3{0, 0, 0}}, // 2
        TMVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}, TMVec3{0, 0, 0}}  // 3
};
static TMHashmap   *gAbsUVs;
static const char  *gImages[] = {
    "../../assets/images/moon.png",
    "../../assets/images/paddle_1.png",
    "../../assets/images/characters_packed.png",
    "../../assets/images/clone.png",
    "../../assets/images/player.png",
    "../../assets/images/paddle_2.png",
    "../../assets/images/font.png"
};



static GraphycsSystemState gGraphicsState;


static void GraphicsUpdatePositions(Entity *entity, TMVec2 position) {
    if(entity->graphics) {
        entity->graphics->position = position;
    }
}

void GraphicsSystemOnMessage(MessageType type, void *sender, void * listener, Message message) {

    switch(type) {
        case MESSAGE_TYPE_GRAPHICS_UPDATE_POSITIONS: {
            Entity *entity = (Entity *)sender;
            TMVec2 position = message.v2[0];
            GraphicsUpdatePositions(entity, position);
        } break;
        case MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX: {
            Entity *entity = (Entity *)sender;
            int index = message.i32[0];
            entity->graphics->index = index;
        }
    }

}

void GraphicsSystemInitialize(TMRenderer *renderer) {

    MessageRegister(MESSAGE_TYPE_GRAPHICS_UPDATE_POSITIONS, NULL, GraphicsSystemOnMessage);
    MessageRegister(MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX, NULL, GraphicsSystemOnMessage);

    gGraphicsState.meterToPixel = 100.0f;

    
    gGraphicsState.spriteShader = TMRendererShaderCreate(renderer,
                                                 "../../assets/shaders/defaultVert.hlsl",
                                                 "../../assets/shaders/spriteFrag.hlsl");
    gGraphicsState.colorShader  =  TMRendererShaderCreate(renderer,
                                                 "../../assets/shaders/defaultVert.hlsl",
                                                 "../../assets/shaders/colorFrag.hlsl");


    // create the shader buffer to store the ConstBuffer on the GPU
    gGraphicsState.shaderBuffer = TMRendererShaderBufferCreate(renderer, &gConstBuffer,
                                                       sizeof(ConstBuffer), 0);
    // create the buffer to store the vertices on the GPU
    gGraphicsState.vertexBuffer = TMRendererBufferCreate(renderer,
                                                 gVertices, ARRAY_LENGTH(gVertices),
                                                 gIndices, ARRAY_LENGTH(gIndices),
                                                 gGraphicsState.colorShader);

    gFontUVs = TMGenerateUVs(128, 64, 7, 9, &gFontCount);
    gAbsUVs = TMHashmapCreate(sizeof(TMVec4));
    gGraphicsState.texture = TMRendererTextureCreateAtlas(renderer, gImages, ARRAY_LENGTH(gImages), 1024*2, 1024*2, gAbsUVs);
    
}

void GraphicsSystemShutdown() {
    // TODO: FREEEE THE FUCKING MEMORYYYYYYY ....

}

void GraphicsSystemSetProjMatrix(TMRenderer *renderer, TMMat4 proj) {
    gConstBuffer.proj = proj;
    TMRendererShaderBufferUpdate(renderer, gGraphicsState.shaderBuffer, &gConstBuffer);
}

void GraphicsSystemSetViewMatrix(TMRenderer *renderer, TMMat4 view) {
    gConstBuffer.view = view;
    TMRendererShaderBufferUpdate(renderer, gGraphicsState.shaderBuffer, &gConstBuffer);
}

void GraphicsSystemSetWorldMatrix(TMRenderer *renderer, TMMat4 world) {
    gConstBuffer.world = world;
    TMRendererShaderBufferUpdate(renderer, gGraphicsState.shaderBuffer, &gConstBuffer);
}


//void GraphicsSystemDraw(TMRenderBatch *batchRenderer, Entity **entities) {
void GraphicsSystemDraw(TMRenderer *renderer, Entity **entities) {

    TMRendererDepthTestEnable(renderer);
    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            
#if 1
            if(graphics->type == GRAPHICS_TYPE_SOLID_COLOR) {
                TMRendererBindShader(renderer, gGraphicsState.colorShader);
                gConstBuffer.color = graphics->color; // Init color
            }
            else {
                TMRendererBindShader(renderer, gGraphicsState.spriteShader);
                TMRendererTextureBind(renderer, gGraphicsState.texture,
                                      gGraphicsState.spriteShader, "uTexture", 0);
                gConstBuffer.color = {1, 1, 1, 1}; // Init color
            }

            TMMat4 trans = TMMat4Translate(entity->graphics->position.x,
                                           entity->graphics->position.y,
                                           2); // TODO: add zIndex
            TMMat4 scale = TMMat4Scale(entity->graphics->size.x,
                                       entity->graphics->size.y,
                                       1.0f);
            gConstBuffer.world = trans * scale;
            gConstBuffer.absUVs = {0, 0, 1, 1};
            if(entity->graphics->relUVs) {
                gConstBuffer.relUVs = *((TMVec4 *)entity->graphics->relUVs);
            }
            TMRendererShaderBufferUpdate(renderer, gGraphicsState.shaderBuffer, &gConstBuffer);
            TMRendererDrawBufferElements(renderer, gGraphicsState.vertexBuffer);

#else

            switch(graphics->type) {
                case GRAPHICS_TYPE_SOLID_COLOR: {
                    TMRendererRenderBatchAdd(batchRenderer,
                                             graphics->position.x, graphics->position.y, 1,
                                             graphics->size.x, graphics->size.y, 0,
                                             graphics->color.x, graphics->color.y,
                                             graphics->color.z, graphics->color.w);
                } break;
                case GRAPHICS_TYPE_SPRITE: {
                    TMRendererRenderBatchAdd(batchRenderer, graphics->position.x, graphics->position.y, 1,
                                             graphics->size.x, graphics->size.y, 0, graphics->relUVs);
                } break;
                case GRAPHICS_TYPE_SUBSPRITE: {
                    TMRendererRenderBatchAdd(batchRenderer, graphics->position.x, graphics->position.y, 1,
                                             graphics->size.x, graphics->size.y, 0, graphics->absUVs, graphics->index, graphics->relUVs);
                } break;

            }
#endif




        }
    }

    TMRendererDepthTestDisable(renderer);
    //TMRendererRenderBatchDraw(batchRenderer);

#ifdef TM_DEBUG
    // draw debug geometry
    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->collision, entity->graphics) {
            CollisionComponent *collision = entity->collision;
            GraphicsComponent *graphics = entity->graphics;

            switch(collision->type) {
                    case COLLISION_TYPE_AABB: {
                        AABB aabb = collision->aabb;
                        float width = aabb.max.x - aabb.min.x;
                        float height = aabb.max.y - aabb.min.y;
                        float x = aabb.min.x + width*0.5f;
                        float y = aabb.min.y + height*0.5f;
                        
                        TMDebugRendererDrawQuad(graphics->position.x, graphics->position.y, width, height, 0, 0xFF00FF00);

                        TMDebugRendererDrawCircle(aabb.max.x, aabb.min.y, 0.4, 0xFFFFFF00, 20);
                        TMDebugRendererDrawCircle(aabb.min.x, aabb.max.y, 0.4, 0xFFFFFF00, 20);
                        TMDebugRendererDrawCircle(aabb.min.x, aabb.min.y, 0.4, 0xFFFFFF00, 20);
                        TMDebugRendererDrawCircle(aabb.max.x, aabb.max.y, 0.4, 0xFFFFFF00, 20);

                        aabb.min = {aabb.min.x - 0.4f, aabb.min.y - 0.4f};
                        aabb.max = {aabb.max.x + 0.4f, aabb.max.y + 0.4f};
                        width = aabb.max.x - aabb.min.x;
                        height = aabb.max.y - aabb.min.y;
                        x = aabb.min.x + width*0.5f;
                        y = aabb.min.y + height*0.5f;
                        TMDebugRendererDrawQuad(x, y, width, height, 0, 0xFF00FF00);

                    }break;
                    case COLLISION_TYPE_CIRCLE: {
                        Circle circle = collision->circle;
                        TMDebugRendererDrawCircle(graphics->position.x, graphics->position.y, circle.r, 0xFF00FF00, 20);
                    }break;
                    case COLLISION_TYPE_OBB: {

                    }break;
                    case COLLISION_TYPE_CAPSULE: {
                        Capsule capsule = collision->capsule;
                        TMVec2 ab = capsule.b - capsule.a;
                        float halfHeight = TMVec2Len(ab)*0.5f;
                        TMDebugRendererDrawCapsule(graphics->position.x, graphics->position.y, capsule.r, halfHeight, 0, 0xFF00FF00, 20);

                    }break;           
            }
        }
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            TMDebugRendererDrawCircle(graphics->position.x, graphics->position.y, 0.05f, 0xFF22FF22, 10);
        }
        if(entity->physics) {
            PhysicsComponent *physics = entity->physics;
            TMDebugRendererDrawLine(physics->down.o.x, physics->down.o.y,
                                    physics->down.o.x + physics->down.d.x,
                                    physics->down.o.y + physics->down.d.y,
                                    0xFF00FF00);
        }
    }
    TMDebugRenderDraw();
#endif
}
