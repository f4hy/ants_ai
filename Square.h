#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

/*
  struct for representing a square in the grid.
*/
struct Square
{
    bool isVisible, isWater, isHill, isFood;
    bool isPassible;
    int ant, hillPlayer;
    int priority;
    int permpriority;
    int defensepriority;
    int updateIndex;

    int moved;
    int gatherer;

    std::vector<int> deadAnts;

    int parent;

    // Initializes values.
    Square();

    //resets the information for the square except water information
    void reset();

};

#endif //SQUARE_H_
