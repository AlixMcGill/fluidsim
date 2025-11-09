#pragma once

const int screenWidth = 1920;
const int screenHeight = 1080;

const int particleNum = 8000;
const float gravity = 300.0f;
const float particleSeperation = 100.0f;
const float repelStrength = 500.0f; // Adjusted for velocity forces
const float G = 0.0f;
const float maxParticleVel = 500.0f;

// Particle collision types
enum collisionMode {
    MODE_A_FIRE_COLL,
    MODE_B_WALL_COLL,
    MODE_C_WRAP_COLL,
    MODE_D_WATER_COLL
};

const collisionMode collmode = MODE_D_WATER_COLL;
