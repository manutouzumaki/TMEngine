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
};

// TODO: create a nice lit system for all this piace of shit code
static ConstBuffer  gConstBuffer;
static unsigned int gIndices[] = { 1, 0, 2, 2, 0, 3 };
static TMVertex     gVertices[] = {
        TMVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec3{0, 0, 0}}, // 0
        TMVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}, TMVec3{0, 0, 0}}, // 1
        TMVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec3{0, 0, 0}}, // 2
        TMVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}, TMVec3{0, 0, 0}}  // 3
};
static TMTexture *gPlayerTexture;
static float     *gUVs[32];
static int        gUVsCount[32];

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

void GraphicsSystemInitialize(TMRenderer *renderer, TMShader *shader) {

    MessageRegister(MESSAGE_TYPE_GRAPHICS_UPDATE_POSITIONS, NULL, GraphicsSystemOnMessage);
    MessageRegister(MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX, NULL, GraphicsSystemOnMessage);

    // create the shader buffer to store the ConstBuffer on the GPU
    gGraphicsState.shaderBuffer = TMRendererShaderBufferCreate(renderer, &gConstBuffer,
                                                       sizeof(ConstBuffer), 0);
    // create the buffer to store the vertices on the GPU
    gGraphicsState.vertexBuffer = TMRendererBufferCreate(renderer,
                                                 gVertices, ARRAY_LENGTH(gVertices),
                                                 gIndices, ARRAY_LENGTH(gIndices),
                                                 shader);

    gPlayerTexture = TMRendererTextureCreate(renderer, "../../assets/images/player.png");

    gUVs[0] = TMGenerateUVs(gPlayerTexture, 16, 16, &gUVsCount[0]);

    
}

void GraphicsSystemShutdown(TMRenderer *renderer) {

    free(gUVs[0]);
    TMRendererTextureDestroy(renderer, gPlayerTexture);
    TMRendererShaderBufferDestroy(renderer, gGraphicsState.shaderBuffer);
    TMRendererBufferDestroy(renderer, gGraphicsState.vertexBuffer);

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


void GraphicsSystemDraw(TMRenderer *renderer, Entity **entities) {

    TMRendererDepthTestEnable(renderer);
    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            

            TMRendererBindShader(renderer, graphics->shader);
            if(graphics->texture) {
                TMRendererTextureBind(renderer, graphics->texture, graphics->shader, "uTexture", 0);
            }
            else {
                TMRendererTextureBind(renderer, gPlayerTexture, graphics->shader, "uTexture", 0);
            }

        
            TMMat4 trans = TMMat4Translate(graphics->position.x,
                                           graphics->position.y,
                                           (float)graphics->zIndex); // TODO: add zIndex
            TMMat4 scale = TMMat4Scale(graphics->size.x,
                                       graphics->size.y,
                                       1.0f);
            gConstBuffer.world = trans * scale;
            gConstBuffer.absUVs = graphics->absUVs;


            if(entity->animation) {
                TMVec4 *uvs = (TMVec4 *)gUVs[entity->animation->index];
                gConstBuffer.relUVs = uvs[graphics->index];
            }
            else {
                gConstBuffer.relUVs = graphics->relUVs;
            }

            gConstBuffer.color = graphics->color;
            TMRendererShaderBufferUpdate(renderer, gGraphicsState.shaderBuffer, &gConstBuffer);
            TMRendererDrawBufferElements(renderer, gGraphicsState.vertexBuffer);

        }
    }

    TMRendererDepthTestDisable(renderer);

#ifdef TM_DEBUG
    // draw debug geometry
    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->collision, entity->graphics) {
            CollisionComponent *collision = entity->collision;
            if(collision) {

                GraphicsComponent *graphics = entity->graphics;

                switch(collision->type) {
                        case COLLISION_TYPE_AABB: {
                            AABB aabb = collision->aabb;
                            float width = aabb.max.x - aabb.min.x;
                            float height = aabb.max.y - aabb.min.y;
                            float x = aabb.min.x + width*0.5f;
                            float y = aabb.min.y + height*0.5f;
                            
                            TMDebugRendererDrawQuad(x, y, width, height, 0, 0xFF00FF00);

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
