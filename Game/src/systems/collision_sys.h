#ifndef _COLLISION_SYS_H_
#define _COLLISION_SYS_H_

#include "../entity.h"

void CollisionSystemInitialize();
void CollisionSystemShutdown();
void CollisionSystemUpdate(Entity **entities);

#endif
