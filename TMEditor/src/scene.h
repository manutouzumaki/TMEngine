#ifndef _SCENE_H_
#define _SCENE_H_

struct EditorState;
struct TMUIElement;

void LoadSceneFromFile(EditorState *state, char *filepath);
void SaveScene(TMUIElement *element);

#endif
