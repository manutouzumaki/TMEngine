#include "collision.h"
#include <math.h>

int TestAABBAABB(AABB a, AABB b) {
    if(fabsf(a.c[0] - b.c[0]) > (a.r[0] + b.r[0])) return 0;
    if(fabsf(a.c[1] - b.c[1]) > (a.r[1] + b.r[1])) return 0;
    return 1;
}

int TestSphereSphere(Sphere a, Sphere b) {
    TMVec2 d = a.c - b.c;
    float dist2 = TMVec2LenSq(d);
    float radiusSum = a.r + b.r;
    return dist2 <= radiusSum * radiusSum;
}
