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

    float size = 2.0f;
    float mass = 1.0f;

    Color color;

    void draw();

    /*
     *  This is the shared velocity helper method for all update function,
     *  helps streamline velocity resolution.
     */
    void velocityUpdate(float dt);

    using updateHandler = void(Particle::*)(float dt);
    updateHandler handler = nullptr; // pointer to current update function required

    void updateSelector(collisionMode mode); // Allows user to change update function in real time

    void update(float dt); // points to current update mode
    void fireUpdate(float dt); // update function for fire mode
    void wallUpdate(float dt); // update function for screen wall collision
    void wrapUpdate(float dt); // update function for screen wrapping
    void waterUpdate(float dt);
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
