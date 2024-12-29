#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "header/cube.h"
#include "header/cubemap.h"
#include "header/object.h"
#include "header/shader.h"
#include "header/stb_image.h"
#include "header/expansion.h"
#include "header/particle.h"

#if defined(__linux__) || defined(__APPLE__)
    std::string objDir = "../../src/asset/obj/";
    std::string textureDir = "../../src/asset/texture/";
    std::string shaderDir = "../../src/shaders/";
#else
    std::string objDir = "..\\..\\src\\asset\\obj\\";
    std::string textureDir = "..\\..\\src\\asset\\texture\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

using namespace std;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(std::vector<string> &mFileName);
void load_image(std::string texturePath);

struct smokeParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life; // 剩餘生命時間
    float size; // 粒子大小
};

std::vector<smokeParticle> smokeparticles;
int maxsmokeParticles = 1; // 最大粒子數量

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

struct ground_t{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

struct camera_t{
    glm::vec3 position;
    glm::vec3 up;
    float rotationY;
};
struct particleSystem_t {
    glm::vec3 position ;
    glm::vec3 incident ;
    glm::vec3 normal ;
    glm::vec3 acceleration ;
    float baseSize ;
    float baseLifetime ;
    float randomFactor ;
    unsigned int generateParticleNumeber ;
};

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
float shake_rotate = 0;
// cube map 
unsigned int cubemapIndex = 0;
std::vector<shader_program_t*> cubemapShaders;
std::vector<unsigned int> cubemapTextures ;
std::vector<unsigned int> cubemapVBOs = {0, 0, 0};
std::vector<unsigned int> cubemapVAOs = {0, 0, 0};
std::vector<std::string> cubeNames = {"skybox", "grass", "mars"};
//// grass source : https://www.humus.name/index.php?page=Cubemap&item=NiagaraFalls3
//// mars source : http://www.paulbourke.net/miscellaneous/mars/

// ground 
unsigned int groundVAO = 0;
unsigned int groundVBO = 0;
GLuint groundTextureIndex = 0;
std::vector<GLuint> groundTextures;
shader_program_t* groundShader;
glm::mat4 groundModel;
ground_t ground;
std::vector<string> groundNames = {"Grass", "Sand", "Stones"};
//// Grass source : https://www.humus.name/index.php?page=Textures&ID=24
//// Sand source : https://www.humus.name/index.php?page=Textures&ID=28
//// Stones source : https://www.humus.name/index.php?page=Textures&ID=29

// particle
std::vector<Particle*> particles;
shader_program_t* particleShader;
particleSystem_t particleSystem;
float rate = 1.0; // rate of particle generation per frame
GLuint ParticleVBO, ParticleVAO;

// shader programs 
int shaderProgramIndex = 6;
bool smoke = 0,shake = 0;
float Time = 0;
float scale = 1.0;
std::vector<shader_program_t*> shaderPrograms;
shader_program_t* cubemapShader;
shader_program_t *SmokeShader;
shader_program_t* fadingShader;

// additional dependencies
light_t light;
material_t material;
camera_t camera;
model_t helicopter;
model_t Smoke;

// model matrix
int moveDir = -1;
glm::mat4 helicopterModel;
glm::mat4 smokemodel;
glm::mat4 cameraModel;

// bomb model & time control
model_t bomb;
glm::mat4 bombModel;
unsigned int bombTexture;
auto startTime = glfwGetTime();
auto currentTime = startTime;
float sparkStartTime = 10.0;
float sparkDuration = 10.0;
float expansionStartTime = 8.0;
float expansionDuration = 11.0;
float expansionScale = 1.0;
float expandSpeed = 10.0;
Expansion bombExpansion(expansionStartTime, expansionDuration, expandSpeed);
float crackStartTime = 4.85;
float crackDuration = 5.15;
float detachStartTime = 10.0;
float detachDuration = 10.0;
float gravity = 1e-4;
float bounceFactor = 0.3f;
glm::vec3 velocity(0.0f, 0.0f, 0.0f);
//explosion
float explosionTime = 20.0;
float explosionEndTime = 25.0;
float particleStartTime = 5.0;
// fading
unsigned int quadVAO, quadVBO;
float quadVertices[] = {
    // positions     // texture coordinates
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f
};
float fadeDuration = explosionEndTime - explosionTime; // Duration of fade in seconds
float overlayAlpha;
// blur 
float blurStrength = 0.03;
// smoke
float smokeStartTime = 20.0;

//////////////////////////////////////////////////////////////////////////
// Parameter setup, 
void camera_setup(){
    camera.position = glm::vec3(0.0, 30.0, 100.0);
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

void update_particle_positionY() {
    float amplitude = 10.0f;
    float offset = 40.0f;
    float frequency = 1.0f;

    particleSystem.position.y = offset + amplitude + amplitude * sin(currentTime * frequency);
}

void particle_model_setup() {
    /* ======================== particle system ========================*/
    particleSystem.position = glm::vec3(0, 0, 0);
    particleSystem.incident = glm::vec3(0, 0.5, 0);                 // direction for emit direction and velocity
    particleSystem.normal = glm::normalize(glm::vec3(0, 1, 0));     // spread direction
    particleSystem.acceleration = glm::vec3(0, -0.1, 0);
    particleSystem.baseSize = 0.15f;                                 // size of particle, will be randomized later
    particleSystem.baseLifetime = 100.0f;                           // lifetime of particle, will be randomized later
    particleSystem.generateParticleNumeber = 10;
    particleSystem.randomFactor = 0.05f;
    update_particle_positionY();

    /* ======================== VAO, VBO =======================*/
    float vertices[] = { 0, 0, 0 }; // real position of emitter

    glGenVertexArrays(1, &ParticleVAO);
    glBindVertexArray(ParticleVAO);

    glGenBuffers(1, &ParticleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ParticleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void particle_shader_setup(){
    std::string vpath = shaderDir + "particle.vert";
    std::string gpath = shaderDir + "particle.geom";
    std::string fpath = shaderDir + "particle.frag";
    particleShader = new shader_program_t();
    particleShader->create();
    particleShader->add_shader(vpath, GL_VERTEX_SHADER);
    particleShader->add_shader(gpath, GL_GEOMETRY_SHADER);
    particleShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    particleShader->link_shader();
}

void fading_shader_setup(){
    std::string vpath = shaderDir + "fading.vert";
    std::string fpath = shaderDir + "fading.frag";
    fadingShader = new shader_program_t();
    fadingShader->create();
    fadingShader->add_shader(vpath, GL_VERTEX_SHADER);
    fadingShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    fadingShader->link_shader();
}

void smoke_model_setup(){
    smokemodel = glm::mat4(1.0f);
    Smoke.position = glm::vec3(0.0f, 0.0f, 0.0f);
    Smoke.scale = glm::vec3(0.1f, 0.1f, 0.1f);
    Smoke.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Smoke.object = new Object(objDir + "explosion.obj");
    Smoke.object->load_to_buffer();
    Smoke.object->load_texture(textureDir + "explosion-diffuse.jpg");
}

void smoke_shader_setup(){
    SmokeShader = new shader_program_t();
    SmokeShader->create();
    std::string v = shaderDir + "smoke.vert";
    std::string f = shaderDir + "smoke.frag";
    SmokeShader->add_shader(v, GL_VERTEX_SHADER);
    SmokeShader->add_shader(f, GL_FRAGMENT_SHADER);
    SmokeShader->link_shader();
}

void bomb_model_setup(){
    #if defined(__linux__) || defined(__APPLE__)
        std::string objDir = "../../src/asset/obj/";
        std::string textureDir = "../../src/asset/texture/";
    #else
        std::string objDir = "..\\..\\src\\asset\\obj\\";
        std::string textureDir = "..\\..\\src\\asset\\texture\\";
    #endif

    bombModel = glm::mat4(1.0f);
    bomb.position = glm::vec3(0.0f, 7.0f, 0.0f);
    bomb.scale = glm::vec3(10.0f, 10.0f, 10.0f);
    bomb.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
    bomb.object = new Object(objDir + "missile.obj");
    bomb.object->load_to_buffer();
    bombTexture = bomb.object->load_texture(textureDir + "missile_baseColor.png");
}

void bomb_shader_setup(){
    // Setup the shader program for each shading method
    std::vector<std::string> shadingMethod = {
        "default",                              // default shading
        "bling-phong", "gouraud", "metallic",   // addional shading effects (basic)
        "glass_schlick", "glass_empricial",     // addional shading effects (advanced)
        "bomb"                                 // final project
    };

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        // add geometry shader for pre-explosion shading
        if (shadingMethod[i] == "bomb") {
            std::string gpath = shaderDir + shadingMethod[i] + ".geom";
            shaderProgram->add_shader(gpath, GL_GEOMETRY_SHADER);
        }
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);

        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    }
}

void ground_model_setup() {
    groundModel = glm::mat4(1.0f);
    ground.position = glm::vec3(0.0f, -0.5f, 0.0f);         
    ground.scale = glm::vec3(1000.0f, 1000.0f, 1000.0f); // for video show
    //ground.scale = glm::vec3(10.0f, 10.0f, 10.0f);          // for test
    ground.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < groundNames.size(); i++) {
        std::string texturePath = textureDir + groundNames[i] + ".jpg";
        GLuint groundTexture;
        glGenTextures(1, &groundTexture);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        load_image(texturePath);
        glBindTexture(GL_TEXTURE_2D, 0);
        groundTextures.push_back(groundTexture);
    }
}

void load_image(std::string texturePath){
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ground_shader_setup(){
    std::string vpath = shaderDir + "ground.vert";
    std::string fpath = shaderDir + "ground.frag";
    groundShader = new shader_program_t();
    groundShader->create();
    groundShader->add_shader(vpath, GL_VERTEX_SHADER);
    groundShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    groundShader->link_shader();

    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void cubemap_setup(){
    for (int i = 0 ; i < cubeNames.size() ; i ++) {
        std::string cubeName = cubeNames[i] ;
        // Setup all the necessary things for cubemap rendering
        // Including: cubemap texture, shader program, VAO, VBO
#if defined(__linux__) || defined(__APPLE__)
        std::string cubemapDir = "../../src/asset/texture/" + cubeName + "/";
#else
        std::string cubemapDir = "..\\..\\src\\asset\\texture\\" + cubeName + "\\";
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
        unsigned int cubemapTexture = loadCubemap(faces);   

        // setup shader for cubemap
        std::string vpath = shaderDir + "cubemap.vert";
        std::string fpath = shaderDir + "cubemap.frag";
        
        shader_program_t* cubemapShader = new shader_program_t();
        cubemapShader->create();
        cubemapShader->add_shader(vpath, GL_VERTEX_SHADER);
        cubemapShader->add_shader(fpath, GL_FRAGMENT_SHADER);
        cubemapShader->link_shader();

        glGenVertexArrays(1, &cubemapVAOs[i]);
        glGenBuffers(1, &cubemapVBOs[i]);
        glBindVertexArray(cubemapVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, cubemapVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        cubemapTextures.push_back(cubemapTexture);
        cubemapShaders.push_back(cubemapShader);
    }
}

void genQuad(){
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void setup(){
    // Initialize shader model camera light material
    light_setup();
    bomb_model_setup();
    bomb_shader_setup();
    ground_model_setup();
    ground_shader_setup();
    camera_setup();
    cubemap_setup();
    material_setup();
    particle_model_setup();
    particle_shader_setup();
    fading_shader_setup();
    genQuad();
    smoke_model_setup();
    smoke_shader_setup();

    // Enable depth test, face culling ...
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Debug: enable for debugging
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback([](  GLenum source, GLenum type, GLuint id, GLenum severity, 
    //                              GLsizei length, const GLchar* message, const void* userParam) {

    //  std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
    //            << "type = " << type 
    //            << ", severity = " << severity 
    //            << ", message = " << message << std::endl;
    //  }, nullptr);
}

void makeParticles(particleSystem_t* system) {
	unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	ParticleCreateInfo createInfo;
	createInfo.acceleration = system->acceleration;
	createInfo.color = glm::vec3(1.0f, 0.0f, 0.0f); // base is red 
	createInfo.position = system->position;

	for (int i = 0; i < system->generateParticleNumeber; ++i) {
		float x = float(generator() % 100) / 50.f - 1.0f;
		float y = float(generator() % 100) / 50.f - 1.0f;
		float z = float(generator() % 100) / 50.f - 1.0f;

		glm::vec3 randomization = glm::vec3(x, y, z);
		glm::vec3 randomizedNormal = glm::normalize(system->randomFactor * randomization + system->normal);

		x = float(generator() % 100) / 10.0f;
		glm::vec3 outgoing = x * glm::reflect(system->incident, randomizedNormal);

		createInfo.velocity = outgoing;
        createInfo.color += glm::vec3(0.0f, 0.1f, 0.0f) * (float(generator() % 100) / 100.0f); // add tobe yellow

        x = system->baseSize + (float(generator() % 100) - 50.0f) / 50.0f * system->baseSize;
        createInfo.size = x ;

		x = system->baseLifetime + (float(generator() % 100) - 50.0f) / 50.0f * system->baseLifetime;
		createInfo.lifetime = x;

		particles.push_back(new Particle(&createInfo));
	}
}

// generate smoke model and texture
void generateSmoke(){
    smoke = 1;
    shake = 1;
    smokeparticles.clear();
    for (int i = 0; i < maxsmokeParticles; ++i) {
        smokeParticle p;
        p.position = Smoke.position;
        p.velocity = glm::vec3(
            (float(rand()) / RAND_MAX - 0.5f) * 10.0f, // 隨機 X
            (float(rand()) / RAND_MAX) * 10.0f,         // 隨機 Y
            (float(rand()) / RAND_MAX - 0.5f) * 10.0f  // 隨機 Z
        );
        p.life = 2.0 - (i + 1) * 0.01 * 1.5; // 1~2秒壽命
        p.size = (i + 1) * 0.005 + 0.15f; // 隨機大小
        std::cout << p.life << " " << p.size << '\n';
        smokeparticles.push_back(p);
    }
}

void update(){
    // Update model matrix
    bombModel = glm::mat4(1.0f);
    bombModel = glm::scale(bombModel, bomb.scale * expansionScale);
    bombModel = glm::translate(bombModel, bomb.position);
    bombModel = glm::rotate(bombModel, glm::radians(bomb.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    bombModel = glm::rotate(bombModel, glm::radians(bomb.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    // Update camera model matrix
    camera.rotationY = (camera.rotationY > 360.0) ? 0.0 : camera.rotationY;
    cameraModel = glm::mat4(1.0f);
    cameraModel = glm::rotate(cameraModel, glm::radians(camera.rotationY), camera.up);
    cameraModel = glm::translate(cameraModel, camera.position);
    // Update ground model matrix
    groundModel = glm::mat4(1.0f);
    groundModel = glm::scale(groundModel, ground.scale);
    groundModel = glm::translate(groundModel, ground.position);
    // Update particle system
    makeParticles(&particleSystem);
	for (int i = 0; i < particles.size(); ++i) {
		Particle* particle = particles[i];
		particle->update(rate);
		if (particle->t >= particle->lifetime) {
			delete particle;
			particles.erase(particles.begin() + i--);
		}
	}
    // Update bomb position
    currentTime = glfwGetTime();
    float time = currentTime - startTime ;
    velocity.y -= gravity * time;
    if (bomb.position.y <= 0.5f) {
        bomb.position.y = 0.5f;
        velocity.y = 0.0f;
    }
    bomb.position += velocity * time;
    // Update camera position
    float crack_deltaTime = time - crackStartTime;
    if (time >= crackStartTime && time < detachStartTime) {
        camera.position.z -= 0.05f * crack_deltaTime;
    } else if (time >= detachStartTime + 0.1 && time < explosionTime) {
        camera.position.z += 0.025f * crack_deltaTime;
    }
    // shake effect
    shake_rotate += 10.0;
    if(shake_rotate > 360.0){
        shake_rotate -= -360.0;
    }    
    // smoke effect
    float smoke_deltaTime = 0.02f;
    for (int i = 0;i < smokeparticles.size();i++) {
        if (smokeparticles[i].life > 0.0f) {
            smokeparticles[i].position += smokeparticles[i].velocity * smoke_deltaTime; // 更新位置
            smokeparticles[i].velocity.y -= 9.8f * smoke_deltaTime;     // 模擬重力
            smokeparticles[i].life -= smoke_deltaTime;
            smokeparticles[i].size += 0.005;                 // 減少生命時間
        }
    }
    if (smokeStartTime <= time && smokeStartTime + 0.1 >= time) {
        generateSmoke();
    }
}

void render(){
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // time calculation
    float time = currentTime - startTime;

    // Calculate view, projection matrix
    glm::mat4 view = glm::lookAt(glm::vec3(cameraModel[3]), glm::vec3(0.0, 20.0, 0.0), camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    if (time < explosionTime){
        // Set matrix for view, projection, model transformation
        shaderPrograms[shaderProgramIndex]->use();
        //shaderPrograms[shaderProgramIndex]->set_uniform_value("model", helicopterModel);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("model", bombModel);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("view", view);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("projection", projection);
        
        // Set uniform value for each shader program
        light.specular = glm::vec3(1.0);
        material.gloss = 10.5;
        // camera uniform value
        shaderPrograms[shaderProgramIndex]->set_uniform_value("camera_position", cameraModel[3]) ;
        // light uniform value
        glm::vec3 lightPositionInCameraSpace = glm::vec3(cameraModel * glm::vec4(light.position, 1.0f));
        shaderPrograms[shaderProgramIndex]->set_uniform_value("light_ambient", light.ambient) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("light_diffuse", light.diffuse) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("light_position", lightPositionInCameraSpace) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("light_specular", light.specular) ;
        // material uniform value 
        shaderPrograms[shaderProgramIndex]->set_uniform_value("material_ambient", material.ambient) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("material_diffuse", material.diffuse) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("material_gloss", material.gloss) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("material_specular", material.specular) ;
        bomb.object->render() ;
        // blur effect
        shaderPrograms[shaderProgramIndex]->set_uniform_value("blurStrength", blurStrength);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("shake", shake);
    
        /*======================== pre-explosion rendering ========================*/ 
        
        if (time >= expansionStartTime && time <= (expansionStartTime + expansionDuration)) {
            bombExpansion.update(time);
            expansionScale = bombExpansion.getScale();
        }
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bombTexture);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("time", time) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("sparkStartTime", sparkStartTime) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("sparkDuration", sparkDuration) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("crackStartTime", crackStartTime) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("crackDuration", crackDuration) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("detachStartTime", detachStartTime) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("detachDuration", detachDuration) ;
        shaderPrograms[shaderProgramIndex]->set_uniform_value("ourTexture", 1) ;
        shaderPrograms[shaderProgramIndex]->release() ;
    }
    /*======================== Ground rendering ========================*/ 
    groundTextureIndex = (time < explosionTime) ? 0 : 2;
    glDisable(GL_CULL_FACE); 
    groundShader->use();
    int groundTextureUnit = 0;
    glActiveTexture(GL_TEXTURE0 + groundTextureUnit);
    glBindTexture(GL_TEXTURE_2D, groundTextures[groundTextureIndex]);
    groundShader->set_uniform_value("model", groundModel);
    groundShader->set_uniform_value("view", view);
    groundShader->set_uniform_value("projection", projection);
    groundShader->set_uniform_value("texture1", groundTextureUnit);
    groundShader->set_uniform_value("u_time", time);
    groundShader->set_uniform_value("blurStrength", blurStrength);
    groundShader->set_uniform_value("shake", shake);
    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    groundShader->release();
    glEnable(GL_CULL_FACE); 
    
    /*======================== cubemap environment rendering ========================*/ 
    cubemapIndex = (time < explosionTime) ? 1 : 2;
    cubemapShaders[cubemapIndex]->use() ;
    glm::mat4 cube_view = glm::mat4(glm::mat3(view)) ;
    cubemapShaders[cubemapIndex]->set_uniform_value("view", cube_view) ;
    cubemapShaders[cubemapIndex]->set_uniform_value("projection", projection) ;
    cubemapShaders[cubemapIndex]->set_uniform_value("cubemap", int(cubemapIndex)) ;
    shaderPrograms[cubemapIndex]->set_uniform_value("u_time", time);
    shaderPrograms[cubemapIndex]->set_uniform_value("blurStrength", blurStrength);
    shaderPrograms[cubemapIndex]->set_uniform_value("shake", shake);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextures[cubemapIndex]) ;
    glBindVertexArray(cubemapVAOs[cubemapIndex]) ;
    glDrawArrays(GL_TRIANGLES, 0, 2 * 6 * 3) ;
    glBindVertexArray(0) ;
    cubemapShaders[cubemapIndex]->release() ;

    /*======================== Particle rendering ========================*/
    if (time >= particleStartTime && time <= explosionTime) {
        glDisable(GL_CULL_FACE); 
        particleShader->use();
        particleShader->set_uniform_value("view", view);
        particleShader->set_uniform_value("projection", projection);
        for (Particle* particle : particles) {
            particleShader->set_uniform_value("model", particle->modelTransform);
            particleShader->set_uniform_value("particleSize", particle->size);
            particleShader->set_uniform_value("tint", particle->tint);  // fading color, not work temporarily use fixed variable in fragment shadere
            glBindVertexArray(ParticleVAO);
            glDrawArrays(GL_POINTS, 0, 1);
            glBindVertexArray(0);
        }
        particleShader->release();
        glEnable(GL_CULL_FACE); 
    }

    /*======================== Smoke rendering ========================*/
    if(smoke){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        SmokeShader->use();
        SmokeShader->set_uniform_value("view", view);
        SmokeShader->set_uniform_value("projection", projection);
        SmokeShader->set_uniform_value("u_time", Time);
        SmokeShader->set_uniform_value("blurStrength", blurStrength);
        SmokeShader->set_uniform_value("shake", shake);
        SmokeShader->set_uniform_value("light_pos", light.position);
        SmokeShader->set_uniform_value("redColor",glm::vec3(1.0,0.0,0.0));
        SmokeShader->set_uniform_value("yellowColor",glm::vec3(1.0, 0.5, 0.0));
        SmokeShader->set_uniform_value("grayColor",glm::vec3(0.5, 0.5, 0.5));
        
        for (const auto& p : smokeparticles) {
            
            if (p.life > 0.0f) {
                
                glm::mat4 model = glm::translate(glm::mat4(1.0f), p.position);
                smokemodel = glm::scale(model, glm::vec3(p.size)); // 調整大小
                SmokeShader->set_uniform_value("objectSize",p.size);
                SmokeShader->set_uniform_value("model", smokemodel);
                SmokeShader->set_uniform_value("alpha", p.life); // 用生命時間作為透明度

                Smoke.object->render(); // 假設 Smoke 是雲霧模型
            }
            //break;
        }

        SmokeShader->release();
        glDisable(GL_BLEND);
    }

    /*======================== Over Lay ========================*/
    if (time >= explosionTime && time <= explosionEndTime) {
        overlayAlpha = 1.0f - glm::clamp((time - explosionTime) / fadeDuration, 0.0f, 1.0f);
        glDisable(GL_DEPTH_TEST);  // Disable depth test to render overlay on top of everything
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        fadingShader->use();
        fadingShader->set_uniform_value("alpha", overlayAlpha);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        glEnable(GL_DEPTH_TEST);
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW4", NULL, NULL);
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
        shaderProgramIndex = 1;
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 2;
    if (key == GLFW_KEY_3 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 3;
    if (key == GLFW_KEY_4 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 4;
    if (key == GLFW_KEY_5 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 5;
    if (key == GLFW_KEY_6 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 6; // bomb

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

    // cubemap selection
    if (key == GLFW_KEY_Z && (action == GLFW_REPEAT || action == GLFW_PRESS))
        cubemapIndex = 0;// skybox
    if (key == GLFW_KEY_X && (action == GLFW_REPEAT || action == GLFW_PRESS))
        cubemapIndex = 1 ; // grass
    if (key == GLFW_KEY_C && (action == GLFW_REPEAT || action == GLFW_PRESS))
        cubemapIndex = 2; // mars

    // ground texture selection
    if (key == GLFW_KEY_V && (action == GLFW_REPEAT || action == GLFW_PRESS))
        groundTextureIndex = 0; // grass
    if (key == GLFW_KEY_B && (action == GLFW_REPEAT || action == GLFW_PRESS))
        groundTextureIndex = 1; // sand
    if (key == GLFW_KEY_N && (action == GLFW_REPEAT || action == GLFW_PRESS))
        groundTextureIndex = 2; // stones

    // shake and smoke effect
    if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        generateSmoke();
    }
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
