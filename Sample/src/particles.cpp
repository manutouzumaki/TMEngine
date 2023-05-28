#include "particles.h"

static TMShader *gShader;
static int gParticleSystemCount;

ParticleSystem ParticleSystemCreate(TMRenderer *renderer,
                                    bool autoManage, bool physics,
                                    float lifeTime, TMTexture *texture) {

    gParticleSystemCount++;

    if(!gShader) {
        gShader = TMRendererShaderCreate(renderer,
                                      "../../assets/shaders/batchVert.hlsl",
                                      "../../assets/shaders/batchFrag.hlsl");
    }

    ParticleSystem particleSystem{};
    particleSystem.texture = texture;
    particleSystem.renderBatch = TMRendererRenderBatchCreate(renderer, gShader, NULL, MAX_PARTICLE_COUNT);

    particleSystem.autoManage = autoManage;
    particleSystem.physics = physics;
    particleSystem.lifeTime = lifeTime;

    return particleSystem;

}

void ParticleSystemDestroy(TMRenderer *renderer, ParticleSystem *particleSystem) {
    
    gParticleSystemCount--;
    if(gParticleSystemCount == 0) {
        TMRendererShaderDestroy(renderer, gShader);
    }

    TMRendererRenderBatchDestroy(renderer, particleSystem->renderBatch);


}

void ParticleSystemAddParticles(ParticleSystem *particleSystem, Particle *particles, int count) {

}

void ParticleSystemUpdate(ParticleSystem *particleSystem, Particle *particles, int count) {

}

void ParticleSystemDraw(ParticleSystem *particleSystem, Particle *particles, int count) {

}

void ParticleSystemDestroyParticle(ParticleSystem *particleSystem, int index) {

}
