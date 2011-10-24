#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>


#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"

/*
  constants
*/
const int NUMDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;

const int PriFood = 20;
const int PriHill = -100;
const int PriBadHill = 50;
const int PriBadAnt = -1;
const int PriAnt = 2;

const int RadFood = 15;
const int RadHill = 1;
const int RadBadHill = 30;
const int RadBadAnt = 2;
const int RadAnt = 2;

// Added to squares already visited
const int PriStuck = -10;


// Added to priority of open squares next to water
const int PriNearWater = -2;

/*
  struct to store current state information
*/
struct State
{
    /*
      Variables
    */
    int rows, cols,
        turn, turns,
        noPlayers;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;

    std::vector<std::vector<Square> > grid;
    std::vector<Location> myAnts, enemyAnts, myHills, enemyHills, food;

    std::vector<Location> priorityUpdateThisRound;


    Timer timer;
    Bug bug;

    /*
      Functions
    */
    State();
    ~State();

    void setup();
    void reset();

    void setPriorities();
    void priorityradius(const int priority, const Location loc,const int radius );
    void setpriorityrow(const int priority, const Location loc,const int length );

    void checkForDeadEnds(const int row, const int col);

    void makeMove(const Location &loc, int direction);

    double distance(const Location &loc1, const Location &loc2);
    Location getLocation(const Location &startLoc, int direction);

    void updateVisionInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif //STATE_H_
