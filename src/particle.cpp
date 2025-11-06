#include "particle.h"
#include "util/globals.h"


/*
 *
 * --------- Particle --------
 *
 * */

void Particle::draw() {
    color = getColorForMass(mass);
    DrawCircle(xPos, yPos, size, color);
}

void Particle::velocityUpdate(float dt) {
    // Apply damping for stability
    float damping = 0.98f;
    xVel *= damping;
    yVel *= damping;

    // clamping max velocity
    if (xVel > maxParticleVel)       xVel = maxParticleVel;
    else if (xVel < -maxParticleVel) xVel = -maxParticleVel;
    if (yVel > maxParticleVel)       yVel = maxParticleVel;
    else if (yVel < -maxParticleVel) yVel = -maxParticleVel;

    // Update position using velocity
    xPos += xVel * dt;
    yPos += yVel * dt;
}

void Particle::updateSelector(collisionMode mode) {
    switch (mode) {
        case collisionMode::MODE_A_FIRE_COLL:
            handler = &Particle::fireUpdate;
            break;
        case collisionMode::MODE_B_WALL_COLL:
            handler = &Particle::wallUpdate;
            break;
        case collisionMode::MODE_C_WRAP_COLL:
            handler = &Particle::wrapUpdate;
            break;
        case collisionMode::MODE_D_WATER_COLL:
            handler = &Particle::waterUpdate;
            break;
        default:
            handler = &Particle::wallUpdate;
            break;
    }
}

void Particle::update(float dt) {
    (this->*handler)(dt);
}

void Particle::fireUpdate(float dt) {
    // Left wall
    if (xPos < 0) {
        xPos = screenWidth / 2.0f;
        yPos = screenHeight / 1.2f;
    }
    // Right wall
    if (xPos > screenWidth) {
        xPos = screenWidth / 2.0f;
        yPos = screenHeight / 1.2f;
    }

    // Top wall
    if (yPos < 0) {
        xPos = screenWidth / 2.0f;
        yPos = screenHeight / 1.2f;
        yVel = 0.0;
        xPos += randRange(-10, 10);
    }

    // Bottom wall
    if (yPos > screenHeight) {
        xPos = screenWidth / 2.0f;
        yPos = screenHeight / 1.2f;
    }
    // Apply gravity
    yVel += gravity * dt;
    xVel += randRange(-6, 6) / mass;
    yVel += randRange(-2, 2) / mass; 
    velocityUpdate(dt);
}

void Particle::wallUpdate(float dt) {
    // --- Edge repulsion parameters ---
    float edgeRepelStrength = 800.0f;   // how strong the wall pushes
    float edgeThreshold = 5.0f;       // how close before repelling starts

    // Left wall
    if (xPos < 0) {
        float dist = std::max(xPos, 1.0f);
        float force = edgeRepelStrength / (dist * dist);
        xVel += force * dt;
    }
    // Right wall
    if (xPos > screenWidth) {
        float dist = std::max(screenWidth - xPos, 1.0f);
        float force = edgeRepelStrength / (dist * dist);
        xVel -= force * dt;
    }
    // Top wall
    if (yPos < 0) {
        float dist = std::max(yPos, 1.0f);
        float force = edgeRepelStrength / (dist * dist);
        yVel += force * dt;
    }
    // Bottom wall
    if (yPos > screenHeight) {
        float dist = std::max(screenHeight - yPos, 1.0f);
        float force = edgeRepelStrength / (dist * dist);
        yVel -= force * dt;
    }

    // Apply gravity
    yVel += gravity * dt;

    xVel += randRange(-1, 1) / mass;
    yVel += randRange(-1, 1) / mass;
    
    velocityUpdate(dt);
}

void Particle::wrapUpdate(float dt) {

}

void Particle::waterUpdate(float dt) {
    // --- Edge repulsion parameters ---
    float edgeRepelStrength = 800.0f;   // how strong the wall pushes
    float edgeThreshold = 5.0f;       // how close before repelling starts

    // Left wall
    if (xPos < 0) {
        float dist = std::max(xPos, 1.0f);
        float force = edgeRepelStrength / (dist * dist);
        xVel += force * dt;
    }
    // Right wall
    if (xPos > screenWidth) {
        xPos = 5;
        yPos = 5;
    }
    // Top wall
    if (yPos < 0) {
        float dist = std::max(yPos, 1.0f);
        float force = edgeRepelStrength / (dist * dist);
        yVel += force * dt;
    }
    // Bottom wall
    if (yPos > screenHeight) {
        float dist = std::max(screenHeight - yPos, 1.0f);
        float force = edgeRepelStrength / (dist * dist);
        yVel -= force * dt;
    }

    // Apply gravity
    yVel += gravity * dt;

    xVel += randRange(-1, 1) / mass;
    yVel += randRange(-1, 1) / mass;
    
    velocityUpdate(dt);

}

/*
 *
 * --------- Quad Tree Algo --------
 *
 * */
Quadtree::Quadtree(float _x, float _y, float _w, float _h, int _capacity)
    : x(_x), y(_y), width(_w), height(_h), capacity(_capacity) {

}

Quadtree::~Quadtree() {
    delete nw; delete ne; delete sw; delete se;
}

bool Quadtree::insert(Particle* p) {
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

void Quadtree::subdivide() {
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
void Quadtree::query(float qx, float qy, float qr, std::vector<Particle*>& found) {
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
