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
#include <list>


#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"
#include "Path.h"
#include "Combat.h"

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

const int REVERSE[4] = {2,3,0,1}; // Gives the reverse of the int direction


const int PriFood = 50;
const int PriHill = -100;
const int PriBadHill = 100;
const int PriBadAnt = -25;
const int PriAnt = -15;
const int PriBadAntAttack = 25;
const int PriAntAttack = 40;

const int RadFood = 12;
const int RadHill = 0;
const int RadBadHill = 100;
const int RadBadAnt = 3;
const int RadAnt = 5;
const int RadBadAntAttack = 2;
const int RadAntAttack = 6;

// Added to squares already visited
const int PriStuck = -10;

const int defenderThreshhold = 9;

const int defenseDistance = 10;
const int maxDefenders = 6;


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
    double spawnradius, viewradius;
    int attackradius;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;

    int updateIndex;

    std::vector<std::vector<Square> > grid;
    std::vector<Location>  enemyAnts, myHills, enemyHills, food;

    std::vector<Location> myAnts;

    std::vector<Location> priorityUpdateThisRound;

    int numberOfDefenders;
    std::vector<Location> defenders;

    std::vector<Path> gatherer;
    std::vector<Path> explorers;


    std::vector<Location> edge_of_view;

    std::vector<Combat> combats;

    Timer timer;
    Bug bug;

    /*
      Functions
    */
    State();
    ~State();

    void setup();
    void reset();


    void foodPathing();

    void pathIntegrityCheck();
    void explorePathing();

    void explorepathIntegrityCheck();

    void setPriorities();
    void setDefenders();

    void basicCombat();

    // void priorityradius(const int priority, const Location loc,const int radius );
    // void setpriorityrow(const int priority, const Location loc,const int length );
    void priorityradiusBFS(const int priority, const Location loc,const int radius );

//    Location chooseAntBFS(const Location loc, const int targetType );
//    std::vector<Location>::iterator findClosestInBFS(const Location loc, std::vector<Location> haystack  );

    Location findClosestInBFS(const Location loc, std::vector<Location> haystack  );

    std::vector<Location> findAllAnts(const Location loc,const bool friendly ,const int distance);

    Path Dijkstra(const Location loc, std::vector<Location> haystack  );
    Path Dijkstraexplore(const Location loc );

    void priorityDefense(const Location loc);


    void checkForDeadEnds(const int row, const int col);

    void makeMove(const Location &loc, int direction, bool antistuck= true);

    double distance(const Location &loc1, const Location &loc2);

    int taxidistance(const Location &loc1, const Location &loc2);


    Location getLocation(const Location &startLoc, int direction);

    void updateVisionInformation();
};

std::ostream& operator<<(std::ostream &os, const State &state);
std::istream& operator>>(std::istream &is, State &state);

#endif //STATE_H_
