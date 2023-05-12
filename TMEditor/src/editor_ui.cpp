#include "editor_ui.h"
#include "editor.h"

#include <stdio.h>

extern EditorState *gState;
extern const char  *gImages[7];
extern TMHashmap   *gAbsUVs;
extern TMTexture   *gTexture;

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

static void OptionSelected(TMUIElement *element) {
    printf("Option selected\n");
    gState->option = (BrushOption)element->index;
    gState->modifyOption = MODIFY_NONE;
    gState->selectedEntity = NULL;
}

static void ClearSelected(TMUIElement *element) {
    printf("Clear selected\n");
    gState->element = NULL;
}

static void ElementSelected(TMUIElement *element) {
    printf("Element selected\n");
    gState->element = element;
    gState->modifyOption = MODIFY_NONE;
    gState->selectedEntity = NULL;
}

static void TranslateEntity(TMUIElement *element) { gState->modifyOption = MODIFY_TRANSLATE; }
static void ScaleEntity(TMUIElement *element)     { gState->modifyOption = MODIFY_SCALE;     }
static void IncrementAbsU(TMUIElement *element)   { gState->modifyOption = MODIFY_INC_ABS_U; }
static void IncrementAbsV(TMUIElement *element)   { gState->modifyOption = MODIFY_INC_ABS_V; }
static void OffsetAbsU(TMUIElement *element)      { gState->modifyOption = MODIFY_OFF_ABS_U; }
static void OffsetAbsV(TMUIElement *element)      { gState->modifyOption = MODIFY_OFF_ABS_V; }
static void IncrementRelU(TMUIElement *element)   { gState->modifyOption = MODIFY_INC_REL_U; }
static void IncrementRelV(TMUIElement *element)   { gState->modifyOption = MODIFY_INC_REL_V; }
static void OffsetRelU(TMUIElement *element)      { gState->modifyOption = MODIFY_OFF_REL_U; }
static void OffsetRelV(TMUIElement *element)      { gState->modifyOption = MODIFY_OFF_REL_V; }


static void IncrementZ(TMUIElement *element) {

    gState->modifyOption = MODIFY_NONE;

    if(gState->selectedEntity) {
        Entity *entity = gState->selectedEntity;
        entity->zIndex++;
        printf("zIndex: %d\n", entity->zIndex);
    }

}

static void DecrementZ(TMUIElement *element) {

    gState->modifyOption = MODIFY_NONE;

    if(gState->selectedEntity) {
        Entity *entity = gState->selectedEntity;
        entity->zIndex--;
        entity->zIndex = MaxI32(entity->zIndex, 1);
        printf("zIndex: %d\n", entity->zIndex);
    }

}



static void AddCollisionToEntity(Entity *entity, CollisionType type) {

    if(!entity->collision) {

        entity->collision = (Collision *)malloc(sizeof(Collision));
        Collision *collision = entity->collision;

        collision->type = type;
        collision->solid = true;
        
        switch(type) {
        
            case COLLISION_TYPE_AABB: 
            {
                AABB aabb;

                aabb.min = entity->position;
                aabb.max = entity->position + entity->size;

                collision->aabb = aabb;
            } break;
            case COLLISION_TYPE_CIRCLE:
            {
                //collision->circle
                // TODO: ....
            } break;
            case COLLISION_TYPE_CAPSULE:
            {
                //collision->capsule
                // TODO: ....
            } break;

        }

    }

}

static void RemoveCollisionToEntity(Entity *entity) {
    if(entity->collision) free(entity->collision);
    entity->collision = NULL;
}


static void AddCollision(TMUIElement *element) {

    gState->modifyOption = MODIFY_NONE;

    if(gState->selectedEntity) {
        Entity *entity = gState->selectedEntity;
        AddCollisionToEntity(entity, COLLISION_TYPE_AABB);
    }

}

static void RemCollision(TMUIElement *element) {

    gState->modifyOption = MODIFY_NONE;

    if(gState->selectedEntity) {
        Entity *entity = gState->selectedEntity;
        RemoveCollisionToEntity(entity);
    }

}

static void SolidCollision(TMUIElement *element) {

    gState->modifyOption = MODIFY_NONE;
    if(gState->selectedEntity) {
        Entity *entity = gState->selectedEntity;
        if(entity->collision) {
            entity->collision->solid = !entity->collision->solid;
        }
    }

}

static void SaveScene(TMUIElement *element) {
    printf("Scene Saved\n");

    TMJsonObject jsonRoot = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonRoot, "Root");

    TMJsonObject jsonScene = TMJsonObjectCreate();
    TMJsonObjectSetName(&jsonScene, "Scene");

    for(int i = 0; i < TMDarraySize(gState->entities); ++i) {
        Entity *entity = gState->entities + i;

        TMJsonObject jsonEntity = TMJsonObjectCreate();
        TMJsonObjectSetName(&jsonEntity, "Entity");
        
        // Save Graphic Component
        {
            TMJsonObject jsonGraphic = TMJsonObjectCreate();
            TMJsonObjectSetName(&jsonGraphic, "Graphics");

            // graphics type
            TMJsonObject jsonType = TMJsonObjectCreate();

            TMJsonObjectSetName(&jsonType, "Type");
            if(entity->shader == gState->colorShader) {
                TMJsonObjectSetValue(&jsonType, 0.0f);
            }
            else if(entity->shader == gState->spriteShader) {
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

            TMJsonObjectAddChild(&jsonGraphic, &jsonType);
            TMJsonObjectAddChild(&jsonGraphic, &jsonPosition);
            TMJsonObjectAddChild(&jsonGraphic, &jsonSize);
            TMJsonObjectAddChild(&jsonGraphic, &jsonColor);
            TMJsonObjectAddChild(&jsonGraphic, &jsonAbsUVs);
            TMJsonObjectAddChild(&jsonGraphic, &jsonRelUVs);
        
            TMJsonObjectAddChild(&jsonEntity, &jsonGraphic);

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

void EditorUIInitialize(EditorUI *ui, float width, float height, float meterToPixel) {

    ui->options = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0, 2}, {6, 0.4}, {0.1, 0.1, 0.1, 1});
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Textures ", {1, 1, 1, 1}, OptionSelected);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Colors ",   {1, 1, 1, 1}, OptionSelected);
    TMUIElementAddChildLabel(ui->options, TM_UI_ORIENTATION_VERTICAL, " Clear Brush ",   {1, 1, 1, 1}, ClearSelected);

    ui->textures = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.1f, 0.4f, 0.1f, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->textures, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElement *child = TMUIElementGetChild(ui->textures, 0);
    for(int i = 0; i < ARRAY_LENGTH(gImages); ++i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, gTexture, {0, 0, 1, 1}, Texture(gAbsUVs, gImages[i]), ElementSelected);
    }
    child = TMUIElementGetChild(ui->textures, 1);
    for(int i = ARRAY_LENGTH(gImages) - 1; i >= 0;  --i) {
        TMUIElementAddChildImageButton(child, TM_UI_ORIENTATION_VERTICAL, gTexture, {0, 0, 1, 1}, Texture(gAbsUVs, gImages[i]), ElementSelected);
    }

    ui->colors = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {0, 0}, {6, 2}, {0.4f, 0.1f, 0.1f, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    TMUIElementAddChildButton(ui->colors, TM_UI_ORIENTATION_HORIZONTAL, {0.2, 0.2, 0.2, 1});
    child = TMUIElementGetChild(ui->colors, 0);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0.1f + 0.1f*(float)i, 0, 0, 1}, ElementSelected);
    }
    child = TMUIElementGetChild(ui->colors, 1);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0.1f + 0.1f*(float)i, 0, 1}, ElementSelected);
    }
    child = TMUIElementGetChild(ui->colors, 2);
    for(int i = 0; i < 8; ++i) {
        TMUIElementAddChildButton(child, TM_UI_ORIENTATION_VERTICAL, {0, 0, 0.1f + 0.1f*(float)i, 1}, ElementSelected);
    }

    ui->modify = TMUIElementCreateButton(TM_UI_ORIENTATION_VERTICAL, {8.0, 0}, {4.8, 2}, {0.1f, 0.1f, 0.1f, 1});
    TMUIElementAddChildLabel(ui->modify, TM_UI_ORIENTATION_VERTICAL,   " Scale ", {1, 1, 1, 1}, ScaleEntity);
    TMUIElementAddChildLabel(ui->modify, TM_UI_ORIENTATION_VERTICAL,   " Translate ", {1, 1, 1, 1}, TranslateEntity);
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1}, TranslateEntity);
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1}, TranslateEntity);
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1}, TranslateEntity);
    TMUIElementAddChildButton(ui->modify, TM_UI_ORIENTATION_HORIZONTAL, {0.1f, 0.1f, 0.1f, 1}, TranslateEntity);
    child = TMUIElementGetChild(ui->modify, 2);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc absU ", {1, 1, 1, 1}, IncrementAbsU);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc absV ", {1, 1, 1, 1}, IncrementAbsV);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off absU ", {1, 1, 1, 1}, OffsetAbsU);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off absV ", {1, 1, 1, 1}, OffsetAbsV);
    child = TMUIElementGetChild(ui->modify, 3);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc relU ", {1, 1, 1, 1}, IncrementRelU);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc relV ", {1, 1, 1, 1}, IncrementRelV);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off relU ", {1, 1, 1, 1}, OffsetRelU);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " off relV ", {1, 1, 1, 1}, OffsetRelV);
    child = TMUIElementGetChild(ui->modify, 4);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " inc z ", {1, 1, 1, 1}, IncrementZ);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " dec z ", {1, 1, 1, 1}, DecrementZ);
    child = TMUIElementGetChild(ui->modify, 5);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " add Collider ", {1, 1, 1, 1}, AddCollision);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " rem Collider ", {1, 1, 1, 1}, RemCollision);
    TMUIElementAddChildLabel(child, TM_UI_ORIENTATION_VERTICAL, " Solid ",        {1, 1, 1, 1}, SolidCollision);
    
    ui->save = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL, {0.0f, height/meterToPixel - 0.25f}, {2.5, 0.25}, {0.1f, 0.1f, 0.1f, 1.0f});
    TMUIElementAddChildLabel(ui->save, TM_UI_ORIENTATION_VERTICAL, " Save Scene ", {1, 1, 1, 1}, SaveScene);

}

void EditorUIUpdate(EditorUI *ui, float width, float height, float meterToPixel) {

    TMVec3 pos = gState->cameraP;
    TMUIElementProcessInput(ui->save, pos.x, pos.y, width, height, meterToPixel);
    TMUIElementProcessInput(ui->options, pos.x, pos.y, width, height, meterToPixel);
    if(gState->option == OPTION_TEXTURE) {
        TMUIElementProcessInput(ui->textures, pos.x, pos.y, width, height, meterToPixel);
    }
    else if (gState->option == OPTION_COLOR){
        TMUIElementProcessInput(ui->colors, pos.x, pos.y, width, height, meterToPixel);
    }
    if(!gState->element && gState->selectedEntity) {
        TMUIElementProcessInput(ui->modify, pos.x, pos.y, width, height, meterToPixel);
    }

    gState->mouseIsHot = false;
    TMUIMouseIsHot(ui->options,  &gState->mouseIsHot);
    TMUIMouseIsHot(ui->textures, &gState->mouseIsHot);
    TMUIMouseIsHot(ui->colors,   &gState->mouseIsHot);
    TMUIMouseIsHot(ui->save,     &gState->mouseIsHot);
    if(!gState->element && gState->selectedEntity) {
        TMUIMouseIsHot(ui->modify, &gState->mouseIsHot);
    }

}

void EditorUIDraw(EditorUI *ui, TMRenderer *renderer) {

    TMUIElementDraw(renderer, ui->options, 0.0f);
    TMUIElementDraw(renderer, ui->save, 0.0f);

    if(gState->option == OPTION_TEXTURE) {
        TMUIElementDraw(renderer, ui->textures, 0.0f);
    }
    else if (gState->option == OPTION_COLOR) {
        TMUIElementDraw(renderer, ui->colors, 0.0f);
    }

    if(!gState->element && gState->selectedEntity) {
        TMUIElementDraw(renderer, ui->modify, 0.0f);
    }

    if(gState->element) {

        TMUIElement *viewport = TMUIElementCreateButton(TM_UI_ORIENTATION_HORIZONTAL,
                                                        {10.8, 0}, {2, 2},
                                                        {0.1f, 0.1f, 0.1f, 1});
        if(gState->element->type == TM_UI_TYPE_IMAGE_BUTTON) {
            TMUIElement *element = gState->element;
            TMUIElementAddChildImageButton(viewport, TM_UI_ORIENTATION_VERTICAL, gTexture, element->absUVs, element->relUVs);
        }
        else if (gState->element->type == TM_UI_TYPE_BUTTON) {
            TMUIElement *element = gState->element;
            TMUIElementAddChildButton(viewport, TM_UI_ORIENTATION_HORIZONTAL, element->oldColor);
        }
        TMUIElementDraw(renderer, viewport, 0.0f);
        TMUIElementDestroy(viewport);
    }

}

void EditorUIShutdown(EditorUI *ui) {

    TMUIElementDestroy(ui->save);
    TMUIElementDestroy(ui->modify);
    TMUIElementDestroy(ui->colors);
    TMUIElementDestroy(ui->textures);
    TMUIElementDestroy(ui->options);

}
