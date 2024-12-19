#pragma once
#include "GLTexture.h"

class GLTextureFB :
    public GLTexture
{
private:
    static inline int fbCount = 0;
    unsigned int fbID;

public:
    GLTextureFB(int type, int width, int height);

    int getId();
};
