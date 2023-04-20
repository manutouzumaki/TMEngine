#ifndef _PHYSICS_SYS_H_
#define _PHYSICS_SYS_H_

#include "../entity.h"

void PhysicSystemInitialize();
void PhysicSystemShutdown();
void PhysicSystemUpdate(Entity **entities, float dt);
void PhysicSystemFixUpdate(Entity **entities, float dt);
void PhysicSystemPostUpdate(Entity **entities, float t);


#endif
