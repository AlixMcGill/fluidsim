#include "particlesim.h"
#include <raylib.h>

// Constructor
ParticleSim::ParticleSim() {

}

// Destructor
ParticleSim::~ParticleSim() {
    UnloadTexture(particleDataTex);
    UnloadShader(shader);
}

/* 
 * ====================================
 * ========== Public Methods ==========
 * ====================================
 */

void ParticleSim::init() {
    initParticles();
    initShader();
}

void ParticleSim::update(float dt, const Vector2 &mousePos) {

    // --- Init quadtree ---
    
    Quadtree qt(0,0, screenWidth, screenHeight, 2);

    for (int i = 0; i < particleNum; i++) {
        qt.insert(&particles[i]);
    }


    // --- Particle interactions ---
    std::vector<Vector2> velocityDelta(particleNum, {0,0});
    
    #pragma omp parallel for
    for (int i = 0; i < particleNum; i++) {
        Vector2 delta = {0,0};
        std::vector<Particle*> neighbors;
        qt.query(particles[i].xPos, particles[i].yPos, 50.0f, neighbors);

        // Particle-particle repulsion
        
        for (Particle* other : neighbors) { 
            if (&particles[i] == other) continue;

            float dx = other->xPos - particles[i].xPos;
            float dy = other->yPos - particles[i].yPos;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance > 0.001f) {
                float nx = dx / distance;
                float ny = dy / distance;

                // Gravitational force magnitude
                
                if (pGrav) {
                    float force = G * (particles[i].mass * other->mass) / (distance * distance);

                    // Apply to velocities (i attracts j and vice versa)
                    delta.x += nx * force * dt;
                    delta.y += ny * force * dt;
                    //other->xVel -= nx * force * dt;
                    //other->yVel -= ny * force * dt;
                }
                
                // Soft repulsion (velocity-based) 
                
                if (distance < particleSeperation) { 
                    float strength = (repelStrength / particles[i].mass) / (distance + 1.0f); 
                    delta.x -= nx * strength * dt; 
                    delta.y -= ny * strength * dt; 
                    /*other->xVel += nx * strength * dt; 
                    other->yVel += ny * strength * dt; */
                }

                // Particle hard collision
                
                float combinedSize = particles[i].size + other->size;
                if (distance < combinedSize) {
                    float overlap = 0.5f * (combinedSize - distance);
                    particles[i].xPos -= nx * overlap;
                    particles[i].yPos -= ny * overlap;
                    other->xPos += nx * overlap;
                    other->yPos += ny * overlap;

                    // Elastic bounce
                    float tempXVel = particles[i].xVel * 0.95;
                    float tempYVel = particles[i].yVel * 0.95;
                    particles[i].xVel = other->xVel;
                    particles[i].yVel = other->yVel;
                    other->xVel = tempXVel;
                    other->yVel = tempYVel;
                }
            }
        }

        // Mouse repulsion
        float dxm = particles[i].xPos - mousePos.x;
        float dym = particles[i].yPos - mousePos.y;
        float mouseMass = 40;
        float dist = std::sqrt(dxm * dxm + dym * dym);
        float mouseRadius = 50.0f;

        if (dist < mouseRadius + particles[i].size && dist > 0.001f) {
            float force = G * (particles[i].mass * mouseMass) / (dist * dist);

            // Apply to velocities (i attracts j and vice versa)
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                delta.x -= dxm * force * dt;
                delta.y -= dym * force * dt;
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                delta.x += dxm * force * dt;
                delta.y += dym * force * dt;
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
                // Collision normal
                float nx = dxm / dist;
                float ny = dym / dist;

                // Push particle outside the mouse
                float overlap = (mouseRadius + particles[i].size) - dist;
                particles[i].xPos += nx * overlap;
                particles[i].yPos += ny * overlap;

                // Reflect velocity along normal for bounce
                float dot = particles[i].xVel * nx + particles[i].yVel * ny;
                particles[i].xVel -= 2 * dot * nx;
                particles[i].yVel -= 2 * dot * ny;

                // Optional damping to reduce jitter
                particles[i].xVel *= 0.8f;
                particles[i].yVel *= 0.8f;
            }
        }

        velocityDelta[i] = delta;
    }

    // Update Particle Positions and pixelData;
    for (int i = 0; i < particleNum; i++) {
        particles[i].xVel += velocityDelta[i].x;
        particles[i].yVel += velocityDelta[i].y;
        particles[i].update(dt);
        pixelData[i] = {particles[i].xPos, particles[i].yPos, particles[i].size};
    }

    // sends pixel data to gpu
    UpdateTexture(particleDataTex, pixelData.data());
}

void ParticleSim::draw() { 
    SetShaderValue(shader, pointCountLoc, &particleNum, SHADER_UNIFORM_INT);
    SetShaderValueTexture(shader, texLoc, particleDataTex);

    BeginShaderMode(shader);
        DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
    EndShaderMode();
}

/*
 * =====================================
 * ========== Private Methods ==========
 * =====================================
 */

// init all particles in sim
void ParticleSim::initParticles() {
    for (int i = 0; i < particleNum; i++) {
        particles[i].updateSelector(collmode); // check collision mode pointer
        particles[i].xPos = randRange(0, screenWidth);
        particles[i].yPos = randRange(0, screenHeight);
        particles[i].mass = randRange(2, 2);
    }
}

/*
 * Loads shader into memory
 * initialize texture buffer
 */
void ParticleSim::initShader() {
    pixelData.resize(particleNum);

    shader = LoadShader(NULL, "/home/alix/dev/fluidsim/src/shaders/blueSmoke.fs");
    resolutionLoc = GetShaderLocation(shader, "u_resolution");
    pointCountLoc = GetShaderLocation(shader, "u_pointCount");
    texLoc = GetShaderLocation(shader, "u_particleTex");

    Vector2 res = {screenWidth, screenHeight};
    SetShaderValue(shader, resolutionLoc, &res, SHADER_UNIFORM_VEC2);

    Image img = GenImageColor(particleNum, 1, BLACK);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R32G32B32);
    particleDataTex = LoadTextureFromImage(img);
    UnloadImage(img);
}
