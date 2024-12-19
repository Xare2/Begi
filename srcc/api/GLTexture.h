#pragma once
#include "Texture.h"
class GLTexture :
    public Texture
{
public:

    GLTexture();
    GLTexture(std::string filename);
    GLTexture(const std::string& left, const std::string& right,
        const std::string& front, const std::string& back,
        const std::string& top, const std::string& bottom);

    GLTexture(const std::string& name, const std::string& format, int frame_range);

    void bind(size_t layer = 0);
    void update() override;

    GLuint getTexId();

protected:
    GLuint glTexId = -1;

    int getTextureMode();
};

