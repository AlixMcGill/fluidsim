#pragma once
#include <raylib.h>
#include <vector>
#include "./util/util.h"
#include "./util/globals.h"

struct Particle {
    float xPos;
    float yPos;

    float xVel = 0.0f;
    float yVel = 0.0f;

    float size = 0.5f;
    float mass = 1.0f;

    Color color;

    void draw();

    void update(float dt);
};

/*
 * Quadtree for spacial optimization
 *
 * */
struct Quadtree {
    float x, y, width, height; // Bounds of the node
    int capacity; // max particles
    std::vector<Particle*> quadparts;
    bool divided = false;
    Quadtree* nw = nullptr;
    Quadtree* ne = nullptr;
    Quadtree* sw = nullptr;
    Quadtree* se = nullptr;

    Quadtree(float _x, float _y, float _w, float _h, int _capacity);
    ~Quadtree();

    bool insert(Particle* p);

    void subdivide();

    // qx current particle x position
    // qy current particle y position
    // qr query radius
    // found, vector of found particles in query radius
    void query(float qx, float qy, float qr, std::vector<Particle*>& found);
};
