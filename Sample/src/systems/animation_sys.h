#ifndef _ANIMATION_SYS_H_
#define _ANIMATION_SYS_H_

void AnimationSystemAddState(Entity *entity, AnimationState state);
void AnimationSystemOnMessage(MessageType type, void *sender, void *listener, Message message);
void AnimationSystemInitialize();
void AnimationSystemShutdown(Entity **entities);
void AnimationSystemUpdate(Entity **entities, float dt);

#endif
