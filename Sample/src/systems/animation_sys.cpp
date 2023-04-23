#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>

#include "../entity.h"
#include "../message.h"
#include <utils/tm_darray.h>

static void UpdateState(Entity *entity, float dt) {
    AnimationComponet *animation = entity->animation;
    AnimationState *current = animation->current;

    animation->timer += dt * current->speed;
    int frameIndex = (int)animation->timer % current->frameCount;
    Message message;
    message.i32[0] = current->frames[frameIndex];
    MessageFireFirstHit(MESSAGE_TYPE_GRAPHICS_UPDATE_ANIMATION_INDEX, (void *)entity, message);

}

static void ChangeState(Entity *entity, int index) {
    AnimationComponet *animation = entity->animation;
    assert(index < animation->statesCount);
    animation->current = &animation->states[index];
}


void AnimationSystemAddState(Entity *entity, AnimationState state) {
    assert(entity->animation);
    AnimationComponet *animation = entity->animation;
    TMDarrayPush(animation->states, state, AnimationState); 
    animation->statesCount++;
    animation->current = animation->states;
}


void AnimationSystemOnMessage(MessageType type, void *sender, void *listener, Message message) {
    switch(type) {

        case MESSAGE_TYPE_ANIMATION_SET_STATE: {
            Entity *entity    = (Entity *)sender;
            int index = message.i32[0]; 
            ChangeState(entity, index);
        } break;
    } 
}

void AnimationSystemInitialize() {
    MessageRegister(MESSAGE_TYPE_ANIMATION_SET_STATE, NULL, AnimationSystemOnMessage);
}

void AnimationSystemShutdown(Entity **entities) {
    
    for(int i = 0; i < TMDarraySize(entities); ++i) {
    
        Entity *entity = entities[i];
        if(entity->animation && entity->animation->statesCount) {
            TMDarrayDestroy(entity->animation->states);
        }

    }
}

void AnimationSystemUpdate(Entity **entities, float dt) {

    for(int i = 0; i < TMDarraySize(entities); ++i) {
    
        Entity *entity = entities[i];
        if(entity->animation && entity->animation->current) {

            UpdateState(entity, dt);


        }

    }
}

