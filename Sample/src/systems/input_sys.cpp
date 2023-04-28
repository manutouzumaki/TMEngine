#include "../entity.h"
#include <utils/tm_darray.h>
#include <tm_input.h>
#include "../message.h"


void InputSystemUpdate(Entity **entities, float dt) {

    // TODO: remove this static vairable
    static bool facingLeft = false;

    for(int i = 0; i < TMDarraySize(entities); ++i) {

        Entity *entity = entities[i];
        if(entity->input && entity->physics) {
            PhysicsComponent *physics = entity->physics;
            TMVec2 acceleration = {};
            if(TMInputKeyboardKeyIsDown(TM_KEY_D) ||
               TMInputJoystickButtomIsDown(TM_JOYSTICK_BUTTON_RIGHT)) {
                acceleration.x = 1.0f;
                facingLeft = false;
            }
            if(TMInputKeyboardKeyIsDown(TM_KEY_A) ||
               TMInputJoystickButtomIsDown(TM_JOYSTICK_BUTTON_LEFT)) {
                acceleration.x = -1.0f;
                facingLeft = true;
            }
            if(TMInputKeyboardKeyJustDown(TM_KEY_SPACE) ||
               TMInputJoystickButtomJustDown(TM_JOYSTICK_BUTTON_A)) {
                if(entity->physics->grounded) {
                    Message message{};
                    message.v2[0] = {0, 15.0f};
                    MessageFireFirstHit(MESSAGE_TYPE_PHYSICS_ADD_IMPULSE, (void *)entity, message);
                }
            }

            if(TMInputJoystickLeftStickX() < 0) {
                facingLeft = true;
            }
            if(TMInputJoystickLeftStickX() > 0) {
                facingLeft = false;
            }
            acceleration.x += TMInputJoystickLeftStickX();


            Message message;
            if(acceleration.x > 0.0f) {
                message.i32[0] = 1;
                MessageFireFirstHit(MESSAGE_TYPE_ANIMATION_SET_STATE, (void *)entity, message);
            }
            else if (acceleration.x < 0.0f){
                message.i32[0] = 0;
                MessageFireFirstHit(MESSAGE_TYPE_ANIMATION_SET_STATE, (void *)entity, message);
            } else {
                if(facingLeft) {
                    message.i32[0] = 2;
                    MessageFireFirstHit(MESSAGE_TYPE_ANIMATION_SET_STATE, (void *)entity, message);
                }
                else {
                    message.i32[0] = 3;
                    MessageFireFirstHit(MESSAGE_TYPE_ANIMATION_SET_STATE, (void *)entity, message);
                }
            }


            if(TMVec2LenSq(acceleration) > 0) {
                
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
