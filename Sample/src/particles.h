#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <utils/tm_math.h>
#include <tm_renderer.h>

#define MAX_PARTICLE_COUNT 1000

struct Particle {
    
    TMVec2 acceleration;
    TMVec2 velocity;
    TMVec2 position;

    TMVec4 color;
    TMVec2 size;
    TMVec2 uv;

    float life;

};

struct ParticleSystem {
    TMTexture *texture; 
    TMRenderBatch *renderBatch;
    Particle particles[MAX_PARTICLE_COUNT];
    int particleCount;
    float rate;
    float timer;

};

ParticleSystem ParticleSystemCreate(TMRenderer *renderer, TMTexture *texture = NULL);
void ParticleSystemDestroy(TMRenderer *renderer, ParticleSystem *particleSystem);
void ParticleSystemAddParticles(ParticleSystem *particleSystem, TMVec2 position);
void ParticleSystemUpdate(ParticleSystem *particleSystem, float dt);
void ParticleSystemDraw(ParticleSystem *particleSystem);

#endif
