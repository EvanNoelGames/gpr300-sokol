#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

#include "ew/procGen.h"


struct {
    glm::vec3 water_color = {1.0f, 0.0f, 1.0f};
} debug;

Scene::Scene()
{
    water = std::make_unique<ew::Shader>("assets/shaders/windwaker/water.vs", "assets/shaders/windwaker/water.fs");

    water128 = std::make_unique<ew::Texture>("assets/textures/windwaker/water128.png");
    water64 = std::make_unique<ew::Texture>("assets/textures/windwaker/water64.png");
    water32 = std::make_unique<ew::Texture>("assets/textures/windwaker/water32.png");
    water16 = std::make_unique<ew::Texture>("assets/textures/windwaker/water16.png");
    water8 = std::make_unique<ew::Texture>("assets/textures/windwaker/water8.png");

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 0.0f, 1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };

    plane.load(ew::createPlane(100.0f, 100.0f, 10));
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
    glBindTextureUnit(0, water128->getID());

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

    water->use();

    // scene matrices
    water->setMat4("model", glm::mat4(1.0f));
    water->setMat4("view_proj", view_proj);
    
    water->setInt("texture2d", 0);

    water->setFloat("time", (float)time.absolute);

    water->setVec3("camera", camera.position);
    water->setVec3("water_color", debug.water_color);
    
    // draw plane
    plane.draw();
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

    ImGui::ColorEdit3("Water Color", &debug.water_color[0]);
    //ImGui::ColorEdit3("Light Color", &light.color[0]);
    //ImGui::DragFloat("Material Alpha", &debug.alpha, 0.1f, 0.0f, 1.0f);

    /* build debug ui here */

    ImGui::End();
}