#ifndef _COLLISION__H_
#define _COLLISION__H_

#include <utils/tm_math.h>

struct AABB { 
    TMVec2 c;
    float r[2];
};

struct OBB {
    TMVec2 c;
    TMVec2 u[2];
    TMVec2 e;
};

struct Sphere {
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

#endif
