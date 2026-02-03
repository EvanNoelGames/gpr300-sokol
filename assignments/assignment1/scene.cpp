#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

struct {
    float alpha = 1.0f;
    float shininess = 1.0f;
    glm::vec3 material_color = {1.0f, 1.0f, 1.0f};
    glm::vec3 ambient = {0.1f, 0.1f, 0.1f};
    glm::vec3 diffuse = {0.5f, 0.5f, 0.5f};;
    glm::vec3 specular = {0.5f, 0.5f, 0.5f};;
} debug;

Scene::Scene()
{
    skull = std::make_unique<ew::Model>("assets/models/skull.obj");
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    toon = std::make_unique<ew::Shader>("assets/shaders/texture.vs", "assets/shaders/toon.fs");

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 0.0f, 1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };

    palette = {
        .color1 = {1.0f, 0.0f, 1.0f},
        .color2 = {0.0f, 1.0f, 1.0f},
    };
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind texture
    glBindTextureUnit(0, brickTexture.getID());
    glBindTextureUnit(1, toonShadeTexture.getID());

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

    toon->use();

    // scene matrices
    toon->setMat4("model", glm::mat4(1.0f));
    toon->setMat4("view_proj", view_proj);
    
    toon->setInt("texture2d", 0);
    toon->setInt("zaToon", 1);
    toon->setVec3("camera", camera.position);
    toon->setVec3("light.position", light.position);
    toon->setVec3("light.color", light.color);

    toon->setVec3("pal.color1", palette.color1);
    toon->setVec3("pal.color2", palette.color2);

    toon->setVec3("material.ambient", debug.ambient);
    toon->setVec3("material.color", debug.material_color);
    toon->setVec3("material.diffuse", debug.diffuse);
    toon->setVec3("material.specular", debug.specular);
    toon->setFloat("material.shininess", debug.shininess);
    toon->setFloat("material.alpha", debug.alpha);

    // draw skull
    skull->draw();
}

void Scene::Debug(void)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    glm::mat4 m{1.0f};
    auto *view = glm::value_ptr(camera.View());
    auto *proj = glm::value_ptr(camera.Projection());
    
    //ImGuizmo::DrawGrid(view, proj, glm::value_ptr(m), 100.0f);

    auto light_matrix = glm::translate(glm::mat4(1.0f), light.position);
    ImGuizmo::Manipulate(
        view,
        proj,
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(light_matrix) // &light_matrix[0][0]
    );

    if (ImGuizmo::IsUsing())
    {
        light.position = glm::vec3(light_matrix[3]);
    }

    cameracontroller.Debug();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::ColorEdit3("Light Color", &light.color[0]);
    ImGui::ColorEdit3("Material Color", &debug.material_color[0]);

    ImGui::DragFloat3("Material Ambient", &debug.ambient[0], 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat3("Material Diffuse", &debug.diffuse[0], 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat3("Material Specular", &debug.specular[0], 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat("Material Shininess", &debug.shininess, 0.1f, 0.0f, 1.0f);
    //ImGui::DragFloat("Material Alpha", &debug.alpha, 0.1f, 0.0f, 1.0f);

    ImGui::SeparatorText("Palette");
    ImGui::ColorEdit3("Color1", &palette.color1[0]);
    ImGui::ColorEdit3("Color2", &palette.color2[0]);

    /* build debug ui here */

    ImGui::End();
}