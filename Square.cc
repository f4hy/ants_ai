#include "Square.h"

Square::Square()
{
    isVisible = isWater = isHill = isFood = 0;
    ant = hillPlayer = -1;
        
};

void Square::reset()
{
    isVisible = 0;
    isHill = 0;
    isFood = 0;
    ant = hillPlayer = -1;
    deadAnts.clear();
};

