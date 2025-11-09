#pragma once
#include "fluidsimI.h"
#include "../util/globals.h"
#include "../particle.h"
#include <raylib.h>
#include <vector>

struct ParticleSim : public FluidSimI {
public:
   Particle particles[particleNum];
   std::vector<Vector3> pixelData;
   Shader shader;
   Texture2D particleDataTex;
   int pointCountLoc;
   int resolutionLoc;
   int texLoc;
   bool pGrav = false; // particle to particle gravity

   ParticleSim();
   ~ParticleSim();

   void init() override;
   void update(float dt, const Vector2 &mousepos) override;
   void draw() override;
private:
   void initParticles();
   void initShader();
};
