#include "../entity.h"
#include <utils/tm_darray.h>
#include <tm_renderer.h>
#include <tm_debug_renderer.h>
#include "../message.h"

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

void GraphicsSystemInitialize() {
    MessageRegister(MESSAGE_TYPE_GRAPHICS_UPDATE_POSITIONS, NULL, GraphicsSystemOnMessage);
    MessageRegister(MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX, NULL, GraphicsSystemOnMessage);
}

void GraphicsSystemShutdown() {


}


void GraphicsSystemDraw(TMRenderBatch *batchRenderer, Entity **entities) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            
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
                                             graphics->size.x, graphics->size.y, 0, graphics->index, graphics->relUVs);
                } break;
                case GRAPHICS_TYPE_SUBSPRITE: {
                    TMRendererRenderBatchAdd(batchRenderer, graphics->position.x, graphics->position.y, 1,
                                             graphics->size.x, graphics->size.y, 0, graphics->absUVs, graphics->index, graphics->relUVs);
                } break;

            }


        }
    }
    TMRendererRenderBatchDraw(batchRenderer);

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
