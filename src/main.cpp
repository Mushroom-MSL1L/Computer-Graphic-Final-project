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

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(std::vector<string> &mFileName);
void load_image(std::string texturePath);

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
int shaderProgramIndex = 0;
std::vector<shader_program_t*> shaderPrograms;

// additional dependencies
light_t light;
material_t material;
camera_t camera;
model_t helicopter;
model_t helicopterBlade;

// model matrix
int moveDir = -1;
glm::mat4 helicopterModel;
glm::mat4 helicopterBladeModel;
glm::mat4 cameraModel;

// Parameter setup, 
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

void particle_model_setup() {
    /* ======================== particle system ========================*/
    particleSystem.position = glm::vec3(0, 0, 0);
    particleSystem.incident = glm::vec3(0, 0.5, 0);                 // direction for emit direction and velocity
    particleSystem.normal = glm::normalize(glm::vec3(0, 1, 0));     // spread direction
    particleSystem.acceleration = glm::vec3(0, -0.1, 0);
    particleSystem.baseSize = 0.15f;                                 // size of particle, will be randomized later
    particleSystem.baseLifetime = 100.0f;                           // lifetime of particle, will be randomized later
    particleSystem.generateParticleNumeber = 4 ;
    particleSystem.randomFactor = 0.05f;

    /* ======================== VAO, VBO =======================*/
    float vertices[] = { 0.0f, 1.0f, 0.0f }; // real position of emitter

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

void heli_model_setup(){
// Load the object and texture for each model here 
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
}

void heli_shader_setup(){
    // Setup the shader program for each shading method
    std::vector<std::string> shadingMethod = {
        "default",                              // default shading
        "bling-phong", "gouraud", "metallic",   // addional shading effects (basic)
        "glass_schlick", "glass_empricial",     // addional shading effects (advanced)
    };

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
}

void ground_model_setup() {
    groundModel = glm::mat4(1.0f);
    ground.position = glm::vec3(0.0f, -0.5f, 0.0f);         
    // ground.scale = glm::vec3(1000.0f, 1000.0f, 1000.0f); // for video show
    ground.scale = glm::vec3(10.0f, 10.0f, 10.0f);          // for test
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

void setup(){
    // Initialize shader model camera light material
    light_setup();
    // heli_model_setup();
    // heli_shader_setup();
    ground_model_setup();
    ground_shader_setup();
    camera_setup();
    cubemap_setup();
    material_setup();
    particle_model_setup();
    particle_shader_setup();

    // Enable depth test, face culling ...
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Debug: enable for debugging
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback([](  GLenum source, GLenum type, GLuint id, GLenum severity, 
    //                             GLsizei length, const GLchar* message, const void* userParam) {

    // std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
    //           << "type = " << type 
    //           << ", severity = " << severity 
    //           << ", message = " << message << std::endl;
    // }, nullptr);
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

void update(){

// Update the heicopter position, camera position, rotation, etc.

    // helicopter.position.y += moveDir;
    // if(helicopter.position.y > 20.0 || helicopter.position.y < -100.0){
    //     moveDir = -moveDir;
    // }

    // helicopterBlade.rotation.y += 10;
    // if(helicopterBlade.rotation.y > 360.0){
    //     helicopterBlade.rotation.y = 0.0;
    // }

    // helicopterModel = glm::mat4(1.0f);
    // helicopterModel = glm::scale(helicopterModel, helicopter.scale);
    // helicopterModel = glm::translate(helicopterModel, helicopter.position);

    // helicopterBladeModel = glm::rotate(helicopterModel, glm::radians(helicopterBlade.rotation.y), glm::vec3(0.0, 1.0, 0.0));

    camera.rotationY = (camera.rotationY > 360.0) ? 0.0 : camera.rotationY;
    cameraModel = glm::mat4(1.0f);
    cameraModel = glm::rotate(cameraModel, glm::radians(camera.rotationY), camera.up);
    cameraModel = glm::translate(cameraModel, camera.position);

    groundModel = glm::mat4(1.0f);
    groundModel = glm::scale(groundModel, ground.scale);
    groundModel = glm::translate(groundModel, ground.position);

    makeParticles(&particleSystem);
	for (int i = 0; i < particles.size(); ++i) {
		Particle* particle = particles[i];
		particle->update(rate);

		if (particle->t >= particle->lifetime) {
			delete particle;
			particles.erase(particles.begin() + i--);
		}
	}
}

void render(){

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate view, projection matrix
    glm::mat4 view = glm::lookAt(glm::vec3(cameraModel[3]), glm::vec3(0.0), camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    // // Set matrix for view, projection, model transformation
    // shaderPrograms[shaderProgramIndex]->use();
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("model", helicopterModel);
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("view", view);
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("projection", projection);
    
    // // TODO 1
    // // Set uniform value for each shader program
    // if (shaderProgramIndex == 1) {
    //     light.specular = glm::vec3(3.0);
    //     material.gloss = 10.5;
    // } else if (shaderProgramIndex == 2) {
    //     light.specular = glm::vec3(4.0);
    //     material.gloss = 48;
    // } else {
    //     light.specular = glm::vec3(1.0);
    //     material.gloss = 10.5;
    // }
    // // camera uniform value
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("camera_position", cameraModel[3]) ;
    // // light uniform value
    // glm::vec3 lightPositionInCameraSpace = glm::vec3(cameraModel * glm::vec4(light.position, 1.0f));
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("light_ambient", light.ambient) ;
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("light_diffuse", light.diffuse) ;
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("light_position", lightPositionInCameraSpace) ;
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("light_specular", light.specular) ;
    // // material uniform value 
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("material_ambient", material.ambient) ;
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("material_diffuse", material.diffuse) ;
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("material_gloss", material.gloss) ;
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("material_specular", material.specular) ;
    
    // // reflection uniform value    
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("cubemap", int(cubemapIndex)) ;
    // // End of TODO 1

    // helicopter.object->render();
    // shaderPrograms[shaderProgramIndex]->set_uniform_value("model", helicopterBladeModel);
    // helicopterBlade.object->render();
    // shaderPrograms[shaderProgramIndex]->release();

    /*======================== Ground rendering ========================*/ 
    glDisable(GL_CULL_FACE); 
    groundShader->use();
    int groundTextureUnit = 0;
    glActiveTexture(GL_TEXTURE0 + groundTextureUnit);
    glBindTexture(GL_TEXTURE_2D, groundTextures[groundTextureIndex]);
    groundShader->set_uniform_value("model", groundModel);
    groundShader->set_uniform_value("view", view);
    groundShader->set_uniform_value("projection", projection);
    groundShader->set_uniform_value("texture1", groundTextureUnit);
    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    groundShader->release();
    glEnable(GL_CULL_FACE); 
    
    /*======================== cubemap environment rendering ========================*/ 
    cubemapShaders[cubemapIndex]->use() ;
    glm::mat4 cube_view = glm::mat4(glm::mat3(view)) ;
    cubemapShaders[cubemapIndex]->set_uniform_value("view", cube_view) ;
    cubemapShaders[cubemapIndex]->set_uniform_value("projection", projection) ;
    cubemapShaders[cubemapIndex]->set_uniform_value("cubemap", int(cubemapIndex)) ;
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextures[cubemapIndex]) ;
    glBindVertexArray(cubemapVAOs[cubemapIndex]) ;
    glDrawArrays(GL_TRIANGLES, 0, 2 * 6 * 3) ;
    glBindVertexArray(0) ;
    cubemapShaders[cubemapIndex]->release() ;

    /*======================== Particle rendering ========================*/
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
