#include "editor_ui.h"
#include "editor.h"

#include <stdio.h>
#include <tm_window.h>
#include <utils/tm_darray.h>
#include <memory.h>
#include <assert.h>

static TMVec4 Texture(TMHashmap *hashmap, const char *filepath) {
    TMVec4 result = *((TMVec4 *)TMHashmapGet(hashmap, filepath));
    return result; 
}

static int MinI32(int a, int b) {
    if(a < b) return a;
    return b;
}

static int MaxI32(int a, int b) {
    if(a > b) return a;
    return b;
}

static int StringLength(char *string) {
    int counter = 0;
    char *letter = string; 
    while(*letter++ != L'\0') counter++;
    return counter;
}

static void OptionSelected(TMUIElement *element) {
    printf("Option selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->option = (BrushOption)element->index;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_NONE;
    state->selectedEntity = NULL;
}

static void ClearSelected(TMUIElement *element) {
    printf("Clear selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->prefabType = PREFAB_TYPE_NONE;
    state->element = NULL;
}

static void ElementSelected(TMUIElement *element) {
    printf("Element selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->selectedEntity = NULL;
}

static void PlayerPrefabSelected(TMUIElement *element) {
    printf("Prefab player selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_PLAYER;
    state->selectedEntity = NULL;

}

static void EnemyPrefabSelected(TMUIElement *element) {
    printf("Prefab enemy selected\n");
    EditorState *state = (EditorState *)element->userData;
    state->element = element;
    state->modifyOption = MODIFY_NONE;
    state->prefabType = PREFAB_TYPE_ENEMY;
    state->selectedEntity = NULL;
}

static void TranslateEntity(TMUIElement *element) { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_TRANSLATE; }
static void ScaleEntity(TMUIElement *element)     { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_SCALE;     }
static void IncrementAbsU(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_ABS_U; }
static void IncrementAbsV(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_ABS_V; }
static void OffsetAbsU(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_ABS_U; }
static void OffsetAbsV(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_ABS_V; }
static void IncrementRelU(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_REL_U; }
static void IncrementRelV(TMUIElement *element)   { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_INC_REL_V; }
static void OffsetRelU(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_REL_U; }
static void OffsetRelV(TMUIElement *element)      { EditorState *state = (EditorState *)element->userData; state->modifyOption = MODIFY_OFF_REL_V; }

static void IncrementZ(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        entity->zIndex++;
        printf("zIndex: %d\n", entity->zIndex);
    }

}

static void DecrementZ(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        entity->zIndex--;
        entity->zIndex = MaxI32(entity->zIndex, 1);
        printf("zIndex: %d\n", entity->zIndex);
    }

}

static void AddCollisionToEntity(Entity *entity) {

    if(!entity->collision) {

        entity->collision = (Collision *)malloc(sizeof(Collision));
        Collision *collision = entity->collision;

        collision->type = COLLISION_TYPE_AABB;
        collision->solid = true;
        AABB aabb;
        aabb.min = entity->position - entity->size*0.5f;
        aabb.max = entity->position + entity->size*0.5f;
        collision->aabb = aabb;
    }

}

static void RemoveCollisionToEntity(Entity *entity) {
    if(entity->collision) free(entity->collision);
    entity->collision = NULL;
}


static void AddCollision(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        AddCollisionToEntity(entity);
    }

}

static void RemCollision(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;

    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        RemoveCollisionToEntity(entity);
    }

}

static void SolidCollision(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    state->modifyOption = MODIFY_NONE;
    if(state->selectedEntity) {
        Entity *entity = state->selectedEntity;
        if(entity->collision) {
            entity->collision->solid = !entity->collision->solid;
        }
    }

}

static void LoadFileNamesFromDirectory(char *path, char ***filesNames) {
    TMGetFileNamesInDirectory(path, filesNames);
    if(*filesNames) {
        for (int i = 0; i < TMDarraySize(*filesNames); ++i) {
            printf("%s\n", (*filesNames)[i]);
        }
    }
}

static void FreeFileNames(char ***filesNames) {
    if(*filesNames) {
        for (int i = 0; i < TMDarraySize(*filesNames); ++i) {
            if((*filesNames)[i]) {
                printf("deleted %s\n", (*filesNames)[i]);
                free((void *)(*filesNames)[i]);
            }
        }
        TMDarrayDestroy(*filesNames);
        *filesNames  = NULL;
    }
}

static void LoadTexture(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    if(state->loadOption == LOAD_OPTION_NONE || state->loadOption == LOAD_OPTION_SHADER) {
        state->loadOption = LOAD_OPTION_TEXTURE;
    }
    else {
        state->loadOption = LOAD_OPTION_NONE;
    }

}

static void LoadShader(TMUIElement *element) {

    EditorState *state = (EditorState *)element->userData;
    if(state->loadOption == LOAD_OPTION_NONE || state->loadOption == LOAD_OPTION_TEXTURE) {
        state->loadOption = LOAD_OPTION_SHADER;
    }
    else {
        state->loadOption = LOAD_OPTION_NONE;
    }
}

static void SelectTexture(TMUIElement *element) {
    
    EditorState *state = (EditorState *)element->userData;
    printf("Texture Selected: %s\n", state->ui.texturesNames[element->index]);

    char filepath[10000] = "../../assets/images/";
    int headerSize = StringLength(filepath);
    int nameSize = StringLength(state->ui.texturesNames[element->index]);
    assert(headerSize + nameSize < 10000);

    memcpy(filepath + headerSize, state->ui.texturesNames[element->index], nameSize);

    filepath[headerSize + nameSize] = '\0';

    printf("path created: %s\n", filepath);

    TMTexture *texture = TMRendererTextureCreate(state->renderer, filepath);
    
    TMDarrayPush(state->textures, texture, TMTexture *);
    TMDarrayPush(state->texturesAddedNames, state->ui.texturesNames[element->index], char *);

    int textureIndex = TMDarraySize(state->texturesAddedNames) - 1;


    int index = (TMDarraySize(state->textures) - 1) / 8;
    if(TMDarraySize(state->textures) <= 3*8) {

        TMVec4 uvs = {0, 0, 1, 1};
        TMUIElementAddChildImageButton(state->ui.texturesChilds[index], TM_UI_ORIENTATION_HORIZONTAL,
                                       texture, uvs, uvs, ElementSelected, state, textureIndex);

    }

}

static void SelectShader(TMUIElement *element) {
    EditorState *state = (EditorState *)element->userData;
}

static void SaveScene(TMUIElement *element) {
    EditorState *state = (EditorState *)element->userData;
    printf("Scene Saved\n");

    TMJsonObject jsonRoot = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonRoot, "Root");

    TMJsonObject jsonLevelTextures = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonLevelTextures, "LevelTextures");

    for(int i = 0; i < TMDarraySize(state->texturesAddedNames); ++i) {
        const char *textureName =  (const char *)state->texturesAddedNames[i];
        TMJsonObjectSetValue(&jsonLevelTextures, textureName);
    
    }
    TMJsonObjectAddChild(&jsonRoot, &jsonLevelTextures);

    TMJsonObject jsonScene = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonScene, "Scene");

    for(int i = 0; i < TMDarraySize(state->entities); ++i) {
        Entity *entity = state->entities + i;

        TMJsonObject jsonEntity = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonEntity, "Entity");
        
        // Save Graphic Component
        {
            TMJsonObject jsonGraphic = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonGraphic, "Graphics");

            // graphics type
            TMJsonObject jsonType = TMJsonObjectCreate();

            TMJsonObjectSetName(&jsonType, "Type");
            if(entity->shader == state->colorShader) {
                TMJsonObjectSetValue(&jsonType, 0.0f);
            }
            else if(entity->shader == state->spriteShader) {
                TMJsonObjectSetValue(&jsonType, 1.0f);
            }

            // position
            TMJsonObject jsonPosition = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonPosition, "Position");
            TMJsonObject xPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&xPos, "X");
            TMJsonObjectSetValue(&xPos, entity->position.x);
            TMJsonObject yPos = TMJsonObjectCreate();
            TMJsonObjectSetName(&yPos, "Y");
            TMJsonObjectSetValue(&yPos, entity->position.y);
            TMJsonObjectAddChild(&jsonPosition, &xPos);
            TMJsonObjectAddChild(&jsonPosition, &yPos);

            // size
            TMJsonObject jsonSize = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonSize, "Size");
            TMJsonObject xSiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&xSiz, "X");
            TMJsonObjectSetValue(&xSiz, entity->size.x);
            TMJsonObject ySiz = TMJsonObjectCreate();
            TMJsonObjectSetName(&ySiz, "Y");
            TMJsonObjectSetValue(&ySiz, entity->size.y);
            TMJsonObjectAddChild(&jsonSize, &xSiz);
            TMJsonObjectAddChild(&jsonSize, &ySiz);

            // color
            TMJsonObject jsonColor = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonColor, "Color");
            TMJsonObject r = TMJsonObjectCreate();
            TMJsonObjectSetName(&r, "R");
            TMJsonObjectSetValue(&r, entity->color.x);
            TMJsonObject g = TMJsonObjectCreate();
            TMJsonObjectSetName(&g, "G");
            TMJsonObjectSetValue(&g, entity->color.y);
            TMJsonObject b = TMJsonObjectCreate();
            TMJsonObjectSetName(&b, "B");
            TMJsonObjectSetValue(&b, entity->color.z);
            TMJsonObject a = TMJsonObjectCreate();
            TMJsonObjectSetName(&a, "A");
            TMJsonObjectSetValue(&a, entity->color.w);
            TMJsonObjectAddChild(&jsonColor, &r);
            TMJsonObjectAddChild(&jsonColor, &g);
            TMJsonObjectAddChild(&jsonColor, &b);
            TMJsonObjectAddChild(&jsonColor, &a);
            
            // Texture
            TMJsonObject jsonAbsUVs = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonAbsUVs, "AbsUvs");
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[0]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[1]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[2]);
            TMJsonObjectSetValue(&jsonAbsUVs, entity->absUVs[3]);
            TMJsonObject jsonRelUVs = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonRelUVs, "RelUvs");
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[0]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[1]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[2]);
            TMJsonObjectSetValue(&jsonRelUVs, entity->relUVs[3]);

            TMJsonObject jsonZIndex = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonZIndex, "ZIndex");
            TMJsonObjectSetValue(&jsonZIndex, entity->zIndex);

            TMJsonObject jsonTextureIndex = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonTextureIndex, "TextureIndex");
            TMJsonObjectSetValue(&jsonTextureIndex, entity->textureIndex);

            TMJsonObjectAddChild(&jsonGraphic, &jsonType);
            TMJsonObjectAddChild(&jsonGraphic, &jsonPosition);
            TMJsonObjectAddChild(&jsonGraphic, &jsonSize);
            TMJsonObjectAddChild(&jsonGraphic, &jsonColor);
            TMJsonObjectAddChild(&jsonGraphic, &jsonAbsUVs);
            TMJsonObjectAddChild(&jsonGraphic, &jsonRelUVs);
            TMJsonObjectAddChild(&jsonGraphic, &jsonZIndex);
            TMJsonObjectAddChild(&jsonGraphic, &jsonTextureIndex);
        
            TMJsonObjectAddChild(&jsonEntity, &jsonGraphic);

        }
        if(entity->prefabType != PREFAB_TYPE_NONE) {
        
            // add Physics Component
            {
                TMJsonObject jsonPhysics = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonPhysics, "Physics");

                // position
                TMJsonObject jsonPosition = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonPosition, "Position");
                TMJsonObject xPos = TMJsonObjectCreate();
                TMJsonObjectSetName(&xPos, "X");
                TMJsonObjectSetValue(&xPos, entity->position.x);
                TMJsonObject yPos = TMJsonObjectCreate();
                TMJsonObjectSetName(&yPos, "Y");
                TMJsonObjectSetValue(&yPos, entity->position.y);
                TMJsonObjectAddChild(&jsonPosition, &xPos);
                TMJsonObjectAddChild(&jsonPosition, &yPos);

                TMJsonObject jsonVelocity = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonVelocity, "Velocity");
                TMJsonObject xVel = TMJsonObjectCreate();
                TMJsonObjectSetName(&xVel, "X");
                TMJsonObjectSetValue(&xVel, 0.0f);
                TMJsonObject yVel = TMJsonObjectCreate();
                TMJsonObjectSetName(&yVel, "Y");
                TMJsonObjectSetValue(&yVel, 0.0f);
                TMJsonObjectAddChild(&jsonVelocity, &xVel);
                TMJsonObjectAddChild(&jsonVelocity, &yVel);

                TMJsonObject jsonAccel = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonAccel, "Acceleration");
                TMJsonObject xAcc = TMJsonObjectCreate();
                TMJsonObjectSetName(&xAcc, "X");
                TMJsonObjectSetValue(&xAcc, 0.0f);
                TMJsonObject yAcc = TMJsonObjectCreate();
                TMJsonObjectSetName(&yAcc, "Y");
                TMJsonObjectSetValue(&yAcc, 0.0f);
                TMJsonObjectAddChild(&jsonAccel, &xAcc);
                TMJsonObjectAddChild(&jsonAccel, &yAcc);

                TMJsonObject jsonDamping = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonDamping, "Damping");
                TMJsonObjectSetValue(&jsonDamping, 0.01f);

                TMJsonObjectAddChild(&jsonPhysics, &jsonPosition);
                TMJsonObjectAddChild(&jsonPhysics, &jsonVelocity);
                TMJsonObjectAddChild(&jsonPhysics, &jsonAccel);
                TMJsonObjectAddChild(&jsonPhysics, &jsonDamping);

                TMJsonObjectAddChild(&jsonEntity, &jsonPhysics);
            }

            if(entity->prefabType == PREFAB_TYPE_PLAYER) {
                TMJsonObject jsonInput = TMJsonObjectCreate();
                TMJsonObjectSetName(&jsonInput, "Input");
                TMJsonObjectSetValue(&jsonInput, 1.0f);
                TMJsonObjectAddChild(&jsonEntity, &jsonInput);
            }

        }
        if(entity->collision) {
            TMJsonObject jsonCollision = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonCollision, "Collision");

            TMJsonObject solid = TMJsonObjectCreate();
            TMJsonObjectSetName(&solid, "Solid");
            TMJsonObjectSetValue(&solid, (float)((int)entity->collision->solid));

            TMJsonObjectAddChild(&jsonCollision, &solid);

            switch(entity->collision->type) {
            
                case COLLISION_TYPE_AABB: {
                    TMJsonObject aabb = TMJsonObjectCreate();
                    TMJsonObjectSetName(&aabb, "AABB");

                    TMJsonObject min = TMJsonObjectCreate();
                    TMJsonObjectSetName(&min, "Min");
                    TMJsonObjectSetValue(&min, entity->collision->aabb.min.x);
                    TMJsonObjectSetValue(&min, entity->collision->aabb.min.y);

                    TMJsonObject max = TMJsonObjectCreate();
                    TMJsonObjectSetName(&max, "Max");
                    TMJsonObjectSetValue(&max, entity->collision->aabb.max.x);
                    TMJsonObjectSetValue(&max, entity->collision->aabb.max.y);

                    TMJsonObjectAddChild(&aabb, &min);
                    TMJsonObjectAddChild(&aabb, &max);

                    TMJsonObjectAddChild(&jsonCollision, &aabb);
                    
                } break;
                case COLLISION_TYPE_CIRCLE: {
                    // TODO: ...

                } break;
                case COLLISION_TYPE_CAPSULE: {
                    TMJsonObject capsule = TMJsonObjectCreate();
                    TMJsonObjectSetName(&capsule, "Capsule");

                    TMJsonObject a = TMJsonObjectCreate();
                    TMJsonObjectSetName(&a, "A");
                    TMJsonObjectSetValue(&a, entity->collision->capsule.a.x);
                    TMJsonObjectSetValue(&a, entity->collision->capsule.a.y);

                    TMJsonObject b = TMJsonObjectCreate();
                    TMJsonObjectSetName(&b, "B");
                    TMJsonObjectSetValue(&b, entity->collision->capsule.b.x);
                    TMJsonObjectSetValue(&b, entity->collision->capsule.b.y);

                    TMJsonObject r = TMJsonObjectCreate();
                    TMJsonObjectSetName(&r, "Radio");
                    TMJsonObjectSetValue(&r, entity->collision->capsule.r);

                    TMJsonObjectAddChild(&capsule, &a);
                    TMJsonObjectAddChild(&capsule, &b);
                    TMJsonObjectAddChild(&capsule, &r);

                    TMJsonObjectAddChild(&jsonCollision, &capsule);

                } break;

            }
 
            TMJsonObjectAddChild(&jsonEntity, &jsonCollision);

        }

        TMJsonObjectAddChild(&jsonScene, &jsonEntity);

    }

    TMJsonObjectAddChild(&jsonRoot, &jsonScene);

    int bytesCount = 0;
    TMJsonObjectStringify(&jsonRoot, NULL, &bytesCount);
    
    char *buffer = (char *)malloc(bytesCount + 1);
    int bytesWriten = 0;
    TMJsonObjectStringify(&jsonRoot, buffer, &bytesWriten);
    printf("%s", buffer);
    
    TMFileWriteText("../../assets/json/testScene.json", buffer, bytesWriten);
    free(buffer);

    TMJsonObjectFree(&jsonRoot);
}

void EditorUIInitialize(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel) {

    LoadFileNamesFromDirectory("../../assets/images", &ui->texturesNames);
    LoadFileNamesFromDirectory("../../assets/shaders", &ui->shadersNames);

    ui->options = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0, 2}, {6, 0.4}, {0.1, 0.1, 0.1, 1});
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Textures ", {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Colors ",   {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Prefabs ",   {1, 1, 1, 1}, OptionSelected, state);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Clear Brush ",   {1, 1, 1, 1}, ClearSelected, state);

    ui->textures = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.4f, 0.1f, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    ui->texturesChilds[0] = TMUIElementGetChild(ui->textures, 2);
    ui->texturesChilds[1] = TMUIElementGetChild(ui->textures, 1);
    ui->texturesChilds[2] = TMUIElementGetChild(ui->textures, 0);

    TMUIElement *child = NULL;
    ui->colors = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    child = TMUIElementGetChild(ui->colors, 0);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.1f + 0.1f*(float)i, 0, 0, 1}, ElementSelected, state);
    }
    child = TMUIElementGetChild(ui->colors, 1);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.1f + 0.1f*(float)i, 0, 1}, ElementSelected, state);
    }
    child = TMUIElementGetChild(ui->colors, 2);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0, 0.1f + 0.1f*(float)i, 1}, ElementSelected, state);
    }

    ui->prefabs = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->prefabs, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    child = TMUIElementGetChild(ui->prefabs, 0);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.5f, 0.5f, 1}, EnemyPrefabSelected, state);
    child = TMUIElementGetChild(ui->prefabs, 1);
    TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.5f, 0.5f, 0, 1}, PlayerPrefabSelected, state);

    ui->modify = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {8.0, 0}, {4.8, 2}, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildLabel(ui->modify, TM_UI_ORIENTATION_VERTICAL,   " Scale ", {1, 1, 1, 1}, ScaleEntity, state);
    TMUIElementAddChildLabel(ui->modify, TM_UI_ORIENTATION_VERTICAL,   " Translate ", {1, 1, 1, 1}, TranslateEntity, state);
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1});
    child = TMUIElementGetChild(ui->modify, 2);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc absU ", {1, 1, 1, 1}, IncrementAbsU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc absV ", {1, 1, 1, 1}, IncrementAbsV, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off absU ", {1, 1, 1, 1}, OffsetAbsU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off absV ", {1, 1, 1, 1}, OffsetAbsV, state);
    child = TMUIElementGetChild(ui->modify, 3);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc relU ", {1, 1, 1, 1}, IncrementRelU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc relV ", {1, 1, 1, 1}, IncrementRelV, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off relU ", {1, 1, 1, 1}, OffsetRelU, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off relV ", {1, 1, 1, 1}, OffsetRelV, state);
    child = TMUIElementGetChild(ui->modify, 4);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc z ", {1, 1, 1, 1}, IncrementZ, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " dec z ", {1, 1, 1, 1}, DecrementZ, state);
    child = TMUIElementGetChild(ui->modify, 5);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " add Collider ", {1, 1, 1, 1}, AddCollision, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " rem Collider ", {1, 1, 1, 1}, RemCollision, state);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Solid ",        {1, 1, 1, 1}, SolidCollision, state);
    
    ui->save = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0.0f, height/meterToPixel - 0.25f}, {7.5, 0.25}, {0.1f, 0.1f, 0.1f, 1.0f});
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Save Scene ", {1, 1, 1, 1}, SaveScene, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Texture ", {1, 1, 1, 1}, LoadTexture, state);
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Load Shader ", {1, 1, 1, 1}, LoadShader, state);

    ui->loadTexture = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {2.5f, height/meterToPixel - 0.25f - 4.0f}, {2.5, 4}, {0.02f, 0.02f, 0.02f, 1.0f});
    if(ui->texturesNames) {
        for(int i = 0; i < TMDarraySize(ui->texturesNames); ++i) {
            TMUIElementAddChildLabel(ui->loadTexture, TM_UI_ORIENTATION_VERTICAL, ui->texturesNames[i], {1, 1, 1, 1}, SelectTexture, state);
        }
    }

    ui->loadShader = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {5.0f, height/meterToPixel - 0.25f - 4.5f}, {2.5, 4.5}, {0.02f, 0.02f, 0.02f, 1.0f});
    if(ui->shadersNames) {
        for(int i = 0; i < TMDarraySize(ui->shadersNames); ++i) {
            TMUIElementAddChildLabel(ui->loadShader, TM_UI_ORIENTATION_VERTICAL, ui->shadersNames[i], {1, 1, 1, 1}, SelectShader, state);
        }
    }

}

void EditorUIUpdate(EditorState *state, EditorUI *ui, float width, float height, float meterToPixel) {

    TMVec3 pos = state->cameraP;
    TMUIElementProcessInput(ui->save, pos.x, pos.y, width, height, meterToPixel);
    TMUIElementProcessInput(ui->options, pos.x, pos.y, width, height, meterToPixel);
    if(state->option == OPTION_TEXTURE) {
        TMUIElementProcessInput(ui->textures, pos.x, pos.y, width, height, meterToPixel);
    }
    else if (state->option == OPTION_COLOR) {
        TMUIElementProcessInput(ui->colors, pos.x, pos.y, width, height, meterToPixel);
    }
    else if (state->option == OPTION_PREFABS) {
        TMUIElementProcessInput(ui->prefabs, pos.x, pos.y, width, height, meterToPixel);
    }

    if(!state->element && state->selectedEntity) {
        TMUIElementProcessInput(ui->modify, pos.x, pos.y, width, height, meterToPixel);
    }

    if(state->loadOption == LOAD_OPTION_TEXTURE) {
        TMUIElementProcessInput(ui->loadTexture, pos.x, pos.y, width, height, meterToPixel);
    }

    if(state->loadOption == LOAD_OPTION_SHADER) {
        TMUIElementProcessInput(ui->loadShader, pos.x, pos.y, width, height, meterToPixel);
    }

    state->mouseIsHot = false;
    TMUIMouseIsHot(ui->options,  &state->mouseIsHot);
    TMUIMouseIsHot(ui->textures, &state->mouseIsHot);
    TMUIMouseIsHot(ui->colors,   &state->mouseIsHot);
    TMUIMouseIsHot(ui->prefabs,   &state->mouseIsHot);
    TMUIMouseIsHot(ui->save,     &state->mouseIsHot);
    if(!state->element && state->selectedEntity) {
        TMUIMouseIsHot(ui->modify, &state->mouseIsHot);
    }
    if(state->loadOption == LOAD_OPTION_TEXTURE) {
        TMUIMouseIsHot(ui->loadTexture, &state->mouseIsHot);
    }
    if(state->loadOption == LOAD_OPTION_SHADER) {
        TMUIMouseIsHot(ui->loadShader, &state->mouseIsHot);
    }

}

void EditorUIDraw(EditorState *state, EditorUI *ui, TMRenderer *renderer) {

    TMUIElementDraw(renderer, ui->options, 0.0f);
    TMUIElementDraw(renderer, ui->save, 0.0f);

    if(state->option == OPTION_TEXTURE) {
        TMUIElementDraw(renderer, ui->textures, 0.0f);
    }
    else if (state->option == OPTION_COLOR) {
        TMUIElementDraw(renderer, ui->colors, 0.0f);
    }
    else if(state->option == OPTION_PREFABS) {
        TMUIElementDraw(renderer, ui->prefabs, 0.0f);
    }

    if(state->loadOption == LOAD_OPTION_TEXTURE) {
        TMUIElementDraw(renderer, ui->loadTexture, 0.0f);
    }
    if(state->loadOption == LOAD_OPTION_SHADER) {
        TMUIElementDraw(renderer, ui->loadShader, 0.0f);
    }

    if(!state->element && state->selectedEntity) {
        TMUIElementDraw(renderer, ui->modify, 0.0f);
    }

#if 0
    if(state->element) {

        TMUIElement *viewport = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL,
                                                        {10.8, 0}, {2, 2},
                                                        {0.1f, 0.1f, 0.1f, 1});
        if(state->element->type == TM_UI_TYPE_IMAGE_BUTTON) {
            TMUIElement *element = state->element;
            TMUIElementAddChildImageButton(viewport, TM_UI_ORIENTATION_VERTICAL, gTexture, element->absUVs, element->relUVs);
        }
        else if (state->element->type == TM_UI_TYPE_BUTTON) {
            TMUIElement *element = state->element;
            TMUIElementAddChildButton(viewport, TM_UI_ORIENTATION_HORIZONTAL, element->oldColor);
        }
        TMUIElementDraw(renderer, viewport, 0.0f);
        TMUIElementDestroy(viewport);
    }
#endif

}

void EditorUIShutdown(EditorUI *ui) {

    TMUIElementDestroy(ui->save);
    TMUIElementDestroy(ui->modify);
    TMUIElementDestroy(ui->colors);
    TMUIElementDestroy(ui->textures);
    TMUIElementDestroy(ui->options);
    TMUIElementDestroy(ui->loadTexture);
    TMUIElementDestroy(ui->loadShader);
    FreeFileNames(&ui->shadersNames);
    FreeFileNames(&ui->texturesNames);

}
