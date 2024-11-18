//
//  main.cpp
//  3D Object Drawing
//
//  Created by Nazirul Hasan on 4/9/23.
//  modified by Badiuzzaman on 3/11/24.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawTableChair(unsigned int VAO, Shader ourShader);
void drawFan(unsigned int VAO, Shader ourShader);

glm::mat4 createRotateYMatrix(float angle) {
    glm::mat4 rotateYMatrix(1.0f);
    float radians = glm::radians(angle);
    rotateYMatrix[0][0] = cos(radians);
    rotateYMatrix[0][2] = sin(radians);
    rotateYMatrix[2][0] = -sin(radians);
    rotateYMatrix[2][2] = cos(radians);
    return rotateYMatrix;
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 0.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 5.0, eyeY = 1.5, eyeZ = 7.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

float r = 0.0f;
bool fanOn = false;

bool birdEyeView = false;
glm::vec3 birdEyePosition(1.0f, 2.5f, 3.0f); // Initial position (10 units above)
glm::vec3 birdEyeTarget(1.0f, 0.0f, 0.0f);   // Focus point
float birdEyeSpeed = 1.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,

    };
    unsigned int cube_indices[] = {
        1, 2, 3,
        3, 0, 1,

        5, 6, 7,
        7, 4, 5,

        4, 7, 3,
        3, 0, 4,

        5, 6, 2,
        2, 1, 5,

        5, 1, 0,
        0, 4, 5,

        6, 2, 3,
        3, 7, 6,
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ourShader.use();
    //constantShader.use();
    r = 0.0f;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(basic_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        //constantShader.setMat4("projection", projection);

        glm::mat4 view;

        if (birdEyeView) {
            // Set camera position directly above the scene
            glm::vec3 up(0.0f, 1.0f, 0.0f); // Ensure the up vector points backward
            view = glm::lookAt(birdEyePosition, birdEyeTarget, up);
        }
        else {
            view = basic_camera.createViewMatrix();
        }

        ourShader.setMat4("view", view);
        // camera/view transformation
        //glm::mat4 view = basic_camera.createViewMatrix();
        //ourShader.setMat4("view", view);
        //constantShader.setMat4("view", view);

        // Modelling Transformation
        //glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //drawCube(ourShader, VAO, identityMatrix, translate_X, translate_Y, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, scale_X, scale_Y, scale_Z);
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;

        drawTableChair(VAO, ourShader);
        drawFan(VAO, ourShader);

        //floor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, -1.0f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, -0.2f, 14.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.494f, 0.514f, 0.541f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //front wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 7.0f, -0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.659f, 0.820f, 0.843f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall section 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, -1.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 7.0f, 14.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //roof
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 2.5f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 0.2f, 14.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.494f, 0.514f, 0.541f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //whiteboard
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 3.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawFan(unsigned int VAO, Shader ourShader) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;

    if (fanOn) {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.95f, 2.5f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan middle
        rotateYMatrix = createRotateYMatrix(r);
        //rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8f, 2.0f, -0.15f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, -0.2f, 0.8f));
        model = translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8f, 2.0f, -0.05f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.5f, -0.2f, 0.4f));
        model = translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.2f, 2.0f, -0.05f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, -0.2f, 0.4f));
        model = translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 2.0f, -0.15f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, -1.5f));
        model = translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 2.0f, 0.25f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, 0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, -0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, 1.5f));
        model = translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        r += 0.5f;
    }

    else {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.95f, 2.5f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan middle
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8f, 2.0f, -0.15f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, -0.2f, 0.8f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8f, 2.0f, -0.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.5f, -0.2f, 0.4f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.2f, 2.0f, -0.05f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, -0.2f, 0.4f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 2.0f, -0.15f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, -1.5f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 2.0f, 0.25f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, 1.5f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void drawTableChair(unsigned int VAO, Shader ourShader) {
    //table top
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.2f, 2.0f));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.5f, 0.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.882f, 0.710f, 0.604f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table leg left back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.5f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.647f, 0.408f, 0.294f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table leg right back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.9f, -0.5f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table leg left front
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.5f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table leg right frint
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.9f, -0.5f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.455f, 0.235f, 0.102f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.329f, 0.173f, 0.110f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper piller left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper piller right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, 0.15f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.20f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.455f, 0.235f, 0.102f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.329f, 0.173f, 0.110f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.25f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper piller left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.25f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper piller right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.65f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.25f, 0.15f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.25f, -0.20f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.75f, -0.5f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.455f, 0.235f, 0.102f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    
    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.75f, -0.5f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.329f, 0.173f, 0.110f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.75f, -0.5f, 0.65f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.35f, -0.5f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.35f, -0.5f, 0.65f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    //chair upper piller left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.75f, -0.4f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper piller right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.75f, -0.4f, 0.65f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    //chair upper line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.75f, 0.15f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 1.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.75f, -0.20f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 1.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        if (fanOn) {
            fanOn = false;
        }
        else {
            fanOn = true;
        }
    }
    
    if (birdEyeView) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            birdEyePosition.z -= birdEyeSpeed * deltaTime; // Move forward along Z
            birdEyeTarget.z -= birdEyeSpeed * deltaTime;
            if (birdEyePosition.z <= -1.0) {
                birdEyePosition.z = -1.0;
            }
            if (birdEyeTarget.z <= -4.0) {
                birdEyeTarget.z = -4.0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            birdEyePosition.z += birdEyeSpeed * deltaTime; // Move backward along Z
            birdEyeTarget.z += birdEyeSpeed * deltaTime;
            if (birdEyePosition.z >= 3.0) {
                birdEyePosition.z = 3.0;
            }
            if (birdEyeTarget.z >= 0.0) {
                birdEyeTarget.z = 0.0;
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        birdEyeView = !birdEyeView;
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.01;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAngle_Z += 1;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        lookAtX += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        lookAtX -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        lookAtY += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        lookAtY -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    basic_camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    basic_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}