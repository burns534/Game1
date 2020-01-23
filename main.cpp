//
//  main.cpp
//  Game1
//
//  Created by Kyle Burns on 1/22/20.
//  Copyright © 2020 Kyle Burns. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Elements.h"

//static const float vertices[] = {
//    -1.0f, 0.0f, -1.0f,
//    0.0f, 0.0f, -1.0f,
//    1.0f, 0.8f, -1.0f,
//    -1.0f, 0.5f, 0.0f,
//    0.0f, 0.5f, 0.0f,
//    1.0f, -0.3f, 0.0f,
//    -1.0f, 0.0f, 1.0f,
//    0.0f, 0.0f, 1.0f,
//    1.0f, 0.0f, 1.0f
//};
//
//static const unsigned indices[] = {
//    0, 1, 4,
//    4, 3, 0,
//    1, 2, 5,
//    5, 4, 1,
//    3, 4, 7,
//    7, 6, 3,
//    4, 5, 8,
//    8, 7, 4
//};

glm::vec3 cameraPos   = glm::vec3(0.0f, 5.0f,  4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f,  -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 direction;

float deltaTime = 0.0f;    // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float yaw = -90.0f; float pitch = -40.0f;

float lastX = 400; float lastY = 300; // half of the width and height

bool firstMouse = true;

glm::vec3 light(10.0f, 20.0f, 10.0f);
glm::vec3 lightcol(1.0f, 1.0f, 0.803f);
glm::vec3 sunPos(light.x, light.y, light.z);

GLFWwindow* window;


Elements meshData(800, 600);

float * vertices = meshData.vertices;
unsigned* indices = meshData.indices;
float* normals = meshData.normals;
float * randoms = meshData.randoms;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw   += xoffset;
    pitch += yoffset;
    if(pitch > 89.0f)
      pitch =  89.0f;
    if(pitch < -89.0f)
      pitch = -89.0f;
    
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void init_texture(const char* filename, unsigned int &t, GLenum tex, GLenum format)
{
    int width, height, nrChannels;
    glGenTextures(1, &t);
    glActiveTexture(tex);
    glBindTexture(GL_TEXTURE_2D, t);
    // apply mag and min filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // clamp texture to border
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data)
    {
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
       glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
       std::cout << "Failed to load texture" << std::endl;
    }
    //free image from memory
    stbi_image_free(data);

    // must supply color for coordinates outside of border
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // white
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int run(int argc, char ** argv)
{
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            getchar();
            glfwTerminate();
            return -1;
        }
    
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    return 0;
}

void processInput(GLFWwindow *window)
{
    sunPos = glm::normalize(sunPos);
    const float cameraSpeed = 3.5f * deltaTime; // adjust accordingly
    glm::mat3 plane = glm::mat3(1.0f);
    plane[1][1] = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * glm::normalize(cameraFront * plane);
        sunPos += cameraSpeed * glm::normalize(cameraFront * plane);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * glm::normalize(cameraFront * plane);
        sunPos -= cameraSpeed * glm::normalize(cameraFront * plane);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        sunPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        sunPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

int main(int argc, char ** argv) {
    srand( (unsigned)time(NULL) );
    
    if (run(argc, argv) == -1) return -1;
    glEnable(GL_DEPTH_TEST);
    
    //  make new vertex attribute array to store VBO's
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); // generate 1 vertex array at VAO.
    glBindVertexArray(VAO); // bind it as current then generate the VBO
    
    // make new vertex buffer object
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind it as a GL_ARRAY_BUFFER to the VAO ( I think )
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices_size, vertices, GL_STATIC_DRAW); // it will be static data
    
    // bind to VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); // stride is 32 (sizeof(float) * 8) = 4 * 8 bytes
    
    
    GLuint VBOnormals;
    glGenBuffers(1, &VBOnormals);
    glBindBuffer(GL_ARRAY_BUFFER, VBOnormals);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices_size, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    
    GLuint VBOrandom;
    glGenBuffers(1, &VBOrandom);
    glBindBuffer(GL_ARRAY_BUFFER, VBOrandom);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices_size, randoms, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    
    /*
    GLuint VBOcolors;
    glGenBuffers(1, &VBOcolors);
    glBindBuffer(GL_ARRAY_BUFFER, VBOcolors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // bind to VAO
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void*) 12); // start after the pos data (12 bytes)
     
     
     */
    
    // make new element buffer object using the usual syntax
    GLuint EBO;
    glGenBuffers(1, &EBO);
    // bind as an EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * meshData.triangle_number, indices, GL_STATIC_DRAW); // same as for VBO
    
    
    /* tells openGL how to interpret the vertex buffer data we are sending it
        0 is the shader layout, 3 is data values (x,y,z), total size is 3 * sizeof(float) = 3 * 4 bytes
     plus 0 casted to void pointer which is the offset parameter. We are beginning from the start of the
     float array (tightly packed) so there is no offset */
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    // enable the vertexarray attribute at layout 0 which we just told openGL how to interpret
    glEnableVertexAttribArray(0); // enable positions
    glEnableVertexAttribArray(1); // enable normals
    glEnableVertexAttribArray(2); // enable random
    
    
     //enable wire mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // looks kinda good tbh
    
    // culls the back of the triangle/mesh halving gpu demand!
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CW); // GL_CCW for counter clock-wise
    
    
    // compile and link GLSL shader files
    Shader ourshader("shader.vs", "shader.fs");
    // set shader program active
    ourshader.use();
    
    // communicate with uniform variable in fragment shader
    // essentially tells the shader which texture comes first when assigning uniforms
// ourshader.setInt("texture1", 0); // I bound GL_TEXTURE0 to t1, thus it goes first and is bound to the uniform texture1
// ourshader.setInt("texture2", 1); // I bound GL_TEXTURE1 to t2, this it goes second and is bound to the uniform texture2

    
    glm::mat4 view;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // look at the right place at launch
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
    
    // render loop, closes with x click, 'q', or ESC;
    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
        // send uniforms to shaders
        unsigned int modelLoc = glGetUniformLocation(ourshader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(ourshader.ID, "view");
        unsigned int projectionLoc = glGetUniformLocation(ourshader.ID, "projection");
        unsigned int lightLoc = glGetUniformLocation(ourshader.ID, "light");
        unsigned int vLoc = glGetUniformLocation(ourshader.ID, "viewPos");
        unsigned int lightcolorLoc = glGetUniformLocation(ourshader.ID, "lightcolor");
        unsigned int sunLoc = glGetUniformLocation(ourshader.ID, "sunPos");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(lightLoc, light.x, light.y, light.z);
        glUniform3f(vLoc, cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(lightcolorLoc, lightcol.x, lightcol.y, lightcol.z);
        glUniform3f(sunLoc, sunPos.x, sunPos.y, sunPos.z);
        
        // bing correct VAO (we only have one)
        glBindVertexArray(VAO);
        // bind element array of indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 3 * meshData.triangle_number, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
