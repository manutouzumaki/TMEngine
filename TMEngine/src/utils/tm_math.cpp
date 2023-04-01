//
// Created by Manuel Cabrerizo on 23/02/2023.
//
#include <math.h>
#include <stdio.h>
#include "tm_math.h"

// TODO(manuel): add TM_EXPORT to functions

////////////////////////////
// TMVec2 ...
////////////////////////////
TMVec2 operator+(TMVec2 a, TMVec2 b) {
    TMVec2 result{};
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

TMVec2 operator-(TMVec2 a, TMVec2 b) {
    TMVec2 result{};
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

TMVec2 operator-(TMVec2 v) {
    TMVec2 result{};
    result.x = -v.x;
    result.y = -v.y;
    return result;
}

TMVec2 operator*(TMVec2 a, TMVec2 b) {
    TMVec2 result{};
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

TMVec2 operator/(TMVec2 a, TMVec2 b) {
    TMVec2 result{};
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    return result;
}

TMVec2 operator*(TMVec2 v, float s) {
    TMVec2 result{};
    result.x = v.x * s;
    result.y = v.y * s;
    return result;
}

TMVec2 operator/(TMVec2 v, float s) {
    TMVec2 result{};
    result.x = v.x / s;
    result.y = v.y / s;
    return result;
}

float TMVec2Dot(TMVec2 a, TMVec2 b) {
    float result = (a.x * b.x) + (a.y * b.y);
    return result;
}

float TMVec2LenSq(TMVec2 v) {
    float result = TMVec2Dot(v, v);
    return result;

}

float TMVec2Len(TMVec2 v) {
    float result = TMVec2LenSq(v);
    if(result < TM_VEC_EPSILON) {
        return 0.0f;
    }
    return sqrtf(result);
}

void TMVec2Normalize(TMVec2 *v) {
    float lenSq = TMVec2LenSq(*v);
    if(lenSq < TM_VEC_EPSILON) {
        return;
    }
    float invLen = 1.0f / sqrtf(lenSq);
    v->x *= invLen;
    v->y *= invLen;
}

TMVec2 TMVec2Normalized(TMVec2 v) {
    float lenSq = TMVec2LenSq(v);
    if(lenSq < TM_VEC_EPSILON) {
        return v;
    }
    float invLen = 1.0f / sqrtf(lenSq);
    TMVec2 result{
        result.x = v.x * invLen,
        result.y = v.y * invLen,
    };
    return result;
}

float TMVec2Angle(TMVec2 a, TMVec2 b) {
    float len = TMVec2Len(a) * TMVec2Len(b);
    if(len < TM_VEC_EPSILON) {
        return 0.0f;
    }
    float dot = TMVec2Dot(a, b);
    float result = acosf(dot / len);
    return result;
}

TMVec2 TMVec2Project(TMVec2 a, TMVec2 b) {
    float magB = TMVec2Len(b);
    if(magB < TM_VEC_EPSILON) {
        return {};
    }
    float scale = TMVec2Dot(a, b) / magB;
    return b * scale;
}

TMVec2 TMVec2Reject(TMVec2 a, TMVec2 b) {
    TMVec2 proj = TMVec2Project(a, b);
    return a - proj;
}

TMVec2 TMVec2Reflect(TMVec2 a, TMVec2 b) {
    float magB = TMVec2Len(b);
    if(magB < TM_VEC_EPSILON) {
        return {};
    }
    float scale = TMVec2Dot(a, b) / magB;
    TMVec2 proj2 = b * (scale * 2.0f);
    return a - proj2;
}

TMVec2 TMVec2Lerp( TMVec2 a, TMVec2 b, float t) {
    return {
        (1 - t) * a.x + t * b.x,
        (1 - t) * a.y + t * b.y
    };
}

TMVec2 TMVec2Slerp(TMVec2 a, TMVec2 b, float t) {
    if(t < 0.01f) {
        return TMVec2Lerp(a, b, t);
    }
    TMVec2 from = TMVec2Normalized(a);
    TMVec2 to = TMVec2Normalized(b);
    float theta = TMVec2Angle(from, to);
    float sin_theta = sinf(theta);
    float s = sinf((1.0f - t) * theta) / sin_theta;
    float e = sinf(t * theta) / sin_theta;
    return from * s + to * s;
}

TMVec2 TMVec2Nlerp(TMVec2 a, TMVec2 b, float t) {
    TMVec2 v = {
        (1 - t) * a.x + t * b.x,
        (1 - t) * a.y + t * b.y
    };
    return TMVec2Normalized(v);
}

bool operator==(TMVec2 a, TMVec2 b) {
    TMVec2 diff{a - b};
    return  TMVec2LenSq(diff) < TM_VEC_EPSILON;
}

bool operator!=(TMVec2 a, TMVec2 b) {
    return !(a == b);
}

////////////////////////////
// TMVec3 ...
////////////////////////////
TMVec3 operator+(TMVec3 a, TMVec3 b) {
    TMVec3 result{};
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

TMVec3 operator-(TMVec3 a, TMVec3 b) {
    TMVec3 result{};
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

TMVec3 operator-(TMVec3 v) {
    TMVec3 result{};
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    return result;
}

TMVec3 operator*(TMVec3 a, TMVec3 b) {
    TMVec3 result{};
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

TMVec3 operator/(TMVec3 a, TMVec3 b) {
    TMVec3 result{};
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    result.z = a.z / b.z;
    return result;
}

TMVec3 operator*(TMVec3 v, float s) {
    TMVec3 result{};
    result.x = v.x * s;
    result.y = v.y * s;
    result.z = v.z * s;
    return result;
}

TMVec3 operator/(TMVec3 v, float s) {
    TMVec3 result{};
    result.x = v.x / s;
    result.y = v.y / s;
    result.z = v.z / s;
    return result;
}

float TMVec3Dot(TMVec3 a, TMVec3 b) {
    float result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    return result;
}

float TMVec3LenSq(TMVec3 v) {
    float result = TMVec3Dot(v, v);
    return result;

}

float TMVec3Len(TMVec3 v) {
    float result = TMVec3LenSq(v);
    if(result < TM_VEC_EPSILON) {
        return 0.0f;
    }
    return sqrtf(result);
}

void TMVec3Normalize(TMVec3 *v) {
    float lenSq = TMVec3LenSq(*v);
    if(lenSq < TM_VEC_EPSILON) {
        return;
    }
    float invLen = 1.0f / sqrtf(lenSq);
    v->x *= invLen;
    v->y *= invLen;
    v->z *= invLen;
}

TMVec3 TMVec3Normalized(TMVec3 v) {
    float lenSq = TMVec3LenSq(v);
    if(lenSq < TM_VEC_EPSILON) {
        return v;
    }
    float invLen = 1.0f / sqrtf(lenSq);
    TMVec3 result{
        result.x = v.x * invLen,
        result.y = v.y * invLen,
        result.z = v.z * invLen,
    };
    return result;
}

float TMVec3Angle(TMVec3 a, TMVec3 b) {
    float len = TMVec3Len(a) * TMVec3Len(b);
    if(len < TM_VEC_EPSILON) {
        return 0.0f;
    }
    float dot = TMVec3Dot(a, b);
    float result = acosf(dot / len);
    return result;
}

TMVec3 TMVec3Project(TMVec3 a, TMVec3 b) {
    float magB = TMVec3Len(b);
    if(magB < TM_VEC_EPSILON) {
        return {};
    }
    float scale = TMVec3Dot(a, b) / magB;
    return b * scale;
}

TMVec3 TMVec3Reject(TMVec3 a, TMVec3 b) {
    TMVec3 proj = TMVec3Project(a, b);
    return a - proj;
}

TMVec3 TMVec3Reflect(TMVec3 a, TMVec3 b) {
    float magB = TMVec3Len(b);
    if(magB < TM_VEC_EPSILON) {
        return {};
    }
    float scale = TMVec3Dot(a, b) / magB;
    TMVec3 proj2 = b * (scale * 2.0f);
    return a - proj2;
}

TMVec3 TMVec3Cross(TMVec3 a, TMVec3 b) {
    return {
        a.y * b.z - a.z * b.y,      
        a.z * b.x - a.x * b.z,      
        a.x * b.y - a.y * b.x
    };
}

TMVec3 TMVec3Lerp( TMVec3 a, TMVec3 b, float t) {
    return {
        (1 - t) * a.x + t * b.x,
        (1 - t) * a.y + t * b.y,
        (1 - t) * a.z + t * b.z
    };
}

TMVec3 TMVec3Slerp(TMVec3 a, TMVec3 b, float t) {
    if(t < 0.01f) {
        return TMVec3Lerp(a, b, t);
    }
    TMVec3 from = TMVec3Normalized(a);
    TMVec3 to = TMVec3Normalized(b);
    float theta = TMVec3Angle(from, to);
    float sin_theta = sinf(theta);
    float s = sinf((1.0f - t) * theta) / sin_theta;
    float e = sinf(t * theta) / sin_theta;
    return from * s + to * s;
}

TMVec3 TMVec3Nlerp(TMVec3 a, TMVec3 b, float t) {
    TMVec3 v = {
        (1 - t) * a.x + t * b.x,
        (1 - t) * a.y + t * b.y,
        (1 - t) * a.z + t * b.z   
    };
    return TMVec3Normalized(v);
}

bool operator==(TMVec3 a, TMVec3 b) {
    TMVec3 diff{a - b};
    return  TMVec3LenSq(diff) < TM_VEC_EPSILON;
}

bool operator!=(TMVec3 a, TMVec3 b) {
    return !(a == b);
}



bool operator==(TMMat4 a, TMMat4 b) {
    for(int i = 0; i < 16; ++i) {
        if(fabsf(a.v[i] - b.v[i]) > TM_MAT_EPSILON) {
            return false;
        }
    }
    return true;
}

bool operator!=(TMMat4 a, TMMat4 b) {
    return !(a == b);
}

TMMat4 operator+(TMMat4 a, TMMat4 b) {
	return {
		a.xx + b.xx, a.xy + b.xy, a.xz + b.xz, a.xw + b.xw,
		a.yx + b.yx, a.yy + b.yy, a.yz + b.yz, a.yw + b.yw,
		a.zx + b.zx, a.zy + b.zy, a.zz + b.zz, a.zw + b.zw,
		a.tx + b.tx, a.ty + b.ty, a.tz + b.tz, a.tw + b.tw
    };
}

TMMat4 operator*(TMMat4 m, float f) {
    return {
        m.xx * f, m.xy * f, m.xz * f, m.xw * f,
        m.yx * f, m.yy * f, m.yz * f, m.yw * f,
        m.zx * f, m.zy * f, m.zz * f, m.zw * f,
        m.tx * f, m.ty * f, m.tz * f, m.tw * f
    };
}

TMMat4 operator*(TMMat4 a, TMMat4 b) {
    TMMat4 result {};
#ifdef TM_WIN32
    for(int row = 0; row < 4; ++row) {
        for(int col = 0; col < 4; ++col) {
            result.v[row * 4 + col] =
                a.v[row * 4 + 0] * b.v[0 * 4 + col] +
                a.v[row * 4 + 1] * b.v[1 * 4 + col] +
                a.v[row * 4 + 2] * b.v[2 * 4 + col] +
                a.v[row * 4 + 3] * b.v[3 * 4 + col];
        }
    }
#elif TM_MACOS
    for(int col = 0; col < 4; ++col) {
        for(int row = 0; row < 4; ++row) {
            result.v[row * 4 + col] =
                a.v[0 * 4 + col] * b.v[row * 4 + 0] +
                a.v[1 * 4 + col] * b.v[row * 4 + 1] +
                a.v[2 * 4 + col] * b.v[row * 4 + 2] +
                a.v[3 * 4 + col] * b.v[row * 4 + 3];
        }
    }
#endif
    return result;
}

TMVec4 operator*(TMMat4 m, TMVec4 v) {
    TMVec4 result{};
#ifdef TM_WIN32
    for(int row = 0; row < 4; ++row) {
        result.v[row] = m.v[row * 4 + 0] * v.x + m.v[row * 4 + 1] * v.y + m.v[row * 4 + 2] * v.z + m.v[row * 4 + 3] * v.w;
    }
#elif TM_MACOS
    for(int col = 0; col < 4; ++col) {
        result.v[col] = m.v[0 * 4 + col] * v.x + m.v[1 * 4 + col] * v.y + m.v[2 * 4 + col] * v.z + m.v[3 * 4 + col] * v.w;
    }
#endif
    return result;
}

// TODO: fix this to work on Opengl and DirectX
TMVec3 TMMat4TransformVector(TMMat4 m, TMVec3 v) {
    TMVec4 vec4 = {v.x, v.y, v.z, 0.0f};
    vec4 = m * vec4;
    TMVec3 result = {vec4.x, vec4.y, vec4.z};
    return result;
}

// TODO: fix this to work on Opengl and DirectX
TMVec3 TMMat4TransformPoint(TMMat4 m, TMVec3 v) {
    TMVec4 vec4 = {v.x, v.y, v.z, 1.0f};
    vec4 = m * vec4;
    TMVec3 result = {vec4.x, vec4.y, vec4.z};
    return result;
}

// TODO: fix this to work on Opengl and DirectX
TMVec3 TMMat4TransformPoint(TMMat4 m, TMVec3 v, float *w) {
    float _w = *w;
    TMVec4 vec4 = {v.x, v.y, v.z, _w};
    vec4 = m * vec4;
    *w = vec4.w;
    TMVec3 result = {vec4.x, vec4.y, vec4.z};
    return result;
}

#define M4SWAP(x, y) \
    {float t = x; x = y; y = t; }


void TMMat4Transpose(TMMat4 *m) {
	M4SWAP(m->yx, m->xy);
	M4SWAP(m->zx, m->xz);
	M4SWAP(m->tx, m->xw);
	M4SWAP(m->zy, m->yz);
	M4SWAP(m->ty, m->yw);
	M4SWAP(m->tz, m->zw);
}

TMMat4 TMMat4Transposed(TMMat4 m) {
	return {
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
    };
}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) - \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) + \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

float TMMat4Determinant(TMMat4 m) {
	return  m.v[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3)
		  - m.v[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
		  + m.v[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3)
		  - m.v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

TMMat4 TMMat4Adjugate(TMMat4 m) {
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	TMMat4 cofactor;

	cofactor.v[0] =  M4_3X3MINOR(1, 2, 3, 1, 2, 3);
	cofactor.v[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
	cofactor.v[2] =  M4_3X3MINOR(1, 2, 3, 0, 1, 3);
	cofactor.v[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

	cofactor.v[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
	cofactor.v[5] =  M4_3X3MINOR(0, 2, 3, 0, 2, 3);
	cofactor.v[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
	cofactor.v[7] =  M4_3X3MINOR(0, 2, 3, 0, 1, 2);

	cofactor.v[8] =   M4_3X3MINOR(0, 1, 3, 1, 2, 3);
	cofactor.v[9] =  -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
	cofactor.v[10] =  M4_3X3MINOR(0, 1, 3, 0, 1, 3);
	cofactor.v[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

	cofactor.v[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
	cofactor.v[13] =  M4_3X3MINOR(0, 1, 2, 0, 2, 3);
	cofactor.v[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
	cofactor.v[15] =  M4_3X3MINOR(0, 1, 2, 0, 1, 2);

	return TMMat4Transposed(cofactor);
}

TMMat4 TMMat4Inverse(TMMat4 m) {
	float det = TMMat4Determinant(m);

	if (det == 0.0f) { // Epsilon check would need to be REALLY small
		printf("WARNING: Trying to invert a matrix with a zero determinant\n");
		return {};
	}
	TMMat4 adj = TMMat4Adjugate(m);

	return adj * (1.0f / det);
}

void TMMat4Invert(TMMat4 *m) {
	float det = TMMat4Determinant(*m);
	if (det == 0.0f) {
		printf("WARNING: Trying to invert a matrix with a zero determinant\n");
		return;
	}
	*m = TMMat4Adjugate(*m) * (1.0f / det);
}

TMMat4 TMMat4Frustum(float l, float r, float b, float t, float n, float f) {
    if (l == r || t == b || n == f) {
        printf("WARNING: Trying to create invalid frustum\n");
        return {}; // Error
    }
#ifdef TM_MACOS
    return TMMat4{
            (2.0f * n) / (r - l), 0, 0, 0,
            0, (2.0f * n) / (t - b), 0, 0,
            (r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
            0, 0, (-2 * f * n) / (f - n), 0
    };
#elif TM_WIN32
    return TMMat4{
        (2.0f * n) / (r - l), 0, -(r + l) / (r - l), 0,
        0, (2.0f * n) / (t - b), -(t + b) / (t - b), 0,
        0, 0, f / (f - n), -(f * n) / (f - n),
        0, 0, 1, 0
    };
#endif
}

TMMat4 TMMat4Perspective(float fov, float aspect, float znear, float zfar) {
    float ymax = znear * tanf(fov * 3.14159265359f / 360.0f);
    float xmax = ymax * aspect;
    return TMMat4Frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

TMMat4 TMMat4Ortho(float l, float r, float b, float t, float n, float f) {
    if (l == r || t == b || n == f) {
        return {}; // Error
    }
    
#ifdef TM_MACOS
    TMMat4 result = {
        2.0f / (r - l), 0, 0, 0,
        0, 2.0f / (t - b), 0, 0,
        0, 0, -2.0f / (f - n), 0,
        -((r + l) / (r - l)), -((t + b) / (t - b)), -((f + n) / (f - n)), 1
    };
#elif TM_WIN32
    TMMat4 result = {
       2.0f / (r - l), 0, 0, (r + l) / (r - l),
       0, 2.0f / (t - b), 0, (t + b) / (t - b),
       0, 0, 1.0f / (f - n), -n / (f - n),
       0, 0, 0, 1
    };
#endif

    return result;
}

TMMat4 TMMat4Identity() {

    return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
    };
}

TMMat4 TMMat4LookAt(TMVec3 position, TMVec3 target, TMVec3 up) {
#ifdef TM_MACOS
    // Remember, forward is negative z
    TMVec3 f = TMVec3Normalized(position - target);
    TMVec3 r = TMVec3Cross(f, up);
    if (r == TMVec3{0, 0, 0}) {
        return {}; // Error
    }
    TMVec3Normalized(r);
    TMVec3 u = TMVec3Normalized(TMVec3Cross(r, f));
    TMVec3 t = TMVec3{
            -TMVec3Dot(r, position),
            -TMVec3Dot(u, position),
            -TMVec3Dot(f, position)
    };
    TMMat4 result = {
            // Transpose upper 3x3 matrix to invert it
            r.x, u.x, f.x, 0,
            r.y, u.y, f.y, 0,
            r.z, u.z, f.z, 0,
            t.x, t.y, t.z, 1
    };
#elif TM_WIN32
    TMVec3 zaxis = TMVec3Normalized(target - position);
    TMVec3 xaxis = TMVec3Normalized(TMVec3Cross(up, zaxis));
    TMVec3 yaxis = TMVec3Cross(zaxis, xaxis);
    TMMat4 result = {
        xaxis.x, xaxis.y, xaxis.z, -TMVec3Dot(xaxis, position), 
        yaxis.x, yaxis.y, yaxis.z, -TMVec3Dot(yaxis, position), 
        zaxis.x, zaxis.y, zaxis.z, -TMVec3Dot(zaxis, position),
        0,       0,       0,       1 
    };
#endif
    return result;
}

// TODO: fix all this functions ...

TMMat4 TMMat4Translate(float x, float y, float z) {
#ifdef TM_MACOS 
    TMMat4 result = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
    };
#elif TM_WIN32
    TMMat4 result = {
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
    };
#endif
    return result;
}

TMMat4 TMMat4Scale(float x, float y, float z) {
    TMMat4 result = {
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
    };
    return result;
}

TMMat4 TMMat4RotateX(float angle) {
#ifdef TM_MACOS
    TMMat4 result = {
            1,           0,            0, 0,
            0, cosf(angle), -sinf(angle), 0,
            0, sinf(angle),  cosf(angle), 0,
            0,           0,            0, 1
    };
#elif TM_WIN32
    TMMat4 result = {
            1,            0,           0,  0,
            0,  cosf(angle), sinf(angle),  0,
            0, -sinf(angle), cosf(angle),  0,
            0,            0,           0,  1
    };
#endif

    return result;
}

TMMat4 TMMat4RotateY(float angle) {
#ifdef TM_MACOS
    TMMat4 result = {
             cosf(angle), 0, sinf(angle), 0,
                       0, 1,           0, 0,
            -sinf(angle), 0, cosf(angle), 0,
                       0, 0,           0, 1
    };
#elif TM_WIN32
    TMMat4 result = {
             cosf(angle), 0, -sinf(angle), 0,
                       0, 1,            0, 0,
             sinf(angle), 0,  cosf(angle), 0,
                       0, 0,            0, 1
    };

#endif
    return result;
}

TMMat4 TMMat4RotateZ(float angle) {
#ifdef TM_MACOS
    TMMat4 result = {
            cosf(angle), -sinf(angle), 0, 0,
            sinf(angle),  cosf(angle), 0, 0,
                      0,            0, 1, 0,
                      0,            0, 0, 1
    };
#elif TM_WIN32
    TMMat4 result = {
             cosf(angle), sinf(angle), 0, 0,
            -sinf(angle), cosf(angle), 0, 0,
                       0,           0, 1, 0,
                       0,           0, 0, 1
    };
#endif
    return result;
}
