#pragma once
#include "raylib.h"
#include "cmath"
#include <cstdlib>


/*
 * Return a random int between two numbers
 *
 * */
int randRange(int min, int max); 

/*
 * Returns distance between two points as a float
 *
 * */
float dist(float x1, float y1, float x2, float y2);

/*
 * Returns a raylib color between 1 and 7, white if not depending on
 * int passed in
 *
 * */
Color getColorForMass(int mass);
