#ifndef EDITOR_UTILS_H_
#define EDITOR_UTILS_H_

struct Entity;

float MinF32(float a, float b);
float MaxF32(float a, float b);

int MinI32(int a, int b);
int MaxI32(int a, int b);
int StringLength(char *string);
void InsertionSortEntities(Entity *entities, int length);

void LoadFileNamesFromDirectory(char *path, char ***filesNames);
void FreeFileNames(char ***filesNames);

#endif
