#include "collision.h"
#include <math.h>
#include <float.h>

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

static float Clamp(float n, float min, float max) {
    if(n < min) return min;
    if(n > max) return max;
    return n;
}

static TMVec2 Corner(AABB b, int n) {
    TMVec2 p;
    p.x = ((n & 1) ? b.max.x : b.min.x);
    p.y = ((n & 2) ? b.max.y : b.min.y);
    return p;
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

int IntersectMovingCircleAABB(Circle circle, TMVec2 d, AABB b, float &t) {

    AABB e = b;
    e.min.x -= circle.r; e.min.y -= circle.r;
    e.max.x += circle.r; e.max.y += circle.r;

    TMVec2 p;
    if(!RayAABB(circle.c, d, e, t, p) || t > 1.0f) {
        return 0;
    }

    int u = 0, v = 0;
    if(p.x < b.min.x) u |= 1;
    if(p.x > b.max.x) v |= 1;
    if(p.y < b.min.y) u |= 2;
    if(p.y > b.max.y) v |= 2;

    int m = u + v;

    Segment seg = {circle.c, circle.c + d};
    TMVec2 q;

    if(m == 3) {
        // Vertex region
        TMVec2 collisionCircleP;
        ClosestPtPointAABB(circle.c, b, collisionCircleP);
        Circle collisionCircle;
        collisionCircle.c = collisionCircleP;
        collisionCircle.r = circle.r;
        return RayCircle(circle.c, d, collisionCircle, t, q); 
    }

    if((m & (m - 1)) == 0) { 
        return 1;
    }
    
    // Face region
    return RayAABB(circle.c, d, b, t, q);

}


int VorornoiRegionAABB(Circle circle, AABB b) {
    int u = 0, v = 0;
    TMVec2 p = circle.c;
    if(p.x < b.min.x) u |= 1;
    if(p.x > b.max.x) v |= 1;
    if(p.y < b.min.y) u |= 2;
    if(p.y > b.max.y) v |= 2;
    int m = u + v;

    return m;
}


void ClosestPtPointAABB(TMVec2 p, AABB b, TMVec2 &q) {
    for(int i = 0; i < 2; ++i) {
        float v = p[i];
        if(v < b.min[i]) v = b.min[i];
        if(v > b.max[i]) v = b.max[i];
        q.v[i] = v;
    }
}

int RayAABB(TMVec2 p, TMVec2 d, AABB a, float &tmin, TMVec2 &q) {

    tmin = 0.0f;
    float tmax = FLT_MAX;
    for(int i = 0; i < 2; ++i) {
        if(fabsf(d[i]) < FLT_EPSILON) {
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

int RayCircle(TMVec2 p, TMVec2 d, Circle circle, float &t, TMVec2 &q) { 
    
    TMVec2 m = p - circle.c;
    float b = TMVec2Dot(m, d);
    float c = TMVec2Dot(m, m) - circle.r * circle.r;
    if(c > 0.0f && b > 0.0f) return 0;
    float discr = b*b - c;
    if(discr < 0.0f) return 0;
    t = -b - sqrtf(discr);
    if(t < 0.0f) t = 0.0f;
    
    q = p + d * t;
    return 1;
}

int IntersectSegmentCircle(Segment seg, Circle circle, float &t, TMVec2 &q) {
    TMVec2 d = seg.b - seg.a;
    TMVec2 m = seg.a - circle.c;

    float b = TMVec2Dot(m, d);
    float c = TMVec2Dot(m, m) - circle.r * circle.r;
    if(c > 0.0f && b > 0.0f) return 0;
    float discr = b*b - c;
    if(discr < 0.0f) return 0;
    t = -b - sqrtf(discr);
    if(t < 0.0f) t = 0.0f;
    
    q = seg.a + d * t;
    return 1;
}

int IntersectSegmentCapsule(Segment seg, TMVec2 a, TMVec2 b, float r, float &tOut, TMVec2 &q) {
    TMVec2 c1;
    TMVec2 c2;
    float s = 0;
    float t = 0;
    float sqDist =  ClosestPtSegmentSegment(seg.a, seg.b, a, b, s, t, c1, c2); 
    bool result = sqDist <= r*r;
    if(result) {
        Circle circle;
        circle.c = c2;
        circle.r = r;
        IntersectSegmentCircle(seg, circle, tOut, q); 
        return 1;
    }

    return 0;
}



void ClosestPtPointCircle(TMVec2 p, Circle circle, TMVec2 &q) {

    TMVec2 d = p - circle.c;
    q = circle.c + TMVec2Normalized(d) * circle.r;

}




float ClosestPtSegmentSegment(TMVec2 p1, TMVec2 q1,
                              TMVec2 p2, TMVec2 q2,
                              float &s,  float &t,
                              TMVec2 &c1, TMVec2 &c2) {
    TMVec2 d1 = q1 - p1; // direction vector of segment s1
    TMVec2 d2 = q2 - p2; // direction vector of segment s2
    TMVec2 r = p1 - p2;
    float a = TMVec2Dot(d1, d1); // squared length of segment S1
    float e = TMVec2Dot(d2, d2); // squared length of segment S2
    float f = TMVec2Dot(d2, r);
    // check if either or both segments degenerate into points
    if(a <= FLT_EPSILON  && e <= FLT_EPSILON) { 
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return TMVec2Dot(c1 - c2, c1 - c2);
    }
    if(a <= FLT_EPSILON) {
        // first segment degenerate into a point
        s = 0.0f;
        t = f / e;
        t = Clamp(t, 0.0f, 1.0f); 
    }
    else {
        float c = TMVec2Dot(d1, r);
        if(e <= FLT_EPSILON) {
            // second segment degenerate into a point
            t = 0.0f;
            s = Clamp(-c / a, 0.0f, 1.0f);
        }
        else {
            // the general nondegenerate case start here
            float b = TMVec2Dot(d1, d2);
            float denom = a*e-b*b;
            // if segments not parallel, compute closest point on L1 to L2 and
            // clamp to segment S1. Else pick arbitrary s (here 0)
            if(denom != 0.0f) {
                s = Clamp((b*f - c*e) / denom, 0.0f, 1.0f); 
            }
            else s = 0.0f;
            // compute point on L2 closest to S1(s) using
            // t = dot((P1 + D1*s) - P2, D2) / dot(D2, D2) = (b*s + f) / e
            t = (b*s + f) / e;

            if(t < 0.0f) {
                t = 0.0f;
                s = Clamp(-c / a, 0.0f, 1.0f);
            }
            else if(t > 1.0f) {
                t = 1.0f;
                s = Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }
    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return TMVec2Dot(c1 - c2, c1 - c2);
}
