#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "rlgl.h"
#include "particle.h"

int main(void) {

    srand(time(NULL));
    InitWindow(screenWidth, screenHeight, "Fluid Sim");
    SetTargetFPS(60);

    Particle particles[particleNum];

    // Initialize particles
    for (int i = 0; i < particleNum; i++) {
        particles[i].xPos = randRange(0, screenWidth);
        particles[i].yPos = randRange(0, screenHeight);
        
        if (i < 0) particles[i].mass = 1;
        else particles[i].mass = randRange(1, 8);
    }

    // -- Shader stuff
    Shader shader = LoadShader(NULL, "/home/alix/dev/fluidsim/src/shaders/fireShader.fs");

    int resolutionLoc = GetShaderLocation(shader, "u_resolution");
    int pointCountLoc = GetShaderLocation(shader, "u_pointCount");
    int texLoc = GetShaderLocation(shader, "u_particleTex");
    //int pointsLoc = GetShaderLocation(shader, "u_points");

    Vector2 res = {screenWidth, screenHeight};
    SetShaderValue(shader, resolutionLoc, &res, SHADER_UNIFORM_VEC2);

    std::vector<Vector3> pixelData(particleNum); // pixel data for shader
    
    // texture to hold point data
    Image img = GenImageColor(particleNum, 1, BLACK);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R32G32B32);
    Texture2D particleDataTex = LoadTextureFromImage(img);
    UnloadImage(img);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(10, 10);

        // --- Init quadtree ---
        
        Quadtree qt(0,0, screenWidth, screenHeight, 8);

        for (int i = 0; i < particleNum; i++) {
            qt.insert(&particles[i]);
        }


        // --- Particle interactions ---

        for (int i = 0; i < particleNum; i++) {
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
                    /*
                    float force = G * (particles[i].mass * other->mass) / (distance * distance);

                    // Apply to velocities (i attracts j and vice versa)
                    particles[i].xVel += nx * force * dt;
                    particles[i].yVel += ny * force * dt;
                    other->xVel -= nx * force * dt;
                    other->yVel -= ny * force * dt;
                    */ 
                    // Soft repulsion (velocity-based) 
                    
                        if (distance < particleSeperation) { 
                            float strength = (repelStrength / particles[i].mass) / (distance + 1.0f); 
                            particles[i].xVel -= nx * strength * dt; 
                            particles[i].yVel -= ny * strength * dt; 
                            other->xVel += nx * strength * dt; 
                            other->yVel += ny * strength * dt; 
                        }

                    // Optional: keep hard collision for particle overlap
                    
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
            float dxm = particles[i].xPos - mouse.x;
            float dym = particles[i].yPos - mouse.y;
            float mouseMass = 40;
            float dist = std::sqrt(dxm * dxm + dym * dym);
            float mouseRadius = 50.0f;

            if (dist < mouseRadius + particles[i].size && dist > 0.001f) {
                float force = G * (particles[i].mass * mouseMass) / (dist * dist);

                // Apply to velocities (i attracts j and vice versa)
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    particles[i].xVel -= dxm * force * dt;
                    particles[i].yVel -= dym * force * dt;
                }
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    particles[i].xVel += dxm * force * dt;
                    particles[i].yVel += dym * force * dt;
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
        }



        // Update positions & draw
        for (int i = 0; i < particleNum; i++) {
            particles[i].update(dt);
            pixelData[i] = {particles[i].xPos, particles[i].yPos, particles[i].size};
        }

        UpdateTexture(particleDataTex, pixelData.data());

        SetShaderValue(shader, pointCountLoc, &particleNum, SHADER_UNIFORM_INT);
        SetShaderValueTexture(shader, texLoc, particleDataTex);

        BeginShaderMode(shader);
            DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
        EndShaderMode();

        EndDrawing();
    }

    UnloadTexture(particleDataTex);
    UnloadShader(shader);
    CloseWindow();
}

