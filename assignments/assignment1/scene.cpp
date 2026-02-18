#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

struct FullScreenQuad
{
    GLuint vao;
    GLuint vbo;

    void Initialize()
    {
        float vertices[] = {
            // pos (x, y), texcoord (u, v)

            // triangle 1
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            // triangle 2
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        // always last
        glBindVertexArray(0);
    }
} fullscreen_quad;

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
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/texture.vs", "assets/shaders/blinnphong.fs");
    postprocess_blur = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/blur.fs");
    postprocess_chromatic = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/chromatic.fs");
    postprocess_edgedetect = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/edgedetect.fs");
    postprocess_gray = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/grayscale.fs");
    postprocess_invert = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/invert.fs");
    postprocess_sharpen = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/sharpen.fs");
    postprocess_vignette = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/vignette.fs");
    postprocess_lensdistortion = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/lensdistortion.fs");
    postprocess_filmgrain = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/filmgrain.fs");
    postprocess_gammacorrection = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/gammacorrection.fs");

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 0.0f, 1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };

    fullscreen_quad.Initialize();

    // framebuffer setup
    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    {
        // create texture
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);  
        
        // create texture
        glGenTextures(1, &fbo_depth);
        glBindTexture(GL_TEXTURE_2D, fbo_depth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo_depth, 0);  
        
        // cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("It's not complete :(\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene()
{
    glDeleteFramebuffers(1, &fbo);
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

static int current_item_index = 0;

void Scene::Render(void)
{
    // suzanne pipeline
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        const auto view_proj = camera.Projection() * camera.View();

        // bind texture
        glBindTextureUnit(0, brickTexture.getID());

        blinnphong->use();

        // scene matrices
        blinnphong->setMat4("model", glm::mat4(1.0f));
        blinnphong->setMat4("view_proj", view_proj);
        
        blinnphong->setInt("texture2d", 0);
        blinnphong->setVec3("camera", camera.position);
        blinnphong->setVec3("light.position", light.position);
        blinnphong->setVec3("light.color", light.color);

        blinnphong->setVec3("material.ambient", debug.ambient);
        blinnphong->setVec3("material.color", debug.material_color);
        blinnphong->setVec3("material.diffuse", debug.diffuse);
        blinnphong->setVec3("material.specular", debug.specular);
        blinnphong->setFloat("material.shininess", debug.shininess);
        blinnphong->setFloat("material.alpha", debug.alpha);

        // draw suzanne
        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    {   // post processing pipeline
        // render fullscreen quad
        switch (current_item_index) {
            // gray
            case 0:
            {
                postprocess_gray->use();
                break;
            }
            // blur
            case 1:
            {
                postprocess_blur->use();
                postprocess_blur->setInt("screen", 0);
                break;
            }
            // edge detect
            case 2:
            {
                postprocess_edgedetect->use();
                break;
            }
            // invert
            case 3:
            {
                postprocess_invert->use();
                break;
            }
            // chromatic aberration
            case 4:
            {
                postprocess_chromatic->use();
                break;
            }
            // sharpen
            case 5:
            {
                postprocess_sharpen->use();
                break;
            }
            // vignette
            case 6:
            {
                postprocess_vignette->use();
                postprocess_vignette->setVec2("resolution", glm::vec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
                break;
            }
            // lens distortion
            case 7:
            {
                postprocess_lensdistortion->use();
                postprocess_lensdistortion->setVec2("resolution", glm::vec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
                break;
            }
            // film grain
            case 8:
            {
                postprocess_filmgrain->use();
                postprocess_filmgrain->setVec2("resolution", glm::vec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
                postprocess_filmgrain->setFloat("time", (float)time.absolute);
                break;
            }
            // gamma correction
            case 9:
            {
                postprocess_gammacorrection->use();
                break;
            }
        }

        // fullscreen pipeline
        glDisable(GL_DEPTH_TEST);

        // default framebuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw fullscreen quad
        glBindVertexArray(fullscreen_quad.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Scene::Debug(void)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    glm::mat4 m{1.0f};
    auto *view = glm::value_ptr(camera.View());
    auto *proj = glm::value_ptr(camera.Projection());
    
    ImGuizmo::DrawGrid(view, proj, glm::value_ptr(m), 100.0f);

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

    ImGui::Image(
        (void*)(intptr_t)fbo_texture,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));

    ImGui::DragFloat3("Material Ambient", &debug.ambient[0], 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat3("Material Diffuse", &debug.diffuse[0], 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat3("Material Specular", &debug.specular[0], 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat("Material Shininess", &debug.shininess, 0.1f, 0.0f, 1.0f);

    const char* items[] = { "Gray", "Blur", "Edge Detect", "Invert", "Chromatic Aberration", "Sharpen", "Vignette", "Lens Distortion", "Film Grain", "Gamma Correction" };
    ImGui::Combo("Select Post Processing", &current_item_index, items, IM_ARRAYSIZE(items));

    ImGui::End();
}