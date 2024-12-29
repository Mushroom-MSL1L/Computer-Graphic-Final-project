#include "header/particle.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


Particle::Particle(ParticleCreateInfo* createInfo) {
	this->position = createInfo->position;
	this->velocity = createInfo->velocity;
	this->acceleration = createInfo->acceleration;
	this->size = createInfo->size;
	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, position);

	this->color = createInfo->color;
	this->lifetime = createInfo->lifetime;
	t = 0.0f;
}

void Particle::update(float rate) {
	velocity += rate * acceleration;
	position += rate * velocity;
	// if (position.y < 0) {
	// 	velocity.y *= -0.5f;
	// 	position.y = 0.0f;
	// 	velocity.x *= 0.5f;
	// 	velocity.z *= 0.5f;
	// }

	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, position);

	t += rate;
	tint = glm::vec4(color, 1.0f - (t / lifetime) / 2);
}

