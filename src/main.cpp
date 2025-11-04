#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "rlgl.h"
#include <iostream>

int randRange(int min, int max) {
    return (rand() % (max - min + 1) + min);
}

float dist(float x1, float y1, float x2, float y2) {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

const int screenWidth = 1280;
const int screenHeight = 720;
const float gravity = 15.0f;
const float particleSeperation = 50.0f;
const float repelStrength = 35.0f; // Adjusted for velocity forces
const float G = 100.0f;

Color getColorForMass(int mass) {
    switch (mass) {
        case 1: return BLUE;
        case 2: return GREEN;
        case 3: return YELLOW;
        case 4: return ORANGE;
        case 5: return RED;
        case 6: return PURPLE;
        case 7: return PINK;
        default: return WHITE;
    }
}

struct Particle {
    float xPos;
    float yPos;

    float xVel = 0.0f;
    float yVel = 0.0f;

    float size = 1.0f;
    float mass = 1.0f;

    Color color;

    void draw() {
        color = getColorForMass(mass);
        DrawCircle(xPos, yPos, size, color);
    }

    void update(float dt) {
        // --- Edge repulsion parameters ---
        float edgeRepelStrength = 800.0f;   // how strong the wall pushes
        float edgeThreshold = 5.0f;       // how close before repelling starts

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
            //yPos = 2;
            //yVel *= 0.95;
            float dist = std::max(screenHeight - yPos, 1.0f);
            float force = edgeRepelStrength / (dist * dist);
            yVel -= force * dt;
        }

        // Apply gravity
        yVel += gravity * dt;

        xVel += randRange(-1, 1) / mass;
        yVel += randRange(-1, 1) / mass;

        // Apply damping for stability
        /*float damping = 0.99f;
        xVel *= damping;
        yVel *= damping;*/

        float maxVelocity = 150.0f;
        // clamping max velocity
        if (xVel > maxVelocity)       xVel = maxVelocity;
        else if (xVel < -maxVelocity) xVel = -maxVelocity;
        if (yVel > maxVelocity)       yVel = maxVelocity;
        else if (yVel < -maxVelocity) yVel = -maxVelocity;

        // Update position using velocity
        xPos += xVel * dt;
        yPos += yVel * dt;
    }
};

struct Quadtree {
    float x, y, width, height; // Bounds of the node
    int capacity; // max particles
    std::vector<Particle*> quadparts;
    bool divided = false;
    Quadtree* nw = nullptr;
    Quadtree* ne = nullptr;
    Quadtree* sw = nullptr;
    Quadtree* se = nullptr;

    Quadtree(float _x, float _y, float _w, float _h, int _capacity)
        : x(_x), y(_y), width(_w), height(_h), capacity(_capacity) {

    }

    ~Quadtree() {
        delete nw; delete ne; delete sw; delete se;
    }

    bool insert(Particle* p) {
        if (p->xPos < x || p->xPos > x + width || p->yPos < y || p->yPos > y + height)
            return false;

        if (quadparts.size() < capacity) {
            quadparts.push_back(p);
            return true;
        }

        if (!divided) subdivide();

        if (nw->insert(p)) return true;
        if (ne->insert(p)) return true;
        if (sw->insert(p)) return true;
        if (se->insert(p)) return true;

        return false;
    }

    void subdivide() {
        float hw = width / 2;
        float hh = height / 2;

        nw = new Quadtree(x, y, hw, hh, capacity);
        ne = new Quadtree(x + hw, y, hw, hh, capacity);
        sw = new Quadtree(x, y + hh, hw, hh, capacity);
        se = new Quadtree(x + hw, y + hh, hw, hh, capacity);

        divided = true;
    }

    // qx current particle x position
    // qy current particle y position
    // qr query radius
    // found, vector of found particles in query radius
    void query(float qx, float qy, float qr, std::vector<Particle*>& found) {
        if (qx + qr < x || qx - qr > x + width || qy + qr < y || qy - qr > y + height)
            return;

        for (Particle* p : quadparts) {
            float dx = p->xPos - qx;
            float dy = p->yPos - qy;
            if (dx*dx + dy*dy <= qr*qr) {
                found.push_back(p);
            }
        }

        if (!divided) return;

        nw->query(qx,  qy,  qr, found);
        ne->query(qx,  qy,  qr, found);
        sw->query(qx,  qy,  qr, found);
        se->query(qx,  qy,  qr, found);
    }
};

int main(void) {

    srand(time(NULL));
    InitWindow(screenWidth, screenHeight, "Fluid Sim");
    SetTargetFPS(60);

    const int particleNum = 1000;
    Particle particles[particleNum];

    // Initialize particles
    for (int i = 0; i < particleNum; i++) {
        particles[i].xPos = randRange(0, screenWidth);
        particles[i].yPos = randRange(0, screenHeight);
        
        if (i < 0) particles[i].mass = 1;
        else particles[i].mass = randRange(1, 1);
    }

    // -- Shader stuff
    Shader shader = LoadShader(NULL, "/home/alix/dev/fluidsim/src/particle.fs");

    int resolutionLoc = GetShaderLocation(shader, "u_resolution");
    int pointCountLoc = GetShaderLocation(shader, "u_pointCount");
    int pointsLoc = GetShaderLocation(shader, "u_points");

    Vector2 res = {screenWidth, screenHeight};
    SetShaderValue(shader, resolutionLoc, &res, SHADER_UNIFORM_VEC2);

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

        Vector3 pointLocations[particleNum];

        // Update positions & draw
        for (int i = 0; i < particleNum; i++) {
            particles[i].update(dt);
            pointLocations[i].x = particles[i].xPos;
            pointLocations[i].y = particles[i].yPos;
            pointLocations[i].z = particles[i].size;
        }

        SetShaderValue(shader, pointCountLoc, &particleNum, SHADER_UNIFORM_INT);
        SetShaderValueV(shader, pointsLoc, pointLocations, SHADER_UNIFORM_VEC3, particleNum);

        BeginShaderMode(shader);
            DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
        EndShaderMode();

        EndDrawing();
    }

    UnloadShader(shader);
    CloseWindow();
}

