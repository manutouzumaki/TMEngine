#ifndef _TM_MATH_H_
#define _TM_MATH_H_

#include "../tm_defines.h"

#define TM_VEC_EPSILON 0.000001f
#define TM_MAT_EPSILON 0.000001f

struct TMVec2 {
    union {
        struct {
            float x;
            float y;
        };
        float v[2];
    };
};

TM_EXPORT TMVec2 operator+(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 operator-(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 operator-(TMVec2 v);
TM_EXPORT TMVec2 operator*(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 operator/(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 operator*(TMVec2 v, float s);
TM_EXPORT TMVec2 operator/(TMVec2 v, float s);
TM_EXPORT float TMVec2Dot(TMVec2 a, TMVec2 b);
TM_EXPORT float TMVec2LenSq(TMVec2 v);
TM_EXPORT float TMVec2Len(TMVec2 v);
TM_EXPORT void TMVec2Normalize(TMVec2 *v);
TM_EXPORT TMVec2 TMVec2Normalized(TMVec2 v);
TM_EXPORT float TMVec2Angle(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 TMVec2Project(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 TMVec2Reject(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 TMVec2Reflect(TMVec2 a, TMVec2 b);
TM_EXPORT TMVec2 TMVec2Lerp( TMVec2 a, TMVec2 b, float t);
TM_EXPORT TMVec2 TMVec2Slerp(TMVec2 a, TMVec2 b, float t);
TM_EXPORT TMVec2 TMVec2Nlerp(TMVec2 a, TMVec2 b, float t);
TM_EXPORT bool operator==(TMVec2 a, TMVec2 b);
TM_EXPORT bool operator!=(TMVec2 a, TMVec2 b);

struct TMVec3 {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float v[3];
    };
};

TM_EXPORT TMVec3 operator+(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 operator-(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 operator-(TMVec3 v);
TM_EXPORT TMVec3 operator*(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 operator/(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 operator*(TMVec3 v, float s);
TM_EXPORT TMVec3 operator/(TMVec3 v, float s);
TM_EXPORT float TMVec3Dot(TMVec3 a, TMVec3 b);
TM_EXPORT float TMVec3LenSq(TMVec3 v);
TM_EXPORT float TMVec3Len(TMVec3 v);
TM_EXPORT void TMVec3Normalize(TMVec3 *v);
TM_EXPORT TMVec3 TMVec3Normalized(TMVec3 v);
TM_EXPORT float TMVec3Angle(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 TMVec3Project(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 TMVec3Reject(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 TMVec3Reflect(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 TMVec3Cross(TMVec3 a, TMVec3 b);
TM_EXPORT TMVec3 TMVec3Lerp( TMVec3 a, TMVec3 b, float t);
TM_EXPORT TMVec3 TMVec3Slerp(TMVec3 a, TMVec3 b, float t);
TM_EXPORT TMVec3 TMVec3Nlerp(TMVec3 a, TMVec3 b, float t);
TM_EXPORT bool operator==(TMVec3 a, TMVec3 b);
TM_EXPORT bool operator!=(TMVec3 a, TMVec3 b);

struct TMVec4 {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        float v[4];
    };
};

struct TMMat3 {
    union {
        float v[9];
		struct {
			//            row 1     row 2     row 3
			/* column 1 */float xx; float xy; float xz;
			/* column 2 */float yx; float yy; float yz;
			/* column 3 */float zx; float zy; float zz;
		};
    };
};

struct TMMat4 {
    union {
        float v[16];
        struct {
            TMVec4 right;
            TMVec4 up;
            TMVec4 forward;
            TMVec4 position;
        };
		struct {
			//            row 1     row 2     row 3     row 4
			/* column 1 */float xx; float xy; float xz; float xw;
			/* column 2 */float yx; float yy; float yz; float yw;
			/* column 3 */float zx; float zy; float zz; float zw;
			/* column 4 */float tx; float ty; float tz; float tw;
		};
    };
};

TM_EXPORT bool operator==(TMMat4 a, TMMat4 b);
TM_EXPORT bool operator!=(TMMat4 a, TMMat4 b);
TM_EXPORT TMMat4 operator+(TMMat4 a, TMMat4 b);
TM_EXPORT TMMat4 operator*(TMMat4 m, float f);
TM_EXPORT TMMat4 operator*(TMMat4 a, TMMat4 b);
TM_EXPORT TMVec4 operator*(TMMat4 m, TMVec4 v);

// TODO: fix this to work on Opengl and DirectX
TM_EXPORT TMVec3 TMMat4TransformVector(TMMat4 m, TMVec3 v);
TM_EXPORT TMVec3 TMMat4TransformPoint(TMMat4 m, TMVec3 v);
TM_EXPORT TMVec3 TMMat4TransformPoint(TMMat4 m, TMVec3 v, float *w);

TM_EXPORT void TMMat4Transpose(TMMat4 *m);
TM_EXPORT TMMat4 TMMat4Transposed(TMMat4 m);
TM_EXPORT float TMMat4Determinant(TMMat4 m);
TM_EXPORT TMMat4 TMMat4Adjugate(TMMat4 m);
TM_EXPORT TMMat4 TMMat4Inverse(TMMat4 m);
TM_EXPORT void TMMat4Invert(TMMat4 *m);
TM_EXPORT TMMat4 TMMat4Frustum(float l, float r, float b, float t, float n, float f);
TM_EXPORT TMMat4 TMMat4Perspective(float fov, float aspect, float znear, float zfar);
TM_EXPORT TMMat4 TMMat4Ortho(float l, float r, float b, float t, float n, float f);
TM_EXPORT TMMat4 TMMat4Identity();
TM_EXPORT TMMat4 TMMat4LookAt(TMVec3 position, TMVec3 target, TMVec3 up);
TM_EXPORT TMMat4 TMMat4Translate(float x, float y, float z);
TM_EXPORT TMMat4 TMMat4Scale(float x, float y, float z);
TM_EXPORT TMMat4 TMMat4RotateX(float angle);
TM_EXPORT TMMat4 TMMat4RotateY(float angle);
TM_EXPORT TMMat4 TMMat4RotateZ(float angle);

struct TMQuat {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        struct {
            TMVec3 vector;
            float scala;
        };
        float v[4];
    };
};

#endif
