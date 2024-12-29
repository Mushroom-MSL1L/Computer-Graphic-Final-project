# Computer-Graphics-Final-project

### Title: 通向地獄的煙火, Fireworks to the hell

## Overview
This project is final project of ICG course in NYCU, creating 30s short animation by using OpenGL with geometry shader.

## How to Run the Code
1. `git clone`: https://github.com/Mushroom-MSL1L/Computer-Graphic-Final-project.git
   
2. Place the `glm` folder (prepare it yourself) into the `extern` directory. This folder corresponds to `extern/glm` used in ICG-HW3.

3. If a `build` folder already exists in the project directory, delete it.

4. Run the code(cmake). Note that it may take 10 seconds or longer to execute.

## Features
We focused on the explosion of a bomb in our project, applying the techniques we learned in class.
- **Pre-Explosion Effects**:
  - **Spark Generation**: Geometry shader to create random sparks.(bomb.geom)
  - **Pulsating Expansion**: Sine wave-based vertex normal displacement.(expansion.cpp)
  - **Crack Simulation**: Fragment normal modification for visual cracks.(bomb.geom)
  - **Flare**: Yellow primitives to mimic pre-explosion glow.(bomb.geom)

- **Explosion Effects**:
  - **Particle System**: CPU-driven particle generation with geometry shader transformation for fire simulation. (main.cpp, particle.vert, particle.geom, particle.frag, particle.cpp)
  - **Scene Shaking**: Trigonometric functions for texture-based shaking effects.(main.cpp, for each fragment shader)
  - **Blur Effect**: Average-based blurring for post-explosion visuals.
  (main.cpp, for each fragment shader)
  - **Cloud Enlargement**: Particle class for smoke growth and gravity effects.(main.cpp, smoke.vert, smoke.frag)
  - **Cloud Shading**: Toon shading for realistic smoke transitions.
  (main.cpp, smoke.vert, smoke.frag)
- **Scene Transitions**:
  - **Fading Effect**: Alpha blending to simulate explosion brightness and glow.(main.cpp)

## Dependencies


This project requires the following files and assets. Please ensure the directory structure is maintained correctly.

#### Source Code
- `main.cpp` - Main execution file.
- `expansion.cpp` - Implementation of explosion effects.
- `particle.cpp` - Implementation of the particle system.

#### Assets
- `asset/obj/cloud.obj` - Cloud object used in the scene.
- `asset/obj/missile.obj` - Missile object (bomb).
- `asset/texture/grass/` - Background texture before explosion.
- `asset/texture/mars/` - Background texture after explosion.
- `asset/texture/Grass.jpg` - Ground texture before explosion.
- `asset/texture/missile_baseColor.png` - Texture for the missile.
- `asset/texture/Stones.jpg` - Ground texture after explosion.

#### Shader Code
- `asset/shaders/bomb.vert`, `frag`, `geom` - Shader code for the missile.
- `asset/shaders/cubemap.vert`, `frag` - Shader code for the cubemap.
- `asset/shaders/fading.vert`, `frag` - Shader code for the fading effect.
- `asset/shaders/particle.vert`, `geom`, `frag` - Shader code for the particle effects.
- `asset/shaders/smoke.vert`, `frag` - Shader code for smoke effects.
- `asset/shaders/ground.vert`, `frag` - Shader code for the ground texture.

## Works
URL: 

## Contributors ✨
Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tbody>
    <tr>
      <td align="center" valign="top" width="14.28%"><a href="https://github.com/Mushroom-MSL1L"><img src="https://avatars.githubusercontent.com/u/136601880?v=4?s=100" width="100px;" alt="Lu Junyou"/><br /><sub><b>Lu Junyou</b></sub></a><br /><a href="https://github.com/Mushroom-MSL1L/Computer-Graphic-Final-project/commits?author=Mushroom-MSL1L" title="Code">💻</a></td>
      <td align="center" valign="top" width="14.28%"><a href="https://github.com/Kennethii2i"><img src="https://avatars.githubusercontent.com/u/125580757?v=4?s=100" width="100px;" alt="Kennethii2i"/><br /><sub><b>Kennethii2i</b></sub></a><br /><a href="https://github.com/Mushroom-MSL1L/Computer-Graphic-Final-project/commits?author=Kennethii2i" title="Code">💻</a></td>
      <td align="center" valign="top" width="14.28%"><a href="https://github.com/Ray910907"><img src="https://avatars.githubusercontent.com/u/136991406?v=4?s=100" width="100px;" alt="Ray910907"/><br /><sub><b>Ray910907</b></sub></a><br /><a href="https://github.com/Mushroom-MSL1L/Computer-Graphic-Final-project/commits?author=Ray910907" title="Code">💻</a></td>
    </tr>
  </tbody>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

