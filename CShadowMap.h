#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class CShadowMap
{
public:
    CShadowMap(unsigned int width = 1024, unsigned int height = 1024);
    ~CShadowMap();

    void BeginRender();
    void EndRender(int screenWidth, int screenHeight);

    GLuint GetDepthTexture() const { return depthMapTexture; }

private:
    unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
    GLuint depthMapFBO;
    GLuint depthMapTexture;
};