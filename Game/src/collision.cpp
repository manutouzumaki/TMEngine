#include "collision.h"
#include <math.h>
#include <float.h>


#define EPSILON 0.000001f

static float Max(float a, float b) {
    if(a > b) {
        return a;
    }
    return b;
}

static float Min(float a, float b) {
    if(a < b) {
        return a;
    }
    return b;
}

static void Swap(float &a, float &b) {
    float tmp = a;
    a = b;
    b = tmp;
}

int TestAABBAABB(AABB a, AABB b) {
    if(a.max[0] < b.min[0] || a.min[0] > b.max[0]) return 0;
    if(a.max[1] < b.min[1] || a.min[1] > b.max[1]) return 0;
    return 1;
}

int TestCircleCircle(Circle a, Circle b) {
    TMVec2 d = a.c - b.c;
    float dist2 = TMVec2LenSq(d);
    float radiusSum = a.r + b.r;
    return dist2 <= radiusSum * radiusSum;
}

int IntersectMovingAABBAABB(AABB a, AABB b,
                            TMVec2 va, TMVec2 vb,
                            float &tfirst, float &tlast) {

    if(TestAABBAABB(a, b)) {
        tfirst = tlast = 0.0f;
        return 1;
    }

    TMVec2 v = vb - va;
    if(TMVec2LenSq(v) == 0) {
        return 0;
    }

    tfirst = 0.0f;
    tlast = 1.0f;

    for(int i = 0; i < 2; ++i) {
        if(v[i] < 0.0f) {
            if(b.max[i] < a.min[i]) return 0;
            if(a.max[i] < b.min[i]) tfirst = Max((a.max[i] - b.min[i]) / v[i], tfirst);
            if(b.max[i] > a.min[i]) tlast  = Min((a.min[i] - b.max[i]) / v[i], tlast);
        }
        if(v[i] > 0.0f) {
            if(b.min[i] > a.max[i]) return 0;
            if(b.max[i] < a.min[i]) tfirst = Max((a.min[i] - b.max[i]) / v[i], tfirst);
            if(a.max[i] > b.min[i]) tlast  = Min((a.max[i] - b.min[i]) / v[i], tlast);
        }

        if(tfirst > tlast) return 0;
    }
    return 1;
}



void ClosestPtPointAABB(TMVec2 p, AABB b, TMVec2 &q) {
    for(int i = 0; i < 2; ++i) {
        float v = p[i];
        if(v < b.min[i]) v = b.min[i];
        if(v > b.max[i]) v = b.max[i];
        q.v[i] = v;
    }
}

int RayAAABB(TMVec2 p, TMVec2 d, AABB a, float &tmin, TMVec2 &q) {

    tmin = 0.0f;
    float tmax = FLT_MAX;
    for(int i = 0; i < 2; ++i) {
        if(fabsf(d[i]) < EPSILON) {
            if(p[i] < a.min[i] || p[i] > a.max[i]) return 0;
        } else {

            float ood = 1.0f / d[i];
            float t1 = (a.min[i] - p[i]) * ood;
            float t2 = (a.max[i] - p[i]) * ood;

            if(t1 > t2) Swap(t1, t2);

            tmin = Max(tmin, t1);
            tmax = Min(tmax, t2);

            if(tmin > tmax) return 0;

        } 
    }

    q = p + d * tmin;
    return 1;
    
}
