#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"

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
    std::unique_ptr<ew::Shader> water;

    // mipmaps:
    std::unique_ptr<ew::Texture> wave_spec;
    std::unique_ptr<ew::Texture> wave_tex;
    std::unique_ptr<ew::Texture> wave_warp;

    // water plane
    ew::Mesh plane;

    batteries::light_t light;
};
