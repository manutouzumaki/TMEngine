#include "../entity.h"
#include <utils/tm_darray.h>
#include <tm_input.h>


void InputSystemUpdate(Entity **entities) {
    for(int i = 0; i < TMDarraySize(entities); ++i) {

        Entity *entity = entities[i];
        if(entity->input && entity->physics) {
            PhysicsComponent *physics = entity->physics;
            TMVec2 acceleration = {};
            if(TMInputKeyboardKeyIsDown('D')) {
                acceleration.x = 1.0f;
            }
            if(TMInputKeyboardKeyIsDown('A')) {
                acceleration.x = -1.0f;
            }
            if(TMInputKeyboardKeyIsDown('W')) {
                acceleration.y = 1.0f;
            }
            if(TMInputKeyboardKeyIsDown('S')) {
                acceleration.y = -1.0f;
            }
            if(TMVec2LenSq(acceleration) > 0) {
              physics->acceleration = TMVec2Normalized(acceleration) * 30.0f;
            }
            else {
                physics->acceleration = acceleration;
            }
        }


    }
}
