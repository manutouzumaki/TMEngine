#include "../entity.h"
#include <utils/tm_darray.h>
#include <tm_renderer.h>
#include <tm_debug_renderer.h>
#include "../message.h"

static float MetersToPixel = 100;



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
    }

}

void GraphicsSystemInitialize() {
    MessageRegister(MESSAGE_TYPE_GRAPHICS_UPDATE_POSITIONS, NULL, GraphicsSystemOnMessage);
}

void GraphicsSystemShutdown() {


}


void GraphicsSystemDraw(TMRenderBatch *batchRenderer, Entity **entities) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            TMRendererRenderBatchAdd(batchRenderer,
                                     graphics->position.x, graphics->position.y, 1,
                                     graphics->size.x, graphics->size.y, 0,
                                     graphics->color.x, graphics->color.y,
                                     graphics->color.z, graphics->color.w);
        }
    }
    TMRendererRenderBatchDraw(batchRenderer);

#ifdef TM_DEBUG
    // draw debug geometry
    for(int i = 0; i < TMDarraySize(entities); ++i) {
        Entity *entity = entities[i];
        if(entity->collision) {
            CollisionComponent *collision = entity->collision;
            AABB aabb = collision->aabb;
            float width = aabb.max.x - aabb.min.x;
            float height = aabb.max.y - aabb.min.y;
            float x = aabb.min.x + width*0.5f;
            float y = aabb.min.y + height*0.5f;
            TMDebugRendererDrawQuad(x, y, width, height, 0, 0xFF00FF00);
            TMDebugRendererDrawCircle(aabb.min.x, aabb.min.y, 5.0f/MetersToPixel, 0xFFFFFF00, 10);
            TMDebugRendererDrawCircle(aabb.max.x, aabb.max.y, 5.0f/MetersToPixel, 0xFF00FFFF, 10);

            aabb.min = {aabb.min.x - 0.4f, aabb.min.y - 0.6f};
            aabb.max = {aabb.max.x + 0.4f, aabb.max.y + 0.6f};
            width = aabb.max.x - aabb.min.x;
            height = aabb.max.y - aabb.min.y;
            x = aabb.min.x + width*0.5f;
            y = aabb.min.y + height*0.5f;
            TMDebugRendererDrawQuad(x, y, width, height, 0, 0xFF00FF00);
        }
        if(entity->graphics) {
            GraphicsComponent *graphics = entity->graphics;
            TMDebugRendererDrawCircle(graphics->position.x, graphics->position.y, 5.0f/MetersToPixel, 0xFF22FF22, 10);
        }
    }
    TMDebugRenderDraw();
#endif
}
