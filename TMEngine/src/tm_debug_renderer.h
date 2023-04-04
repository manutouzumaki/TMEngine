#ifndef _TM_DEBUG_RENDERER_H_
#define _TM_DEBUG_RENDERER_H_

#include "tm_defines.h"

struct TMRenderer;

#ifdef TM_DEBUG

#define TMDebugRendererInitialize(renderer, bufferSize) TMDebugRendererInitialize_(renderer, bufferSize)
#define TMDebugRendererShutdown() TMDebugRendererShutdown_()
#define TMDebugRendererDrawQuad(x, y, w, h, angle, color) TMDebugRendererDrawQuad_(x, y, w, h, angle, color)
#define TMDebugRenderDraw() TMDebugRenderDraw_()

#elif

#define TMDebugRendererInitialize(renderer, bufferSize)
#define TMDebugRendererShutdown()
#define TMDebugRendererDrawQuad(x, y, w, h, angle, color)
#define TMDebugRenderDraw()

#endif


TM_EXPORT void TMDebugRendererInitialize_(TMRenderer *renderer, size_t bufferSize);
TM_EXPORT void TMDebugRendererShutdown_();
TM_EXPORT void TMDebugRendererDrawQuad_(float x, float y, float w, float h,
                                        float angle, unsigned int color);
TM_EXPORT void TMDebugRenderDraw_();
 
#endif
