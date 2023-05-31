#include "editor.h"
#include "utils.h"
#include <tm_window.h>

struct ConstBuffer {
    TMMat4 proj;
    TMMat4 view;
    TMMat4 world;
    TMVec4 color;
    TMVec4 absUVs;
    TMVec4 relUVs;
};

// local to the obj file
///////////////////////////////////////////////////

static ConstBuffer  gConstBuffer;
static unsigned int gIndices[] = { 1, 0, 2, 2, 0, 3 };
static TMVertex     gVertices[] = {
        TMVertex{TMVec3{ 0.5f,  0.5f, 0}, TMVec2{1, 0}, TMVec3{0, 0, 0}}, // 0
        TMVertex{TMVec3{-0.5f,  0.5f, 0}, TMVec2{0, 0}, TMVec3{0, 0, 0}}, // 1
        TMVertex{TMVec3{-0.5f, -0.5f, 0}, TMVec2{0, 1}, TMVec3{0, 0, 0}}, // 2
        TMVertex{TMVec3{ 0.5f, -0.5f, 0}, TMVec2{1, 1}, TMVec3{0, 0, 0}}  // 3
};

TMTexture   *gPlayerTexture;
TMTexture   *gShotEnemyTexture;
TMTexture   *gMoveEnemyTexture;

///////////////////////////////////////////////////

static void AddDefaultEntity(EditorState *state, float posX, float posY) {
    printf("Entity added\n");

    TMUIElement *element = state->element;

    Entity entity = {};
    entity.color = element->oldColor;
    entity.absUVs = element->absUVs;
    entity.relUVs = element->relUVs;
    entity.position = {floorf(posX) + 0.5f, floorf(posY) + 0.5f};
    entity.size = {1, 1};
    entity.texture = element->texture;
    entity.zIndex = 2;
    entity.textureIndex = element->textureIndex;
    entity.id = state->entities ? TMDarraySize(state->entities) : 0;
    entity.prefabType = PREFAB_TYPE_NONE;



    if(element->type == TM_UI_TYPE_BUTTON) {
        entity.shader = state->colorShader;
    }
    else {
        entity.shader = state->spriteShader;
    }
    TMDarrayPush(state->entities, entity, Entity);
}

static void AddPlayerEntity(EditorState *state, float posX, float posY) {

    printf("Player added\n");

    Entity entity = {};
    entity.color = {1, 1, 1, 1};
    entity.absUVs = {0, 0, 1, 1};
    entity.relUVs = {0, 0, 0.25, 0.5};
    entity.position = {floorf(posX) + 0.5f, floorf(posY) + 0.5f};
    entity.size = {1.2, 1.2};
    entity.texture = gPlayerTexture;
    entity.textureIndex = -1;
    entity.shader = state->spriteShader;
    entity.zIndex = 2;
    entity.id = state->entities ? TMDarraySize(state->entities) : 0;
    entity.prefabType = PREFAB_TYPE_PLAYER;

    entity.collision = (Collision *)malloc(sizeof(Collision));

    TMVec2 offset = {0, entity.size.y * 0.15f};

    Capsule capsule;
    capsule.r = entity.size.x*0.35f;
    capsule.a = entity.position + offset;
    capsule.b = entity.position - offset;

    entity.collision->capsule = capsule;
    entity.collision->type = COLLISION_TYPE_CAPSULE;
    entity.collision->solid = true;

    entity.animation = (Animation *)malloc(sizeof(Animation));

    AnimationState walkLeft;
    walkLeft.frames[0] = 0;
    walkLeft.frames[1] = 1;
    walkLeft.frames[2] = 2;
    walkLeft.frames[3] = 3;
    walkLeft.frameCount = 4;
    walkLeft.speed = 15.0f;

    AnimationState walkRight;
    walkRight.frames[0] = 4;
    walkRight.frames[1] = 5;
    walkRight.frames[2] = 6;
    walkRight.frames[3] = 7;
    walkRight.frameCount = 4;
    walkRight.speed = 15.0f;

    AnimationState idleLeft;
    idleLeft.frames[0] = 0;
    idleLeft.frames[1] = 3;
    idleLeft.frameCount = 2;
    idleLeft.speed = 7.0f;

    AnimationState idleRight;
    idleRight.frames[0] = 4;
    idleRight.frames[1] = 7;
    idleRight.frameCount = 2;
    idleRight.speed = 7.0f;

    entity.animation->states[0] = walkLeft;
    entity.animation->states[1] = walkRight;
    entity.animation->states[2] = idleLeft;
    entity.animation->states[3] = idleRight;
    entity.animation->statesCount = 4;
    entity.animation->index = 0;

    TMDarrayPush(state->entities, entity, Entity);
}

static void AddMoveEnemyEntity(EditorState *state, float posX, float posY) {
    printf("Enemy added\n");

    Entity entity = {};
    entity.color = {1, 1, 1, 1};
    entity.absUVs = {1.0f-(1.0f/9.0f), 0, 1, 1.0f/3.0f};
    entity.relUVs = {0, 0, 1, 1};
    entity.position = {floorf(posX) + 0.5f, floorf(posY) + 0.5f};
    entity.size = {1, 1};
    entity.texture = gMoveEnemyTexture;
    entity.textureIndex = -1;
    entity.shader = state->spriteShader;
    entity.zIndex = 2;
    entity.id = state->entities ? TMDarraySize(state->entities) : 0;
    entity.prefabType = PREFAB_TYPE_MOVE_ENEMY;

    entity.collision = (Collision *)malloc(sizeof(Collision));

    TMVec2 offset = {0, entity.size.y * 0.15f};

    Circle circle;
    circle.r = entity.size.x*0.35f;
    circle.c = entity.position;

    entity.collision->circle = circle;
    entity.collision->type = COLLISION_TYPE_CIRCLE;
    entity.collision->solid = true;

    TMDarrayPush(state->entities, entity, Entity);

}

static void AddShotEnemyEntity(EditorState *state, float posX, float posY) {
    printf("Enemy added\n");

    Entity entity = {};
    entity.color = {1, 1, 1, 1};
    entity.absUVs = {0, 0, 1, 1};
    entity.relUVs = {0, 0, 0.25, 0.5};
    entity.position = {floorf(posX) + 0.5f, floorf(posY) + 0.5f};
    entity.size = {1.2, 1.2};
    entity.texture = gShotEnemyTexture;
    entity.textureIndex = -1;
    entity.shader = state->spriteShader;
    entity.zIndex = 2;
    entity.id = state->entities ? TMDarraySize(state->entities) : 0;
    entity.prefabType = PREFAB_TYPE_SHOT_ENEMY;

    entity.collision = (Collision *)malloc(sizeof(Collision));

    TMVec2 offset = {0, entity.size.y * 0.15f};

    Capsule capsule;
    capsule.r = entity.size.x*0.35f;
    capsule.a = entity.position + offset;
    capsule.b = entity.position - offset;

    entity.collision->capsule = capsule;
    entity.collision->type = COLLISION_TYPE_CAPSULE;
    entity.collision->solid = true;

    entity.animation = (Animation *)malloc(sizeof(Animation));

    AnimationState idleLeft;
    idleLeft.frames[0] = 0;
    idleLeft.frames[1] = 3;
    idleLeft.frameCount = 2;
    idleLeft.speed = 7.0f;

    AnimationState idleRight;
    idleRight.frames[0] = 4;
    idleRight.frames[1] = 7;
    idleRight.frameCount = 2;
    idleRight.speed = 7.0f;

    entity.animation->states[0] = idleLeft;
    entity.animation->states[1] = idleRight;
    entity.animation->statesCount = 2;
    entity.animation->index = 0;

    entity.enemyShot = (EnemyShot *)malloc(sizeof(EnemyShot));

    entity.enemyShot->speed = 3;
    entity.enemyShot->range = 6;
    entity.enemyShot->facingLeft = true;

    TMDarrayPush(state->entities, entity, Entity);

}

static void AddEntity(EditorState *state, float posX, float posY) {

    switch(state->prefabType) {

        case PREFAB_TYPE_NONE:
        {
            AddDefaultEntity(state, posX, posY);
        } break;
        case PREFAB_TYPE_PLAYER:
        {
            AddPlayerEntity(state, posX, posY);
        } break;
        case PREFAB_TYPE_SHOT_ENEMY:
        {
            AddShotEnemyEntity(state, posX, posY);    
        } break;
        case PREFAB_TYPE_MOVE_ENEMY:
        {
            AddMoveEnemyEntity(state, posX, posY);    
        } break;

    }

}

static void MouseToWorld(TMVec3 cameraP, float *mouseX, float *mouseY, float width, float height, float meterToPixel) {

    float x = (float)TMInputMousePositionX();
    float y = height - (float)TMInputMousePositionY();

    float worldMouseX = (x / width)  * (width/meterToPixel);
    float worldMouseY = (y / height) * (height/meterToPixel);

    *mouseX = worldMouseX + cameraP.x;
    *mouseY = worldMouseY + cameraP.y;

}

static void LastMouseToWorld(TMVec3 cameraP, float *mouseX, float *mouseY, float width, float height, float meterToPixel) {

    float x = (float)TMInputMouseLastPositionX();
    float y = height - (float)TMInputMouseLastPositionY();

    float worldMouseX = (x / width)  * (width/meterToPixel);
    float worldMouseY = (y / height) * (height/meterToPixel);

    *mouseX = worldMouseX + cameraP.x;
    *mouseY = worldMouseY + cameraP.y;

}

static void UpdateCollision(Entity *entity) {
    Collision *collision = entity->collision;
    switch(collision->type) {
    
        case COLLISION_TYPE_AABB: 
        {
            AABB aabb;
            aabb.min = entity->position - entity->size*0.5f;
            aabb.max = entity->position + entity->size*0.5f;
            collision->aabb = aabb;
        } break;
        case COLLISION_TYPE_CIRCLE:
        {
            Circle *circle = &collision->circle;
            circle->c = entity->position;
            circle->r = entity->size.x*0.35f;
        } break;
        case COLLISION_TYPE_CAPSULE:
        {
            // TODO: improve this function to  update the capsule size
            Capsule *capsule = &collision->capsule;
            TMVec2 ab = capsule->b - capsule->a;
            float halfHeight = TMVec2Len(ab)*0.5f; 
            TMVec2 up = {0, 1};
            capsule->a = entity->position + up * halfHeight;
            capsule->b = entity->position - up * halfHeight;
        } break;

    }
}

void ClearLights(EditorState *state) {
    LightsConstBuffer *lightsConstBuffer = &state->lightsConstBuffer;
    lightsConstBuffer->count = 0;
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
}

void AddLight(EditorState *state, TMVec2 position, TMVec3 attributes, TMVec3 color, float range) {
    LightsConstBuffer *lightsConstBuffer = &state->lightsConstBuffer;
    if(lightsConstBuffer->count < 100) {
        int index = lightsConstBuffer->count;
        
        PointLight *light = lightsConstBuffer->lights + index;
        light->position = position;
        light->attributes = attributes;
        light->color = color;
        light->range = range;
        
        lightsConstBuffer->count++;

        TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
    }
}

void SetAmbientLight(EditorState *state, TMVec3 ambient) {
    LightsConstBuffer *lightsConstBuffer = &state->lightsConstBuffer;
    lightsConstBuffer->ambient = ambient;
    TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
}

void EditorInitialize(EditorState *state, TMWindow *window) {
    // TODO: remove this ...
    
    state->window = window;
    state->meterToPixel = 100.0f;
    state->renderer = TMRendererCreate(window);

    state->spriteShader = TMRendererShaderCreate(state->renderer,
                                                 "../../assets/shaders/defaultVert.hlsl",
                                                 "../../assets/shaders/spriteFrag.hlsl");

    state->colorShader = TMRendererShaderCreate(state->renderer,
                                                "../../assets/shaders/defaultVert.hlsl",
                                                "../../assets/shaders/colorFrag.hlsl");

    TMDebugRendererInitialize(state->renderer, 100);
    TMUIInitialize(state->renderer, state->meterToPixel);
    
    // Initialize ConstBuffer data
    int clientWidth = TMRendererGetWidth(state->renderer);
    int clientHeight = TMRendererGetHeight(state->renderer);
    TMVec3 target = {0, 0, 1};
    TMVec3 up = {0, 1, 0};
    gConstBuffer.view = TMMat4LookAt(state->cameraP, state->cameraP + target, up);
    gConstBuffer.proj = TMMat4Ortho(0, clientWidth/state->meterToPixel,
                                    0, clientHeight/state->meterToPixel,
                                    0.1f, 100.0f);
    gConstBuffer.world = TMMat4Identity();

    // create the shader buffer to store the ConstBuffer on the GPU
    state->shaderBuffer = TMRendererShaderBufferCreate(state->renderer, &gConstBuffer,
                                                       sizeof(ConstBuffer), 0);
    // create the buffer to store the vertices on the GPU
    state->vertexBuffer = TMRendererBufferCreate(state->renderer,
                                                 gVertices, ARRAY_LENGTH(gVertices),
                                                 gIndices, ARRAY_LENGTH(gIndices),
                                                 state->colorShader);

    // Initialize texture atlas
    // TODO: make a system to add and remove textures on the fly
    gPlayerTexture = TMRendererTextureCreate(state->renderer, "../../assets/images/player.png");
    gShotEnemyTexture = TMRendererTextureCreate(state->renderer, "../../assets/images/player2.png");
    gMoveEnemyTexture = TMRendererTextureCreate(state->renderer, "../../assets/images/characters_packed.png");

    EditorUIInitialize(state, &state->ui, (float)clientWidth, (float)clientHeight, state->meterToPixel);

    state->lightShaderBuffer = TMRendererShaderBufferCreate(state->renderer, &state->lightsConstBuffer, sizeof(LightsConstBuffer), 1);
    state->lightSelected = -1;
    
    SetAmbientLight(state, {0.2, 0.2, 0.2});

}

void EditorUpdate(EditorState *state) {
    float clientWidth  = (float)TMRendererGetWidth(state->renderer);
    float clientHeight = (float)TMRendererGetHeight(state->renderer);
    EditorUIUpdate(state, &state->ui, clientWidth, clientHeight, state->meterToPixel);


    if(!state->mouseIsHot) {
        if(TMInputMousButtonJustDown(TM_MOUSE_BUTTON_RIGHT)) {
            TMMouseSetCapture(state->window);
        }
        if(TMInputMousButtonJustUp(TM_MOUSE_BUTTON_RIGHT)) { 
            TMMouseReleaseCapture();
        }
        if(TMInputMousButtonIsDown(TM_MOUSE_BUTTON_RIGHT)) {

            state->cameraP = {
                state->cameraP.x - (float)(TMInputMousePositionX() - TMInputMouseLastPositionX()) / state->meterToPixel,
                state->cameraP.y + (float)(TMInputMousePositionY() - TMInputMouseLastPositionY()) / state->meterToPixel,
                -1
            };
            TMVec3 target = {0, 0, 1};
            TMVec3 up = {0, 1, 0};
            gConstBuffer.view = TMMat4LookAt(state->cameraP, state->cameraP + target, up);
            TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &gConstBuffer);
        }

    }
    
    if(!state->mouseIsHot && !state->element && state->entities) {
        if(TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT)) {
            for(int i = 0; i < TMDarraySize(state->entities); ++i) {

                Entity *entity = state->entities + i;
                float minX = entity->position.x - entity->size.x*0.5f;
                float maxX = minX + entity->size.x;
                float minY = entity->position.y - entity->size.y*0.5f;
                float maxY = minY + entity->size.y;

                float mouseX;
                float mouseY;
                MouseToWorld(state->cameraP, &mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);

                if(mouseX > minX && mouseX <= maxX &&
                   mouseY > minY && mouseY <= maxY) {
                    state->selectedEntity = entity;
                    state->lightSelected = -1;
                }

            }

            for(int i = 0; i < state->lightsConstBuffer.count; ++i) {

                PointLight *light = state->lightsConstBuffer.lights + i;

                float minX = light->position.x - 0.5f;
                float maxX = light->position.x + 0.5f;
                float minY = light->position.y - 0.5f;
                float maxY = light->position.y + 0.5f;

                float mouseX;
                float mouseY;
                MouseToWorld(state->cameraP, &mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);

                if(mouseX > minX && mouseX <= maxX &&
                   mouseY > minY && mouseY <= maxY) {

                    state->selectedEntity = NULL;
                    state->lightSelected = i;
                }
            }
        }
    }

    if(!state->mouseIsHot && TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT) && state->element) {
        float mouseX;
        float mouseY;
        MouseToWorld(state->cameraP, &mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);
        AddEntity(state, mouseX, mouseY);
    }
    
    if(!state->mouseIsHot && TMInputMousButtonJustDown(TM_MOUSE_BUTTON_LEFT) && state->option == OPTION_LIGHT && state->lightSelected < 0) {
        float mouseX;
        float mouseY;
        MouseToWorld(state->cameraP, &mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);
        AddLight(state, {mouseX, mouseY}, {1.0, 0.7, 1.8}, {1, 1, 1}, 2);
        
    }

    if(!state->mouseIsHot && state->selectedEntity && TMInputMousButtonIsDown(TM_MOUSE_BUTTON_LEFT)) {
        float mouseX, mouseY;
        float lastMouseX, lastMouseY;
        MouseToWorld(state->cameraP, &mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);
        LastMouseToWorld(state->cameraP, &lastMouseX, &lastMouseY, clientWidth, clientHeight, state->meterToPixel);
        float offsetX = mouseX - lastMouseX;
        float offsetY = mouseY - lastMouseY;

        Entity *entity = state->selectedEntity;
        if(state->modifyOption == MODIFY_TRANSLATE) {
            entity->position.x += offsetX;
            entity->position.y += offsetY;
            if(entity->collision) UpdateCollision(entity);

        }
        else if (state->modifyOption == MODIFY_SCALE){
            entity->size.x += offsetX;
            entity->size.y += offsetY;
            entity->position.x += offsetX*0.5f;
            entity->position.y += offsetY*0.5f;
            if(entity->collision) UpdateCollision(entity);
        }
        else if(state->modifyOption == MODIFY_INC_REL_U) {
            entity->relUVs.z -= offsetX*0.02f;
        }
        else if(state->modifyOption == MODIFY_INC_REL_V) {
            entity->relUVs.w += offsetY*0.02f;
        }
        else if(state->modifyOption == MODIFY_OFF_REL_U) {
            entity->relUVs.x += offsetX*0.02f;
            entity->relUVs.z += offsetX*0.02f;
        }
        else if(state->modifyOption == MODIFY_OFF_REL_V) {
            entity->relUVs.y += offsetY*0.02f;
            entity->relUVs.w += offsetY*0.02f;
        }
        else if(state->modifyOption == MODIFY_INC_ABS_U) {
            entity->absUVs.z -= offsetX*0.08f;
        }
        else if(state->modifyOption == MODIFY_INC_ABS_V) {
            entity->absUVs.w += offsetY*0.08f;
        }
        else if(state->modifyOption == MODIFY_OFF_ABS_U) {
            entity->absUVs.x += offsetX*0.02f;
            entity->absUVs.z += offsetX*0.02f;
        }
        else if(state->modifyOption == MODIFY_OFF_ABS_V) {
            entity->absUVs.y += offsetY*0.02f;
            entity->absUVs.w += offsetY*0.02f;
        }
        else if(state->modifyOption == MODIFY_SHOT_RANGE) {
            if(entity->enemyShot) {
                entity->enemyShot->range += offsetX*0.5f;
            }
        }
        else if(state->modifyOption == MODIFY_SHOT_SPEED) {
            if(entity->enemyShot) {
                entity->enemyShot->speed += offsetX;
                printf("bullet speed: %f\n", entity->enemyShot->speed);
            }
        }
    }

    if(!state->mouseIsHot && state->lightSelected >= 0 && TMInputMousButtonIsDown(TM_MOUSE_BUTTON_LEFT)) {

        float mouseX, mouseY;
        float lastMouseX, lastMouseY;
        MouseToWorld(state->cameraP, &mouseX, &mouseY, clientWidth, clientHeight, state->meterToPixel);
        LastMouseToWorld(state->cameraP, &lastMouseX, &lastMouseY, clientWidth, clientHeight, state->meterToPixel);
        float offsetX = mouseX - lastMouseX;
        float offsetY = mouseY - lastMouseY;

        PointLight *light = state->lightsConstBuffer.lights + state->lightSelected;

        TMVec2 *position =  &light->position;
        TMVec3 *colors =  &light->color;
        if(state->modifyOption == MODIFY_TRANSLATE_LIGHT) {

            position->x += offsetX;
            position->y += offsetY;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);

        }
        else if(state->modifyOption == MODIFY_QUADRA_LIGHT){
            float quadratic = light->attributes.z;
            quadratic -= offsetX;
            quadratic = MaxF32(quadratic, 0.0f);
            light->attributes.z = quadratic;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
        }
        else if(state->modifyOption == MODIFY_LINEAR_LIGHT){
            float linear = light->attributes.y;
            linear -= offsetX;
            linear = MaxF32(linear, 0.0f);
            light->attributes.y = linear;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
        }
        else if(state->modifyOption == MODIFY_CONSTA_LIGHT){
            float constant = light->attributes.x;
            constant -= offsetX;
            constant = MaxF32(constant, 0.0f);
            light->attributes.x = constant;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
        }
        else if(state->modifyOption == MODIFY_RANGE_LIGHT){
            float range = light->range;
            range += offsetX;
            range = MaxF32(range, 0.0f);
            light->range = range;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
        }
        else if(state->modifyOption == MODIFY_R_LIGHT){
            float color = colors->x;
            color -= offsetX;
            color = MinF32(color, 1.0f);
            color = MaxF32(color, 0.0f);
            colors->x = color;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
        }
        else if(state->modifyOption == MODIFY_G_LIGHT){
            float color = colors->y;
            color -= offsetX;
            color = MinF32(color, 1.0f);
            color = MaxF32(color, 0.0f);
            colors->y = color;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
        }
        else if(state->modifyOption == MODIFY_B_LIGHT){
            float color = colors->z;
            color -= offsetX;
            color = MinF32(color, 1.0f);
            color = MaxF32(color, 0.0f);
            colors->z = color;
            TMRendererShaderBufferUpdate(state->renderer, state->lightShaderBuffer, &state->lightsConstBuffer);
        }

    }

    // TODO: clean this up ...
    if(state->entities) {
        int id = 0;
        if(state->selectedEntity) {
            id = state->selectedEntity->id;
        }
        InsertionSortEntities(state->entities, TMDarraySize(state->entities));
        if(state->selectedEntity) {
            for(int i = 0; i < TMDarraySize(state->entities); ++i) {
                Entity *entity = state->entities + i;
                if(id == entity->id) {
                    state->selectedEntity = entity;
                }
            }
        }
    }

}

void EditorRender(EditorState *state) {

    int clientWidth, clientHeight = 0;
    if(TMRendererUpdateRenderArea(state->renderer, &clientWidth, &clientHeight)) {

        TMUIUpdateProjMatrix(state->renderer, state->meterToPixel);

        gConstBuffer.proj = TMMat4Ortho(0, clientWidth/state->meterToPixel, 0, clientHeight/state->meterToPixel, 0.1f, 100.0f);
        TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &gConstBuffer);

        state->ui.modify->position      = {clientWidth/state->meterToPixel - 4.8f, 0.0f};  
        state->ui.shotEnemyModify->position      = {clientWidth/state->meterToPixel - 4.8f, 2.0f};  
        state->ui.lightModify->position      = {clientWidth/state->meterToPixel - 4.8f, 0.0f};  
        state->ui.save->position        = {0.0f, clientHeight/state->meterToPixel - 0.25f};

        state->ui.loadScene->position   = {3.0f, clientHeight/state->meterToPixel - 0.25f - 3.5f};
        state->ui.loadTexture->position = {6.0f, clientHeight/state->meterToPixel - 0.25f - 4.0f};
        state->ui.loadShader->position  = {9.0f, clientHeight/state->meterToPixel - 0.25f - 4.5f};
    
        TMUIElementRecalculateChilds(state->ui.modify);
        TMUIElementRecalculateChilds(state->ui.shotEnemyModify);
        TMUIElementRecalculateChilds(state->ui.lightModify);
        TMUIElementRecalculateChilds(state->ui.save);
        TMUIElementRecalculateChilds(state->ui.loadScene);
        TMUIElementRecalculateChilds(state->ui.loadTexture);
        TMUIElementRecalculateChilds(state->ui.loadShader);

    }


    TMRendererClear(state->renderer, 0.2, 0.2, 0.4, 1, TM_COLOR_BUFFER_BIT|TM_DEPTH_BUFFER_BIT);

    int width  = TMRendererGetWidth(state->renderer);
    int height = TMRendererGetHeight(state->renderer);


    TMRendererDepthTestEnable(state->renderer);

    if(state->entities) {
        int entityCount = TMDarraySize(state->entities);
        for(int i = 0; i < entityCount; ++i) {
            Entity *entity = state->entities + i;
            TMRendererBindShader(state->renderer, entity->shader);
            if(entity->texture) TMRendererTextureBind(state->renderer, entity->texture, entity->shader, "uTexture", 0);

            TMMat4 trans = TMMat4Translate(entity->position.x, entity->position.y, entity->zIndex);
            TMMat4 scale = TMMat4Scale(entity->size.x, entity->size.y, 1.0f);
            gConstBuffer.world = trans * scale;
            gConstBuffer.color = entity->color;
            gConstBuffer.absUVs = entity->absUVs;
            gConstBuffer.relUVs = entity->relUVs;
            TMRendererShaderBufferUpdate(state->renderer, state->shaderBuffer, &gConstBuffer);
            TMRendererDrawBufferElements(state->renderer, state->vertexBuffer);

        }
    }

    TMRendererDepthTestDisable(state->renderer);

    TMVec3 pos = state->cameraP;
    for(int y = 0; y < (height/state->meterToPixel) + 2; ++y) {
        int offsetY = (int)pos.y;
        TMDebugRendererDrawLine(0 + pos.x, y  + offsetY, width/state->meterToPixel + pos.x, y + offsetY, 0xFF666666);
    }
    for(int x = 0; x < (width/state->meterToPixel) + 2; ++x) {
        int offsetX = (int)pos.x;
        TMDebugRendererDrawLine(x + offsetX, 0 + pos.y, x + offsetX, height/state->meterToPixel + pos.y, 0xFF666666);
    }

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        TMDebugRendererDrawQuad(entity->position.x, entity->position.y, entity->size.x, entity->size.y, 0, 0xFF00FF00);
    }
    if(state->lightSelected >= 0) {

        PointLight *light = state->lightsConstBuffer.lights + state->lightSelected;
        TMVec2 position = light->position;
        TMDebugRendererDrawQuad(position.x, position.y, 1, 1, 0, 0xFF00FF00);
    }

    TMDebugRenderDraw();

    if(state->entities) {
        for(int i = 0; i < TMDarraySize(state->entities); ++i) {
            Entity *entity = state->entities + i;

            if(entity->collision) {

                unsigned int color = 0xFF00FFFF;
                if(entity->collision->solid) {
                    color = 0xFFFF0000;
                }

                switch(entity->collision->type) {
                
                    case COLLISION_TYPE_AABB: 
                    {
                        TMVec2 position = entity->collision->aabb.min + entity->size*0.5f;
                        TMVec2 size = entity->collision->aabb.max - entity->collision->aabb.min;
                        TMDebugRendererDrawQuad(position.x, position.y, size.x, size.y, 0, color);
                    } break;
                    case COLLISION_TYPE_CIRCLE:
                    {
                        Circle circle = entity->collision->circle;
                        TMDebugRendererDrawCircle(circle.c.x, circle.c.y, circle.r, color, 20);
                    } break;
                    case COLLISION_TYPE_CAPSULE:
                    {
                        Capsule capsule = entity->collision->capsule;
                        TMVec2 ab = capsule.b - capsule.a;
                        float halfHeight = TMVec2Len(ab)*0.5f;
                        TMDebugRendererDrawCapsule(entity->position.x, entity->position.y, capsule.r, halfHeight, 0, color, 20);
                    } break;
                }
            }
            if(entity->enemyShot) {
                TMVec2 start = entity->position;
                TMVec2 end = {1, 0};
                if(entity->enemyShot->facingLeft) {
                    end = end * -entity->enemyShot->range;
                }
                else {
                    end = end * entity->enemyShot->range;
                }
                end = start + end;
                TMDebugRendererDrawLine(start.x, start.y, end.x, end.y, 0xFFFFFF00);
            }



        }
    }


    TMDebugRenderDraw();

    EditorUIDraw(state, &state->ui, state->renderer);

    TMRendererPresent(state->renderer, 1);
}

void EditorShutdown(EditorState *state) {

    if(state->entities) {
        for(int i = 0; i < TMDarraySize(state->entities); ++i) {
            Entity *entity = state->entities + i;
            if (entity->collision) free(entity->collision);
            if (entity->animation) free(entity->animation);
            if (entity->enemyShot) free(entity->enemyShot);
        }
        TMDarrayDestroy(state->entities);
    }

    if(state->textures) {
        for(int i = 0; i < TMDarraySize(state->textures); ++i) {
            TMTexture *texture = state->textures[i];
            TMRendererTextureDestroy(state->renderer, texture);
        }
        TMDarrayDestroy(state->textures);
    }

    if(state->shaders) {
        for(int i = 0; i < TMDarraySize(state->shaders); ++i) {
            TMShader *shader = state->shaders[i];
            TMRendererShaderDestroy(state->renderer, shader);
        }
        TMDarrayDestroy(state->shaders);
    }
    if(state->texturesAddedNames) TMDarrayDestroy(state->texturesAddedNames);
    if(state->shadersAddedNames) TMDarrayDestroy(state->shadersAddedNames);

    EditorUIShutdown(&state->ui);
    TMRendererTextureDestroy(state->renderer, gPlayerTexture);
    TMRendererTextureDestroy(state->renderer, gShotEnemyTexture);
    TMRendererTextureDestroy(state->renderer, gMoveEnemyTexture);

    TMRendererBufferDestroy(state->renderer, state->vertexBuffer);
    TMRendererShaderBufferDestroy(state->renderer, state->lightShaderBuffer);
    TMRendererShaderBufferDestroy(state->renderer, state->shaderBuffer);
    TMUIShutdown(state->renderer);
    TMDebugRendererShutdown();
    TMRendererShaderDestroy(state->renderer, state->colorShader);
    TMRendererShaderDestroy(state->renderer, state->spriteShader);
    TMRendererDestroy(state->renderer);

    printf("Shuting down !!!\n");

}
