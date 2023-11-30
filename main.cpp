///////////////////////////////////////////////////////////////////////////////
// A simple mesh viewer application.
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>

#include <iostream>
#include <vector>

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include "mesh.h"
#include "shader.h"

namespace
{

// Constants.
const int k_screen_height = 512;
const int k_screen_width = 512;
const float k_near_plane = 0.1f;
const float k_far_plane = 2000.0f;
const char* k_screen_title = "OpenGL Scene";
const float k_range = 5.0f; // 5.0
const float k_pi = 3.141592;
const float k_fov = k_pi / 2.0;

const char* solid_vert = "shaders/vertex.glsl";
const char* solid_frag = "shaders/fragment.glsl";

struct object_t
{
    glm::mat4 model = glm::mat4(1.0f);           // Local to inertial matrix.
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f); // x, y, z
    GLuint vao = 0;                              // vertex array object
    GLuint icount = 0;                           // number of indices
    GLuint shader = 0;                           // Shader program.
};

struct camera_t
{
    glm::mat4 view = glm::mat4(1.0f);            // View matrix.
    glm::mat4 proj = glm::mat4(1.0f);            // Projection matrix.
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f); // x, y, z
    float fov = k_fov;                           // Field of view, radians
};

struct scene_t
{
    object_t obj; // one object in the scene
    camera_t cam; // camera
};

} // namespace

// Read object from mesh file.
bool load_scene_object(object_t& obj, const char* mesh_file)
{
    mesh_t mesh(mesh_file);

    GLuint vao; // vertex array object
    GLuint vbo; // position array buffer object
    GLuint nbo; // normal array buffer object
    GLuint cbo; // normal array buffer object
    GLuint ebo; // element array buffer object
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &nbo);
    glGenBuffers(1, &cbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // Position array.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.verts * 3 * sizeof(GLfloat), mesh.vpos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Normal array.
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.verts * 3 * sizeof(GLfloat), mesh.vnorm, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // Color array.
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.verts * 3 * sizeof(GLfloat), mesh.vcolor, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    // Element array.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces * 3 * sizeof(GLuint), mesh.elem, GL_STATIC_DRAW);

    obj.vao = vao;
    obj.icount = mesh.faces * 3;

    glBindVertexArray(0); // unbind vao
    return true;          // got object data
}

void set_camera_dyn(scene_t& scene)
{
    scene.cam.view = glm::mat4(1.0f);
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    scene.cam.view = glm::rotate(scene.cam.view, -k_pi, axis);
    scene.cam.view = glm::translate(scene.cam.view, scene.cam.pos);

    // Projection matrix.
    float aspect_ratio = (float)k_screen_width / k_screen_height;
    scene.cam.proj = glm::mat4(1.0f);
    scene.cam.proj = glm::perspective(scene.cam.fov, aspect_ratio, k_near_plane, k_far_plane);
}

void render(scene_t& scene)
{
    // GLuint shader = scene.obj.shader;
    glUseProgram(scene.obj.shader);

    glm::mat4 mvp = scene.cam.proj * scene.cam.view * scene.obj.model;
    GLuint mvp_loc = glGetUniformLocation(scene.obj.shader, "MVP");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &mvp[0][0]);

    GLuint model_loc = glGetUniformLocation(scene.obj.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &(scene.obj.model[0][0]));

    // Bind mesh data and draw mesh.
    glBindVertexArray(scene.obj.vao);
    glDrawElements(GL_TRIANGLES, scene.obj.icount, GL_UNSIGNED_INT, 0);
    glUseProgram(0);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <mesh file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the main window.
    GLFWwindow* window = glfwCreateWindow(k_screen_width, k_screen_height, k_screen_title, NULL, NULL);

    if (window == NULL)
    {
        fprintf(stderr, "Error creating GLFW window.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    // Load and initialize GLAD.
    if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Error initializing GLAD.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    scene_t scene;
    scene.cam.pos.x = 0.0f;
    scene.cam.pos.y = 0.0f;
    scene.cam.pos.z = k_range;

    // Load the mesh.
    load_scene_object(scene.obj, argv[1]);
    scene.obj.shader = create_shader_program(solid_vert, NULL, solid_frag);
    scene.obj.model = glm::mat4(1.0f);

    // Main render loop ///////////////////////////////////////////////////////

    while (not glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        usleep(10000);

        // Rotate the object.
        glm::vec3 axis(0.0f, 1.0f, 0.0f);
        scene.obj.model = glm::rotate(scene.obj.model, 0.0174533f, axis);

        set_camera_dyn(scene);
        render(scene);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
