#pragma once

const int screenWidth = 800;
const int screenHeight = 600;

const int particleNum = 10000;
const float gravity = 0.0f;
const float particleSeperation = 0.0f;
const float repelStrength = 0.0f; // Adjusted for velocity forces
const float G = 100.0f;
const float maxParticleVel = 50.0f;

// Particle collision types
enum collisionMode {
    MODE_A_FIRE_COLL,
    MODE_B_WALL_COLL,
    MODE_C_WRAP_COLL,
    MODE_D_WATER_COLL
};

const collisionMode collmode = MODE_B_WALL_COLL;
