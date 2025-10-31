
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

int randRange(int min, int max) {
    return (rand() % (max - min + 1) + min);
}

float dist(float x1, float y1, float x2, float y2) {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

const int screenWidth = 1280;
const int screenHeight = 720;
const float gravity = 00.0f;
const float particleSeperation = 10000.0f;
const float repelStrength = 10.0f; // Adjusted for velocity forces

Color getColorForMass(int mass) {
    switch (mass) {
        case 1: return BLUE;
        case 2: return GREEN;
        case 3: return YELLOW;
        case 4: return ORANGE;
        case 5: return RED;
        default: return WHITE;
    }
}

struct Particle {
    float xPos;
    float yPos;

    float xVel = 0.0f;
    float yVel = 0.0f;

    float size = 2.0f;
    float mass = 1.0f;

    void draw() {
        Color c = getColorForMass(mass);
        DrawCircle(xPos, yPos, size, c);
    }

    void update(float dt) {
        // --- Edge repulsion parameters ---
        float edgeRepelStrength = 800.0f;   // how strong the wall pushes
        float edgeThreshold = 200.0f;       // how close before repelling starts

        // Left wall
        if (xPos < edgeThreshold) {
            float dist = std::max(xPos, 1.0f);
            float force = edgeRepelStrength / (dist * dist);
            xVel += force * dt;
        }

        // Right wall
        if (xPos > screenWidth - edgeThreshold) {
            float dist = std::max(screenWidth - xPos, 1.0f);
            float force = edgeRepelStrength / (dist * dist);
            xVel -= force * dt;
        }

        // Top wall
        if (yPos < edgeThreshold) {
            float dist = std::max(yPos, 1.0f);
            float force = edgeRepelStrength / (dist * dist);
            yVel += force * dt;
        }

        // Bottom wall
        if (yPos > screenHeight - edgeThreshold) {
            float dist = std::max(screenHeight - yPos, 1.0f);
            float force = edgeRepelStrength / (dist * dist);
            yVel -= force * dt;
        }

        // Apply gravity
        yVel += gravity * dt;

        // Apply damping for stability
        float damping = 0.99f;
        xVel *= damping;
        yVel *= damping;

        // Update position using velocity
        xPos += xVel * dt;
        yPos += yVel * dt;
    }
};

int main(void) {

    srand(time(NULL));
    InitWindow(screenWidth, screenHeight, "Fluid Sim");
    SetTargetFPS(60);

    const int particleNum = 1500;
    Particle particles[particleNum];

    // Initialize particles
    for (int i = 0; i < particleNum; i++) {
        particles[i].xPos = randRange(0, screenWidth);
        particles[i].yPos = randRange(0, screenHeight);
        particles[i].size = 2.0f; //+ particles[i].mass; // optional size by mass
        
        if (i < 0) particles[i].mass = 1;
        else particles[i].mass = randRange(1, 5);
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(10, 10);

        // --- Particle interactions ---
        for (int i = 0; i < particleNum; i++) {

            // Particle-particle repulsion
            for (int j = i + 1; j < particleNum; j++) {
                float dx = particles[j].xPos - particles[i].xPos;
                float dy = particles[j].yPos - particles[i].yPos;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance > 0.001f) {
                    float nx = dx / distance;
                    float ny = dy / distance;

                    // Soft repulsion (velocity-based)
                    if (distance < particleSeperation) {
                        float strength = (repelStrength / particles[i].mass) / (distance + 1.0f);
                        particles[i].xVel += nx * strength * dt;
                        particles[i].yVel += ny * strength * dt;
                        particles[j].xVel -= nx * strength * dt;
                        particles[j].yVel -= ny * strength * dt;
                    }

                    // Hard collision
                    float combinedSize = particles[i].size + particles[j].size;
                    if (distance < combinedSize) {
                        float overlap = 0.5f * (combinedSize - distance);
                        particles[i].xPos -= nx * overlap;
                        particles[i].yPos -= ny * overlap;
                        particles[j].xPos += nx * overlap;
                        particles[j].yPos += ny * overlap;

                        // Optional: basic elastic bounce
                        float tempXVel = particles[i].xVel;
                        float tempYVel = particles[i].yVel;
                        particles[i].xVel = particles[j].xVel;
                        particles[i].yVel = particles[j].yVel;
                        particles[j].xVel = tempXVel;
                        particles[j].yVel = tempYVel;
                    }
                }
            }

            // Mouse repulsion
            float dxm = particles[i].xPos - mouse.x;
            float dym = particles[i].yPos - mouse.y;
            float dist = std::sqrt(dxm * dxm + dym * dym);
            float mouseRadius = 50.0f;

            if (dist < mouseRadius + particles[i].size && dist > 0.001f) {
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

        // Update positions & draw
        for (int i = 0; i < particleNum; i++) {
            particles[i].update(dt);
            particles[i].draw();
        }

        EndDrawing();
    }

    CloseWindow();
}

