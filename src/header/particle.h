#include <bits/stdc++.h>
#include <glm/glm.hpp>

struct ParticleCreateInfo {
	glm::vec3 position, velocity, acceleration;
	float lifetime, size;
	glm::vec3 color;
};

class Particle {
public:
	glm::vec3 position, velocity, acceleration;
	glm::mat4 modelTransform;
	float t, lifetime, size;
	glm::vec3 color;
	glm::vec4 tint;
	Particle(ParticleCreateInfo* createInfo);
	void update(float rate);
};