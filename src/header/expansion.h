#ifndef EXPANSION_H
#define EXPANSION_H

#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

class Expansion {
public:
    Expansion(float startTime, float duration, float scaleSpeed);

    void update(float currentTime);
    float getScale() const;

private:
    float startTime;
    float duration;
    float scaleSpeed;
    float scale;
};

#endif // EXPANSION_H
