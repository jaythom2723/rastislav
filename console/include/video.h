#ifndef VIDEO_H
#define VIDEO_H

#include "common.h"

#include <Windows.h>

#define MAX_COLORS 255

#define RED 0
#define GREEN 1
#define BLUE 2
#define BLACK 3
#define WHITE 4

typedef struct {
    float red;
    float green;
    float blue;
} color_t;

const static color_t colors[MAX_COLORS] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f }, 
    { 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 1.0f }
};

void videoInit(rastislav_t *rastislav);
void videoProcess(rastislav_t *rastislav);
void videoUpdateConsoleReference(rastislav_t *rastislav);

#endif // VIDEO_H