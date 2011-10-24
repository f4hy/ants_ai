#include "Square.h"

Square::Square()
{
    isVisible = isWater = isHill = isFood = 0;
    priority = 0;
    ant = hillPlayer = -1;
    permpriority = 0;
};

void Square::reset()
{
    isVisible = 0;
    isHill = 0;
    isFood = 0;
    priority = permpriority;

    ant = hillPlayer = -1;
    deadAnts.clear();
};

