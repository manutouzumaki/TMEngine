#ifndef _TM_MATH_H_
#define _TM_MATH_H_

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

TMVec2 operator+(TMVec2 a, TMVec2 b);
TMVec2 operator-(TMVec2 a, TMVec2 b);
TMVec2 operator-(TMVec2 v);
TMVec2 operator*(TMVec2 a, TMVec2 b);
TMVec2 operator/(TMVec2 a, TMVec2 b);
TMVec2 operator*(TMVec2 v, float s);
TMVec2 operator/(TMVec2 v, float s);
float TMVec2Dot(TMVec2 a, TMVec2 b);
float TMVec2LenSq(TMVec2 v);
float TMVec2Len(TMVec2 v);
void TMVec2Normalize(TMVec2 *v);
TMVec2 TMVec2Normalized(TMVec2 v);
float TMVec2Angle(TMVec2 a, TMVec2 b);
TMVec2 TMVec2Project(TMVec2 a, TMVec2 b);
TMVec2 TMVec2Reject(TMVec2 a, TMVec2 b);
TMVec2 TMVec2Reflect(TMVec2 a, TMVec2 b);
TMVec2 TMVec2Lerp( TMVec2 a, TMVec2 b, float t);
TMVec2 TMVec2Slerp(TMVec2 a, TMVec2 b, float t);
TMVec2 TMVec2Nlerp(TMVec2 a, TMVec2 b, float t);
bool operator==(TMVec2 a, TMVec2 b);
bool operator!=(TMVec2 a, TMVec2 b);

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

TMVec3 operator+(TMVec3 a, TMVec3 b);
TMVec3 operator-(TMVec3 a, TMVec3 b);
TMVec3 operator-(TMVec3 v);
TMVec3 operator*(TMVec3 a, TMVec3 b);
TMVec3 operator/(TMVec3 a, TMVec3 b);
TMVec3 operator*(TMVec3 v, float s);
TMVec3 operator/(TMVec3 v, float s);
float TMVec3Dot(TMVec3 a, TMVec3 b);
float TMVec3LenSq(TMVec3 v);
float TMVec3Len(TMVec3 v);
void TMVec3Normalize(TMVec3 *v);
TMVec3 TMVec3Normalized(TMVec3 v);
float TMVec3Angle(TMVec3 a, TMVec3 b);
TMVec3 TMVec3Project(TMVec3 a, TMVec3 b);
TMVec3 TMVec3Reject(TMVec3 a, TMVec3 b);
TMVec3 TMVec3Reflect(TMVec3 a, TMVec3 b);
TMVec3 TMVec3Cross(TMVec3 a, TMVec3 b);
TMVec3 TMVec3Lerp( TMVec3 a, TMVec3 b, float t);
TMVec3 TMVec3Slerp(TMVec3 a, TMVec3 b, float t);
TMVec3 TMVec3Nlerp(TMVec3 a, TMVec3 b, float t);
bool operator==(TMVec3 a, TMVec3 b);
bool operator!=(TMVec3 a, TMVec3 b);

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
		struct {
			float c0r0; float c0r1; float c0r2;
			float c1r0; float c1r1; float c1r2;
			float c2r0; float c2r1; float c2r2;
		};
		struct {
			float r0c0; float r1c0; float r2c0;
			float r0c1; float r1c1; float r2c1;
			float r0c2; float r1c2; float r2c2;
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
		struct {
			float c0r0; float c0r1; float c0r2; float c0r3;
			float c1r0; float c1r1; float c1r2; float c1r3;
			float c2r0; float c2r1; float c2r2; float c2r3;
			float c3r0; float c3r1; float c3r2; float c3r3;
		};
		struct {
			float r0c0; float r1c0; float r2c0; float r3c0;
			float r0c1; float r1c1; float r2c1; float r3c1;
			float r0c2; float r1c2; float r2c2; float r3c2;
			float r0c3; float r1c3; float r2c3; float r3c3;
		};
    };
};

bool operator==(TMMat4 a, TMMat4 b);
bool operator!=(TMMat4 a, TMMat4 b);
TMMat4 operator+(TMMat4 a, TMMat4 b);
TMMat4 operator*(TMMat4 m, float f);
TMMat4 operator*(TMMat4 a, TMMat4 b);
TMVec4 operator*(TMMat4 m, TMVec4 v);
TMVec3 TMMat4TransformVector(TMMat4 m, TMVec3 v);
TMVec3 TMMat4TransformPoint(TMMat4 m, TMVec3 v);
TMVec3 TMMat4TransformPoint(TMMat4 m, TMVec3 v, float *w);
void TMMat4Transpose(TMMat4 *m);
TMMat4 TMMat4Transposed(TMMat4 m);
float TMMat4Determinant(TMMat4 m);
TMMat4 TMMat4Adjugate(TMMat4 m);
TMMat4 TMMat4Inverse(TMMat4 m);
void TMMat4Invert(TMMat4 *m);
TMMat4 TMMat4Frustum(float l, float r, float b, float t, float n, float f);
TMMat4 TMMat4Perspective(float fov, float aspect, float znear, float zfar);
TMMat4 TMMat4Ortho(float l, float r, float b, float t, float n, float f);
TMMat4 TMMat4Identity();
TMMat4 TMMat4LookAt(TMVec3 position, TMVec3 target, TMVec3 up);
TMMat4 TMMat4Translate(float x, float y, float z);
TMMat4 TMMat4Scale(float x, float y, float z);
TMMat4 TMMat4RotateX(float angle);
TMMat4 TMMat4RotateY(float angle);
TMMat4 TMMat4RotateZ(float angle);

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
