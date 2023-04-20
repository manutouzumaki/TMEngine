#include "../entity.h"
#include <utils/tm_darray.h>
#include <tm_input.h>
#include "../message.h"


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
                //acceleration.y = 1.0f;
            }
            if(TMInputKeyboardKeyIsDown('S')) {
                //acceleration.y = -1.0f;
            }
            if(TMInputKeyboardKeyJustDown(TM_KEY_SPACE)) {
                if(entity->physics->grounded) {
                    Message message{};
                    message.v2[0] = {0, 15.0f};
                    MessageFireFirstHit(MESSAGE_TYPE_PHYSICS_ADD_IMPULSE, (void *)entity, message);
                }
            }
            
            if(TMVec2LenSq(acceleration) > 0) {

                Message message{};
                
                if(entity->physics->grounded) {
                    message.v2[0] = TMVec2Normalized(acceleration) * 30.0f;
                }
                else {
                    message.v2[0] = (TMVec2Normalized(acceleration) * 30.0f) * 0.1f;
                }
                MessageFireFirstHit(MESSAGE_TYPE_PHYSICS_ADD_FORCE, (void *)entity, message);
            }
        }


    }
}
