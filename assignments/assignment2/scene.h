#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"
#include "batteries/opengl.h"

// ew
#include "ew/model.h"
#include "ew/mesh.h"
#include "ew/shader.h"
#include "ew/texture.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    void CreateDepthBuffer();
    void CreateFrameBuffer();

    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Shader> blinnphong;

    // post-processing
    std::unique_ptr<ew::Shader> postprocess_blur;
    std::unique_ptr<ew::Shader> postprocess_edgedetect;
    std::unique_ptr<ew::Shader> postprocess_gray;
    std::unique_ptr<ew::Shader> postprocess_invert;
    std::unique_ptr<ew::Shader> postprocess_chromatic;
    std::unique_ptr<ew::Shader> postprocess_sharpen;
    std::unique_ptr<ew::Shader> postprocess_vignette;
    std::unique_ptr<ew::Shader> postprocess_lensdistortion;
    std::unique_ptr<ew::Shader> postprocess_filmgrain;
    std::unique_ptr<ew::Shader> postprocess_gammacorrection;

    std::unique_ptr<ew::Shader> depth;

    ew::Texture brickTexture = ew::Texture("assets/textures/brick_small.jpg");

    batteries::light_t light;

    // framebuffer
    GLuint fbo;
    GLuint fbo_texture;
    GLuint fbo_depth;

    // depthbuffer
    GLuint shadow_fbo;
    GLuint shadow_depth;
    
    // plane
    ew::Mesh plane;
};
