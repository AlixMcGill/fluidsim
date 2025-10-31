#include "raylib.h"
#include <cstdlib>
#include <time.h>

int randRange(int min, int max) {
    return (rand() % (max - min + 1) + min);
}

const int screenWidth = 1280;
const int screenHeight = 720;
const float gravity = 17.0f;

struct Particle {
    float xPos;
    float yPos;

    float size = 2.0f;

    void draw() {
        DrawCircle(xPos, yPos, size, WHITE);
    };

    void update() {

        // screen bound collisions
        if (xPos < 0) xPos = 1;
        else if (xPos > screenWidth) xPos = screenWidth - 1;
        else if (yPos < 0) yPos = 1;
        else if (yPos > screenHeight) yPos = screenHeight - 1;

        // apply gravity
        yPos += gravity * GetFrameTime();
    }
};

int main(void) {
    
    srand(time(NULL));


    const int particleNum = 1500;

    Particle particles[particleNum]; // Particle Array
    
    for (int i = 0; i < particleNum; i++) { // Fills particle array
        Particle p;

        p.xPos = randRange(0, screenWidth);
        p.yPos = randRange(0, screenHeight);

        particles[i] = p;
    }

    InitWindow(screenWidth, screenHeight, "Fluid Sim");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            
            DrawFPS(10, 10);

            for (int i = 0; i < particleNum; i++) {
                particles[i].update();
                particles[i].draw();
            }

        EndDrawing();
    }

    CloseWindow();
}
