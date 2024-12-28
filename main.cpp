#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "header/cube.h"
#include "header/object.h"
#include "header/shader.h"
#include "header/stb_image.h"

using namespace std;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(std::vector<string> &mFileName);

/*void setcloud()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 載入雲霧貼圖
    GLuint textureID;
    glGenTextures(1, &textureID);
    // 綁定貼圖及配置
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}*/

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life; // 剩餘生命時間
    float size; // 粒子大小
};

std::vector<Particle> particles;
int maxParticles = 100; // 最大粒子數量


struct material_t{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float gloss;
};

struct light_t{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct model_t{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    Object* object;
};

struct camera_t{
    glm::vec3 position;
    glm::vec3 up;
    float rotationY;
};

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
float shake_rotate = 0;
// cube map 
unsigned int cubemapTexture;
unsigned int cubemapVAO, cubemapVBO;

// shader programs 
int shaderProgramIndex = 0;
bool smoke = 0,shake = 0;
float Time = 0;
float scale = 1.0;
std::vector<shader_program_t*> shaderPrograms;
shader_program_t* cubemapShader;
shader_program_t *SmokeShader;

// additional dependencies
light_t light;
material_t material;
camera_t camera;
model_t helicopter;
model_t helicopterBlade;
model_t Smoke;

// model matrix
int moveDir = -1;
glm::mat4 helicopterModel;
glm::mat4 smokemodel;
glm::mat4 helicopterBladeModel;
glm::mat4 cameraModel;

//////////////////////////////////////////////////////////////////////////
// Parameter setup, 
// You can change any of the settings if you want

void camera_setup(){
    camera.position = glm::vec3(0.0, 20.0, 100.0);
    camera.up = glm::vec3(0.0, 1.0, 0.0);
    camera.rotationY = 0;
}

void light_setup(){
    light.position = glm::vec3(0.0, 1000.0, 0.0);
    light.ambient = glm::vec3(1.0);
    light.diffuse = glm::vec3(1.0);
    light.specular = glm::vec3(1.0);
}

void material_setup(){
    material.ambient = glm::vec3(1.0);
    material.diffuse = glm::vec3(1.0);
    material.specular = glm::vec3(0.7);
    material.gloss = 10.5;
}
//////////////////////////////////////////////////////////////////////////

void model_setup(){

// Load the object and texture for each model here 

#if defined(__linux__) || defined(__APPLE__)
    std::string objDir = "../../src/asset/obj/";
    std::string textureDir = "../../src/asset/texture/";
#else
    std::string objDir = "..\\..\\src\\asset\\obj\\";
    std::string textureDir = "..\\..\\src\\asset\\texture\\";
#endif
    helicopterModel = glm::mat4(1.0f);

    helicopter.position = glm::vec3(0.0f, -50.0f, 0.0f);
    helicopter.scale = glm::vec3(0.1f, 0.1f, 0.1f);
    helicopter.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    helicopter.object = new Object(objDir + "helicopter_body.obj");
    helicopter.object->load_to_buffer();
    helicopter.object->load_texture(textureDir + "helicopter_red.jpg");

    helicopterBlade.position = helicopter.position;
    helicopterBlade.scale = helicopter.scale;
    helicopterBlade.rotation = helicopter.rotation;
    helicopterBlade.object = new Object(objDir + "helicopter_blade.obj");
    helicopterBlade.object->load_to_buffer();
    helicopterBlade.object->load_texture(textureDir + "helicopter_red.jpg");

    smokemodel = glm::mat4(1.0f);
    Smoke.position = glm::vec3(0.0f, 0.0f, 0.0f);
    Smoke.scale = glm::vec3(0.1f, 0.1f, 0.1f);
    Smoke.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Smoke.object = new Object(objDir + "cloud.obj");
    Smoke.object->load_to_buffer();
    Smoke.object->load_texture(textureDir + "cloud.png");
}


void shader_setup(){

// Setup the shader program for each shading method

#if defined(__linux__) || defined(__APPLE__)
    std::string shaderDir = "../../src/shaders/";
#else
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> shadingMethod = {"default"};

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    }

    SmokeShader = new shader_program_t();
    SmokeShader->create();
    std::string v = shaderDir + "smoke.vert";
    std::string f = shaderDir + "smoke.frag";
    SmokeShader->add_shader(v, GL_VERTEX_SHADER);
    SmokeShader->add_shader(f, GL_FRAGMENT_SHADER);
    SmokeShader->link_shader();
}

void cubemap_setup(){

// Setup all the necessary things for cubemap rendering
// Including: cubemap texture, shader program, VAO, VBO

#if defined(__linux__) || defined(__APPLE__)
    std::string cubemapDir = "../../src/asset/texture/skybox/";
    std::string shaderDir = "../../src/shaders/";
#else
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    // setup texture for cubemap
    std::vector<std::string> faces
    {
        cubemapDir + "right.jpg",
        cubemapDir + "left.jpg",
        cubemapDir + "top.jpg",
        cubemapDir + "bottom.jpg",
        cubemapDir + "front.jpg",
        cubemapDir + "back.jpg"
    };
    cubemapTexture = loadCubemap(faces);   

    // setup shader for cubemap
    std::string vpath = shaderDir + "cubemap.vert";
    std::string fpath = shaderDir + "cubemap.frag";
    
    cubemapShader = new shader_program_t();
    cubemapShader->create();
    cubemapShader->add_shader(vpath, GL_VERTEX_SHADER);
    cubemapShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    cubemapShader->link_shader();

    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
}

unsigned int SmokeTexture;


void setup(){
    // Initialize shader model camera light material
    light_setup();
    model_setup();
    shader_setup();
    camera_setup();
    cubemap_setup();
    material_setup();

    // Enable depth test, face culling ...
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Debug: enable for debugging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](  GLenum source, GLenum type, GLuint id, GLenum severity, 
                                 GLsizei length, const GLchar* message, const void* userParam) {

     std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
               << "type = " << type 
               << ", severity = " << severity 
               << ", message = " << message << std::endl;
     }, nullptr);
}

void update(){
// Update the heicopter position, camera position, rotation, etc.

    helicopter.position.y += moveDir;
    if(helicopter.position.y > 20.0 || helicopter.position.y < -100.0){
        moveDir = -moveDir;
    }

    helicopterBlade.rotation.y += 10;
    if(helicopterBlade.rotation.y > 360.0){
        helicopterBlade.rotation.y = 0.0;
    }

    helicopterModel = glm::mat4(1.0f);
    helicopterModel = glm::scale(helicopterModel, helicopter.scale);
    helicopterModel = glm::translate(helicopterModel, helicopter.position);

    helicopterBladeModel = glm::rotate(helicopterModel, glm::radians(helicopterBlade.rotation.y), glm::vec3(0.0, 1.0, 0.0));

    camera.rotationY = (camera.rotationY > 360.0) ? 0.0 : camera.rotationY;
    cameraModel = glm::mat4(1.0f);
    cameraModel = glm::rotate(cameraModel, glm::radians(camera.rotationY), camera.up);
    cameraModel = glm::translate(cameraModel, camera.position);

    shake_rotate += 10.0;
    if(shake_rotate > 360.0){
        shake_rotate -= -360.0;
    }
    
    float deltaTime = 0.016f;

    for (auto& p : particles) {
        if (p.life > 0.0f) {
            p.position += p.velocity * deltaTime; // 更新位置
            p.velocity.y -= 9.8f * deltaTime;     // 模擬重力
            p.life -= deltaTime;                 // 減少生命時間
        }
    }

}

void render(){
    std::cout << "render\n";
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate view, projection matrix
    glm::mat4 view = glm::lookAt(glm::vec3(cameraModel[3]), glm::vec3(0.0), camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    // Set matrix for view, projection, model transformation
    float blurStrength = 0.03;
    
    shaderPrograms[0]->use();
    shaderPrograms[0]->set_uniform_value("model", helicopterModel);
    shaderPrograms[0]->set_uniform_value("view", view);
    shaderPrograms[0]->set_uniform_value("projection", projection);
    shaderPrograms[0]->set_uniform_value("u_time", Time);
    shaderPrograms[0]->set_uniform_value("blurStrength", blurStrength);
    shaderPrograms[0]->set_uniform_value("shake", shake);
    // TODO 1
    // Set uniform value for each shader program
    


    glm::vec3 camera_pos = glm::vec3(cameraModel[3]);

    shaderPrograms[0]->set_uniform_value("camera_pos", camera_pos);


    helicopter.object->render();
    shaderPrograms[0]->set_uniform_value("model", helicopterBladeModel);
    shaderPrograms[0]->set_uniform_value("u_time", Time);
    shaderPrograms[0]->set_uniform_value("blurStrength", blurStrength);
    shaderPrograms[0]->set_uniform_value("shake", shake);

    helicopterBlade.object->render();
    shaderPrograms[0]->set_uniform_value("model", helicopterBladeModel);
    shaderPrograms[0]->set_uniform_value("u_time", Time);
    shaderPrograms[0]->set_uniform_value("blurStrength", blurStrength);
    shaderPrograms[0]->set_uniform_value("shake", shake);


    shaderPrograms[0]->release();


    // TODO 4-2 
    // Rendering cubemap environment
    // Hint:
    // 1. All the needed things are already set up in cubemap_setup() function.
    // 2. You can use the vertices in cubemapVertices provided in the header/cube.h
    // 3. You need to set the view, projection matrix.
    // 4. Use the cubemapShader to render the cubemap 
    //    (refer to the above code to get an idea of how to use the shader program)
    cubemapShader->use();

    cubemapShader->set_uniform_value("u_time", Time);
    cubemapShader->set_uniform_value("blurStrength", blurStrength);
    cubemapShader->set_uniform_value("shake", shake);
    cubemapShader->set_uniform_value("view", view);
    cubemapShader->set_uniform_value("projection", projection);
    cubemapShader->set_uniform_value("cube", 1);
    
    glBindVertexArray(cubemapVAO);
    glDrawArrays(GL_TRIANGLES,0,36);
    glBindVertexArray(0);

    cubemapShader->release();
    
    if(smoke)
    {

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        SmokeShader->use();
        SmokeShader->set_uniform_value("view", view);
        SmokeShader->set_uniform_value("projection", projection);
        shaderPrograms[0]->set_uniform_value("u_time", Time);
        shaderPrograms[0]->set_uniform_value("blurStrength", blurStrength);
        shaderPrograms[0]->set_uniform_value("shake", shake);
        for (const auto& p : particles) {
            if (p.life > 0.0f) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), p.position);
                smokemodel = glm::scale(model, glm::vec3(p.size)); // 調整大小
                SmokeShader->set_uniform_value("model", smokemodel);
                SmokeShader->set_uniform_value("alpha", p.life); // 用生命時間作為透明度

                Smoke.object->render(); // 假設 Smoke 是雲霧模型
            }

        }

        SmokeShader->release();
        glDisable(GL_BLEND);
    }
}


int main() {
    
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW4-110704071", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Setup texture, model, shader ...e.t.c
    setup();
    
    // Render loop, main logic can be found in update, render function
    while (!glfwWindowShouldClose(window)) {
        Time = glfwGetTime();
        update(); 
        render(); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Add key callback
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE.
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // shader program selection
    if (key == GLFW_KEY_0 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 0;
    if (key == GLFW_KEY_1 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shake = (shake + 1) % 2;
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        {
            smoke = (smoke + 1) % 2;
            if(smoke)
            {
                particles.clear();
                for (int i = 0; i < maxParticles; ++i) {
                    Particle p;
                    p.position = Smoke.position;
                    p.velocity = glm::vec3(
                        (float(rand()) / RAND_MAX - 0.5f) * 10.0f, // 隨機 X
                        (float(rand()) / RAND_MAX) * 10.0f,         // 隨機 Y
                        (float(rand()) / RAND_MAX - 0.5f) * 10.0f  // 隨機 Z
                    );
                    p.life = float(rand()) / RAND_MAX * 2.0f; // 1~2秒壽命
                    p.size = (i / 10) * 0.05 + 0.5f + float(rand()) / RAND_MAX; // 隨機大小
                    particles.push_back(p);
                }
            }
        }

    // camera movement
    float cameraSpeed = 0.5f;
    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.position.z -= 10.0;
    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.position.z += 10.0;
    if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.rotationY -= 10.0;
    if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.rotationY += 10.0;
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// Loading cubemap texture
unsigned int loadCubemap(vector<std::string>& faces){

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}  
