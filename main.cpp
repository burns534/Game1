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

static const float v[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f
};

static const unsigned int ind[] = {
    0, 1, 2,
    2, 0, 3
};

glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f,  1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -2.0f,  -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 direction;

float deltaTime = 0.0f;    // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float yaw = -90.0f; float pitch = 0.0f;

float lastX = 400; float lastY = 300; // half of the width and height

bool firstMouse = true;

glm::vec3 light(0.5f, 1.0f, 0.5f);
glm::vec3 lightcol(1.0f, 1.0f, 0.6f);
glm::vec3 sunPos(0.5f, 1.0f, 0.5f);

GLFWwindow* window;

// mesh vertices and indices
Elements meshData(800, 600);

float * vertices = meshData.vertices;
unsigned* indices = meshData.indices;
float* normals = meshData.normals;
float * randoms = meshData.randoms;

//static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };

int width, height; // because of retina display

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        pitch = - 180.0f * atan(2.0) / 3.14159265f;
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
    
    window = glfwCreateWindow(800, 600, "Game1", NULL, NULL);

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
    const float cameraSpeed = 0.9 * deltaTime; // adjust accordingly
    glm::mat3 plane = glm::mat3(1.0f);
    plane[1][1] = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * glm::normalize(cameraFront * plane);
        //sunPos += factor * cameraSpeed * glm::normalize(cameraFront * plane);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * glm::normalize(cameraFront * plane);
       // sunPos -= factor * cameraSpeed * glm::normalize(cameraFront * plane);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      //  sunPos -= factor * glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      //  sunPos += factor * glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}

int main(int argc, char ** argv) {
    srand( (unsigned)time(NULL) );
    std::ofstream outfile;
    outfile.open("mainlog.txt");
    
    if (run(argc, argv) == -1) return -1;
    
    //  make new vertex attribute array to store VBO's
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); // generate 1 vertex array at VAO.
    glBindVertexArray(VAO); // bind it as current then generate the VBO

    // make new vertex buffer object to store vertices for meshdata
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind it as a GL_ARRAY_BUFFER to the VAO ( I think )
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices_size, vertices, GL_STATIC_DRAW); // it will be static data
    
    // bind to VAO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); // stride is 32 (sizeof(float) * 8) = 4 * 8 bytes
    
    // stores normals for light calculations
    GLuint VBOnormals;
    glGenBuffers(1, &VBOnormals);
    glBindBuffer(GL_ARRAY_BUFFER, VBOnormals);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices_size, normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    // stores -- noise for water
    GLuint VBOrandom;
    glGenBuffers(1, &VBOrandom);
    glBindBuffer(GL_ARRAY_BUFFER, VBOrandom);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices_size, randoms, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    
    // make new element buffer object using the usual syntax- stores indices
    GLuint EBO;
    glGenBuffers(1, &EBO);
    // bind as an EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * meshData.triangle_number, indices, GL_STATIC_DRAW); // same as for VBO
    
    // all of this is bound to the first VAO
    
    // framebuffers are not bound to VAOs
    // this is the off-screen framebuffer copied and pasted from the website.
//    unsigned int framebuffer;
//    glGenFramebuffers(1, &framebuffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//    // create a color attachment texture
//    unsigned int textureColorbuffer;
//    glGenTextures(1, &textureColorbuffer);
//    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // can be float also
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
//
//    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
//    unsigned int rbo;
//    glGenRenderbuffers(1, &rbo);
//    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); // use a single renderbuffer object for both a depth AND stencil buffer.
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
//    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    
    // bind normal buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    GLuint depthMapFBO = 42;

    glGenFramebuffers(1, &depthMapFBO); // create framebuffer object
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    
    GLuint depthMap = 43;
    glGenTextures(1, &depthMap); // create depthmap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);  // bind depthMap as 2D texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); // set up storage for texture
    // minimizing and magnification filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // no wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    // bind texture as storage for framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0); // 0 mipmap level
    // there is no color attachment for this framebuffer object
    glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
    // bind default framebuffer
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "offscreen framebuffer failure\n";
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // create shadow shader
    Shader shadowShader("shader.shadowvs", "shader.shadowfs");
    
    // compile and link GLSL shader files
    Shader ourshader("shader.vs", "shader.fs");
     
    //shadowShader.setInt("screenTexture", 0);
    
    
    
//    GLuint color_texture;
//    glGenTextures(1, &color_texture);
//    glBindTexture(GL_TEXTURE_2D, color_texture);
//    glTexStorage2D(color_texture, 0, GL_RGB32F, 800, 600); // not sure about the unsigned byte thing
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    // bind to framebuffer object
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
//    glBindTexture(GL_TEXTURE_2D, 0); // bind non texture
    //glDrawBuffer(GL_COLOR_ATTACHMENT0);
    // create texture for depth buffer
//    GLuint depth_texture;
//    glGenTextures(1, &depth_texture);
//    glBindTexture(GL_TEXTURE_2D, depth_texture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//
//    // set up texture
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
//    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//
//
//    // attach depth texture to the FBO
//    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LESS);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
//
//    glDrawBuffer(GL_NONE); // no color buffer
//    glReadBuffer(GL_NONE);
//    // check status of framebuffer
//    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
//    {
//        std::cout << "framebuffer complete!\n";
//    }
    // create texture for colorbuffer
//    GLuint color_texture;
//    glGenTextures(1, &color_texture);
//    glBindTexture(GL_TEXTURE_2D, color_texture);
//    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, 800, 600);
//
//    // set up texture
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    // attach color texture to FBO
//    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
    
 
    
    unsigned int quadVAO, quadVBO, quadVerts;
    // generate vertex array object and vertex buffer object
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVerts);
    glBindVertexArray(quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, quadVerts);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v) , v, GL_STATIC_DRAW); // I had this wrong before
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW); // Should also be 12 bytes per indexing set ( triangle)
    
     //enable wire mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // looks kinda good tbh
    
    // culls the back of the triangle/mesh halving gpu demand!
//    glEnable(GL_CULL_FACE); // cull face
//    glCullFace(GL_BACK); // cull back face
//    glFrontFace(GL_CW); // GL_CCW for counter clock-wise
    
    
    // communicate with uniform variable in fragment shader
    // essentially tells the shader which texture comes first when assigning uniforms
// ourshader.setInt("texture1", 0); // I bound GL_TEXTURE0 to t1, thus it goes first and is bound to the uniform texture1
// ourshader.setInt("texture2", 1); // I bound GL_TEXTURE1 to t2, this it goes second and is bound to the uniform texture2

    
    glm::mat4 view;
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.01f, 100.0f);
    //projection = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
    
    glm::mat4 shadowProjection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
    //glm::mat4 shadowProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f); // light rays should be parallel
    //glm::mat4 shadowProjection = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1000.0f);
    glm::mat4 shadowView = glm::lookAt(glm::vec3(2.0f, 1.0f, 2.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
    
    glm::mat4 lightSpace = shadowProjection * shadowView; // create light transform matrix to pass to shader
    
    // standard blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // look at the right place at launch
//    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    direction.y = sin(glm::radians(pitch));
//    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    cameraFront = glm::normalize(direction);

    // bind map vao
    glBindVertexArray(VAO);
    // enable depth-testing
    
    //glDepthFunc(GL_ALWAYS);
    // render loop, closes with x click, 'q', or ESC;
    float * here = new float[800*600];
    float * reset = here;
    bool first = true;
    
    
    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        
        // mouse camera movement
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // view matrix based on camera movement
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // input
        processInput(window);
        
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
//  1.  Render of off-screen z-buffer object depthMapFBO
        
        // use simple shader
        shadowShader.use();
        // send lightSpace and model matrices as uniforms to shadowShader
        shadowShader.setMat4("lightSpace", lightSpace);
        shadowShader.setMat4("model", model);
        
        // set viewport matrix
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        //bind off-screen depth framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // render scene
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, 3 * meshData.triangle_number, GL_UNSIGNED_INT, 0);
        
        // bind on-screen framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        
//  2.
        
        // clear buffer depth and color values
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate a program that will read from the texture
        ourshader.use();
  
        // set approporiate viewport matrix
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        // send uniforms to main shader
        ourshader.setMat4("model", model);
        ourshader.setMat4("view", view);
        ourshader.setMat4("projection", projection);
        ourshader.setMat4("lightSpace", lightSpace);
        
        ourshader.setVec3("light", light);
        ourshader.setVec3("viewPos", cameraPos);
        ourshader.setVec3("lightcolor", lightcol);
        ourshader.setVec3("sunPos", sunPos);
        
        // render actual scene
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        ourshader.setInt("depthMap", 0); // set uniform "depthMap" as 0;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 3 * meshData.triangle_number, GL_UNSIGNED_INT, 0); // render scene to depth framebuffer from light's perspective
        if (first)
        {
            glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, here);
            here = reset;
            for (;*here; here++) outfile << *here << " ";
            here = reset;
            delete[] here;
            first = false;
        }
        glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
        
        /*
         
        glBindVertexArray(quadVAO);
        // bind element array of indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadVBO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        // bind depth_texture
        // draw elements
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_TRIANGLES, 3 * meshData.triangle_number, GL_UNSIGNED_INT, 0);
        // refresh screen
        // unbind the texture and we're done
        //glBindTexture(GL_TEXTURE_2D, 0);
        //glBindVertexArray(0);
         
         */
        
//     2.
        
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glDisable(GL_DEPTH_TEST);
//        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        shadowShader.use();
//        shadowShader.setMat4("proj", glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
//        glBindVertexArray(quadVAO);
//        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadVBO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
         //should be done
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    outfile.close();
    return 0;
}


