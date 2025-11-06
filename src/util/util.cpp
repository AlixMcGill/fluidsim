#include "util.h"

int randRange(int min, int max) {
    return (rand() % (max - min + 1) + min);
}

float dist(float x1, float y1, float x2, float y2) {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

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
