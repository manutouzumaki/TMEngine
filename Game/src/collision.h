#ifndef _COLLISION__H_
#define _COLLISION__H_

#include <utils/tm_math.h>

struct AABB { 
    TMVec2 min;
    TMVec2 max;
};

struct OBB {
    TMVec2 c;
    TMVec2 u[2];
    TMVec2 e;
};

struct Circle {
    TMVec2 c;
    float r;
};

struct Capsule {
    TMVec2 a;
    TMVec2 b;
    float r;
};

struct Ray {
    TMVec2 o;
    TMVec2 d;
};

int TestAABBAABB(AABB a, AABB b);
int TestCircleCircle(Circle a, Circle b);
int IntersectMovingAABBAABB(AABB a, AABB b,
                            TMVec2 va, TMVec2 vb,
                            float &tfirst, float &tlast);
void ClosestPtPointAABB(TMVec2 p, AABB b, TMVec2 &q);

void RayAAABB(Ray ray, AABB aabb, float &t);
int RayAAABB(TMVec2 p, TMVec2 d, AABB a, float &tmin, TMVec2 &q);

#endif
