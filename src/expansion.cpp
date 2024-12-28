#include "header/expansion.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


Expansion::Expansion(float startTime, float duration, float scaleSpeed)
    : startTime(startTime), duration(duration), scaleSpeed(scaleSpeed), scale(1.0f) {}

void Expansion::update(float currentTime) {
    if (currentTime >= startTime && currentTime <= (startTime + duration)) {
        float normalizedTime = (currentTime - startTime) / duration;
        float oscillation = sin(normalizedTime * glm::pi<float>() * 2.0f * scaleSpeed);
        scale = 1.0f + 0.2f * oscillation;
    }
}

float Expansion::getScale() const {
    return scale;
}
