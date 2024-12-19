#pragma once
#include "VKTexture.h"
class VKTextureFB :
    public VKTexture
{
private:
    static inline int fbCount = 0;
    unsigned int fbID;

public:
    VKTextureFB(int type, int width, int height,
        VkDevice device, VkPhysicalDevice physicalDevice, 
        VkCommandPool commandPool, VkQueue graphicsQueue);

    int getId();
};

