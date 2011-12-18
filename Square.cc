#include "Square.h"

// Ant=0 is friendly
// Ant=-1 is no ant
// Ant>1 is enemy

Square::Square()
{
    isVisible = isWater = isHill = isFood = 0;
    priority = 0;
    ant = hillPlayer = -1;
    permpriority = 0;
    defensepriority = 0;
    updateIndex =0;

    parent = -1;

    moved = 0;
};

void Square::reset()
{
    isVisible = 0;
    isHill = 0;
    isFood = 0;
    priority = permpriority;

    defensepriority = 0;

    updateIndex = 0;
    ant = hillPlayer = -1;
    deadAnts.clear();

    parent = -1;
    moved = 0;
};

