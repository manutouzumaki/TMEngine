#ifndef _GRAPHYCS_SYS_H_
#define _GRAPHYCS_SYS_H_

struct Entity;

void GraphicsSystemInitialize(TMRenderer *renderer, TMShader *shader);
void GraphicsSystemShutdown(TMRenderer *renderer);
void GraphicsSystemDraw(TMRenderer *renderer, Entity **entities);


void GraphicsSystemSetProjMatrix(TMRenderer *renderer, TMMat4 proj);
void GraphicsSystemSetViewMatrix(TMRenderer *renderer, TMMat4 view);
void GraphicsSystemSetWorldMatrix(TMRenderer *renderer, TMMat4 world);


#endif
