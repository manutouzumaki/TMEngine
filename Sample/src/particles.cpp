#include "particles.h"

#include <math.h>

static TMShader *gShader;
static int gParticleSystemCount;

ParticleSystem ParticleSystemCreate(TMRenderer *renderer, TMTexture *texture) {

    gParticleSystemCount++;

    if(!gShader) {
        gShader = TMRendererShaderCreate(renderer,
                                      "../../assets/shaders/batchVert.hlsl",
                                      "../../assets/shaders/batchFrag.hlsl");
    }

    ParticleSystem particleSystem{};
    particleSystem.texture = texture;
    particleSystem.renderBatch = TMRendererRenderBatchCreate(renderer, gShader, NULL, MAX_PARTICLE_COUNT);
    particleSystem.rate = 0.002f;
    particleSystem.timer = 0.0f;
    

    return particleSystem;

}

void ParticleSystemDestroy(TMRenderer *renderer, ParticleSystem *particleSystem) {
    
    gParticleSystemCount--;
    if(gParticleSystemCount == 0) {
        TMRendererShaderDestroy(renderer, gShader);
    }

    TMRendererRenderBatchDestroy(renderer, particleSystem->renderBatch);


}

// TODO: improve this to add diferent kind of prticles
void ParticleSystemAddParticles(ParticleSystem *particleSystem, TMVec2 position) {

    if(particleSystem->timer == 0.0f) {
        Particle particle {};
        particle.acceleration.x = ((rand() % 10) - 5)*2.0f;
        particle.acceleration.y = ((rand() % 5))*2.0f;
        particle.position = position;
        particle.color = {0.8, 0.4, 0.2, 0.4f};
        float size = ((rand() % 5) + 1)*0.05f;
        particle.size.x = size;
        particle.size.y = size;
        particle.life = 0.5f;

        particleSystem->particles[particleSystem->particleCount++] = particle;
        particleSystem->particleCount %= MAX_PARTICLE_COUNT;
    }

}

void ParticleSystemUpdate(ParticleSystem *particleSystem, float dt) {

    particleSystem->timer += dt;
    if(particleSystem->timer >= particleSystem->rate) {
        particleSystem->timer = 0;
    }

    for(int i = 0; i < particleSystem->particleCount; ++i) {

        Particle *particle = particleSystem->particles + i;
        particle->velocity = particle->velocity + particle->acceleration * dt;
        particle->position = particle->position + particle->velocity * dt;
        particle->life -= dt;

    }

}

void ParticleSystemDraw(ParticleSystem *particleSystem) {
    
    
    for(int i = 0; i < particleSystem->particleCount; ++i) {
        Particle *particle = particleSystem->particles + i;
        if(particle->life > 0.0f) {
            TMRendererRenderBatchAdd(particleSystem->renderBatch,
                                     particle->position.x,
                                     particle->position.y,
                                     1.0f,
                                     particle->size.x,
                                     particle->size.y,
                                     0.0f,
                                     particle->color.x, particle->color.y, particle->color.z, particle->color.w);
        }
    }
    
    TMRendererRenderBatchDraw(particleSystem->renderBatch);

}
