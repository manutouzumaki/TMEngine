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

struct Segment {
    TMVec2 a;
    TMVec2 b;
};

int TestAABBAABB(AABB a, AABB b);
int TestCircleCircle(Circle a, Circle b);
int TestCircleAABB(Circle a, AABB b);
int TestPointAABB(TMVec2 a, AABB b);
int TestPointCircle(TMVec2 a, Circle b);


int IntersectMovingAABBAABB(AABB a, AABB b,
                            TMVec2 va, TMVec2 vb,
                            float &tfirst, float &tlast);
int IntersectMovingCircleAABB(Circle circle, TMVec2 d, AABB b, float &t);
void ClosestPtPointAABB(TMVec2 p, AABB b, TMVec2 &q);
int RayAABB(TMVec2 p, TMVec2 d, AABB a, float &tmin, TMVec2 &q);
int RayCircle(TMVec2 p, TMVec2 d, Circle circle, float &t, TMVec2 &q);


void ClosestPtPointCircle(TMVec2 p, Circle circle, TMVec2 &q) ;
float ClosestPtSegmentSegment(TMVec2 p1, TMVec2 q1,
                              TMVec2 p2, TMVec2 q2,
                              float &s,  float &t,
                              TMVec2 &c1, TMVec2 &c2);


int VorornoiRegionAABB(Circle circle, AABB b);


int IntersectSegmentCircle(Segment seg, Circle circle, float &t, TMVec2 &q);
int IntersectSegmentCapsule(Segment seg, TMVec2 a, TMVec2 b, float r, float &tOut, TMVec2 &q);


#endif
