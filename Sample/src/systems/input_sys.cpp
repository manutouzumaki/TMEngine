#include "../entity.h"
#include <utils/tm_darray.h>
#include <tm_input.h>
#include "../message.h"

// TODO: remove the animation hardcoded piece of shit of this code

void InputSystemUpdate(Entity **entities, float dt) {

    static bool facingLeft = true;
    static float increment = 0;

    for(int i = 0; i < TMDarraySize(entities); ++i) {

        Entity *entity = entities[i];
        if(entity->input && entity->physics) {
            PhysicsComponent *physics = entity->physics;
            TMVec2 acceleration = {};
            if(TMInputKeyboardKeyIsDown('D')) {
                acceleration.x = 1.0f;
                facingLeft = false;
            }
            if(TMInputKeyboardKeyIsDown('A')) {
                acceleration.x = -1.0f;
                facingLeft = true;
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


                increment += dt * 15;
                if(facingLeft) {
                    message.i32[0] = ((int)increment) % 4;
                }
                else {
                    message.i32[0] = (((int)increment) % 4) + 4;
                }
                MessageFireFirstHit(MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX, (void *)entity, message);
            }
            else {

                increment += dt * 10;
                Message message{};
                if(facingLeft) {
                    message.i32[0] = (((int)increment) % 2) * 3;
                }
                else {
                    message.i32[0] = ((((int)increment) % 2) * 3) + 4;
                }
                MessageFireFirstHit(MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX, (void *)entity, message);
            }
        }


    }
}
