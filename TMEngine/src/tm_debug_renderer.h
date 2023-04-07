#ifndef _TM_DEBUG_RENDERER_H_
#define _TM_DEBUG_RENDERER_H_

#include "tm_defines.h"

struct TMRenderer;

#ifdef TM_DEBUG

#define TMDebugRendererInitialize(renderer, bufferSize) TMDebugRendererInitialize_(renderer, bufferSize)
#define TMDebugRendererShutdown() TMDebugRendererShutdown_()
#define TMDebugRendererDrawLine(ax, ay, bx, by, color) TMDebugRendererDrawLine_(ax, ay, bx, by, color)
#define TMDebugRendererDrawQuad(x, y, w, h, angle, color) TMDebugRendererDrawQuad_(x, y, w, h, angle, color)
#define TMDebugRendererDrawCircle(x, y, radio, color, vertNum) TMDebugRendererDrawCircle_(x, y, radio, color, vertNum)
#define TMDebugRendererDrawCapsule(x, y, radio, halfHeight, rotation, color, vertNum) TMDebugRendererDrawCapsule_(x, y, radio, halfHeight, rotation, color, vertNum)

#define TMDebugRenderDraw() TMDebugRenderDraw_()

#elif

#define TMDebugRendererInitialize(renderer, bufferSize)
#define TMDebugRendererShutdown()
#define TMDebugRendererDrawLine(ax, ay, bx, by, color)
#define TMDebugRendererDrawQuad(x, y, w, h, angle, color)
#define TMDebugRendererDrawCircle(x, y, radio, color, vertNum)
#define TMDebugRendererDrawCapsule(x, y, radio, halfHeight, rotation, color, vertNum)
#define TMDebugRenderDraw()

#endif


TM_EXPORT void TMDebugRendererInitialize_(TMRenderer *renderer, size_t bufferSize);
TM_EXPORT void TMDebugRendererShutdown_();


TM_EXPORT void TMDebugRendererDrawLine_(float ax, float ay,
                                        float bx, float by,
                                        unsigned int color);
TM_EXPORT void TMDebugRendererDrawQuad_(float x, float y, float w, float h,
                                        float angle, unsigned int color);
TM_EXPORT void TMDebugRendererDrawCircle_(float x, float y, float radio,
                                          unsigned int color, unsigned int vertNum);
TM_EXPORT void TMDebugRendererDrawCapsule_(float x, float y, float radio, float halfHeight, float rotation,
                                           unsigned int color, unsigned int vertNum);
TM_EXPORT void TMDebugRenderDraw_();
 
#endif
