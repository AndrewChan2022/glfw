//========================================================================
// OpenGL triangle example
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
//! [code]

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
//#include <windows.h>

/*#define printf printf2

int __cdecl printf2(const char* format, ...)
{
    char str[1024];

    va_list argptr;
    va_start(argptr, format);
    int ret = vsnprintf(str, sizeof(str), format, argptr);
    va_end(argptr);

    OutputDebugStringA(str);

    return ret;
}*/

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;

/*static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};*/

static const Vertex vertices[3] =
{
    { { -0.06f, -0.04f }, { 1.f, 0.f, 0.f } },
    { {  0.06f, -0.04f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.06f }, { 0.f, 0.f, 1.f } }
};

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static int width;
static int height;

float RandomFloat(float min, float max) {
    // NOLINTNEXTLINE(runtime/threadsafe_fn)
    float zeroOne = rand() / (float)(RAND_MAX);
    return zeroOne * (max - min) + min;
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__)
    GLFWwindow* window = glfwCreateWindow(1920/2, 1080/2, "OpenGL Triangle", NULL, NULL);
#else
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "OpenGL Triangle", NULL, NULL);
#endif
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vcol_location = glGetAttribLocation(program, "vCol");

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, col));

    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float)height;
    printf("wh: %d %d\n", width, height);

    int framecount = 0;
    while (!glfwWindowShouldClose(window))
    {

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4 m, p, mvp, m2;
        mat4x4_identity(m);
        float timediff = (float)glfwGetTime();
        //timediff = 0;
        float angle = timediff;
        
        glUseProgram(program);
        float dx = 0, dy = 0;
        for (int i = 0; i < 100000; i++) {
            mat4x4_rotate_Z(m, m, angle);
            mat4x4_dup(m2, m );
            mat4x4_scale_aniso(m2, m2, 0.8, 0.8, 1.0);
            dx = RandomFloat(-0.8, 0.8);
            dy = RandomFloat(-0.8, 0.8);
            mat4x4_translate_in_place(m2, dx, dy, 0);
            mat4x4_ortho(p, -ratio/ratio, ratio/ratio, -1.f, 1.f, 1.f, -1.f);
            mat4x4_mul(mvp, p, m2);

            glBindVertexArray(vertex_array);
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            angle = 0.03;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        framecount++;
        if (framecount % 10 == 0) {
            printf("timediff:%f, framecount:%d fps:%.2f\n", timediff, framecount, framecount/timediff);
        }
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
