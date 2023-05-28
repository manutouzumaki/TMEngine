#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <utils/tm_math.h>
#include <tm_renderer.h>

#define MAX_PARTICLE_COUNT 300

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
    bool autoManage;
    bool physics;
    float lifeTime;

};

ParticleSystem ParticleSystemCreate(TMRenderer *renderer,
                                    bool autoManage, bool physics,
                                    float lifeTime, TMTexture *texture = NULL);
void ParticleSystemDestroy(TMRenderer *renderer, ParticleSystem *particleSystem);

void ParticleSystemAddParticles(ParticleSystem *particleSystem, Particle *particles, int count);
void ParticleSystemUpdate(ParticleSystem *particleSystem, Particle *particles, int count);
void ParticleSystemDraw(ParticleSystem *particleSystem, Particle *particles, int count);
void ParticleSystemDestroyParticle(ParticleSystem *particleSystem, int index);



#endif
