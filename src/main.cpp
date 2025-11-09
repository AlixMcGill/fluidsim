#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "rlgl.h"
#include "particle.h"
#include "sims/fluidsimI.h"
#include "sims/particlesim.h"
#include "util/globals.h"

int main(void) {

    srand(time(NULL));
    InitWindow(screenWidth, screenHeight, "Fluid Sim");
    SetTargetFPS(60);

    FluidSimI *sim = nullptr;
    bool useParticleSim = true;

    if (useParticleSim) {
        sim = new ParticleSim();
    } else {
        // sim = new EulerianFluidSim();
    }

    sim->init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangle(0, 0, screenWidth, screenHeight, WHITE); // Do not remove Primes frame buffer for raylib

        sim->update(dt, mouse);
        sim->draw();

        DrawFPS(10, 10);

        EndDrawing();
    }

    delete sim;
    CloseWindow();
    return 0;
}

