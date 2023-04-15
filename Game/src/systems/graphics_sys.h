#ifndef _GRAPHYCS_SYS_H_
#define _GRAPHYCS_SYS_H_

struct TMRenderBatch;
struct Entity;

void GraphicsSystemInitialize();
void GraphicsSystemShutdown();
void GraphicsSystemDraw(TMRenderBatch *batchRenderer, Entity **entities);


#endif
