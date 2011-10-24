#include "State.h"

using namespace std;

//constructor
State::State()
{
    gameover = 0;
    turn = 0;
    updateIndex=0;

    bug.open("./debug.txt");
};

//deconstructor
State::~State()
{
    bug.close();
};

//sets the state up
void State::setup()
{
    grid = vector<vector<Square> >(rows, vector<Square>(cols, Square()));
};

//resets all non-water squares to land and clears the bots ant vector
void State::reset()
{
    myAnts.clear();
    enemyAnts.clear();
    myHills.clear();
    enemyHills.clear();
    food.clear();
    updateIndex=0;

    for(int row=0; row<rows; row++)
        for(int col=0; col<cols; col++)
            if(!grid[row][col].isWater)
                grid[row][col].reset();
            else{
                checkForDeadEnds(row,col);
            }
};

//outputs move information to the engine
void State::makeMove(const Location &loc, int direction)
{
    cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;

    Location nLoc = getLocation(loc, direction);
    grid[nLoc.row][nLoc.col].ant = grid[loc.row][loc.col].ant;
    bug << "set " << nLoc.row << "," <<nLoc.col <<"to value" <<grid[nLoc.row][nLoc.col].ant << endl;
    grid[loc.row][loc.col].ant = -1;
    grid[loc.row][loc.col].permpriority += PriStuck;
};



//returns the euclidean distance between two locations with the edges wrapped
double State::distance(const Location &loc1, const Location &loc2)
{
    int d1 = abs(loc1.row-loc2.row),
        d2 = abs(loc1.col-loc2.col),
        dr = min(d1, rows-d1),
        dc = min(d2, cols-d2);
    return sqrt(dr*dr + dc*dc);
};

void State::setpriorityrow(const int priority, const Location loc,const int length ){

    grid[loc.row][loc.col].priority = priority;

    Location eLoc = getLocation(loc,EAST);
    Location wLoc = getLocation(loc,WEST);

    for(int i = 1; i < length; i++){
        grid[eLoc.row][eLoc.col].priority += priority-i;
        grid[wLoc.row][wLoc.col].priority += priority-i;
        eLoc = getLocation(eLoc,EAST);
        wLoc = getLocation(wLoc,WEST);
    }
    grid[eLoc.row][eLoc.col].priority += priority-length;
    grid[wLoc.row][wLoc.col].priority += priority-length;

}

void State::priorityradius(const int priority, const Location loc,const int radius ){


    setpriorityrow(priority, loc,radius);
    Location nLoc, sLoc;
    nLoc = getLocation(loc,NORTH);
    sLoc = getLocation(loc,SOUTH);
    for(int i = 1; i < radius; i++){
        setpriorityrow(priority-i, nLoc,radius-i);
        setpriorityrow(priority-i, sLoc,radius-i);
        nLoc = getLocation(nLoc,NORTH);
        sLoc = getLocation(sLoc,SOUTH);
    }
    grid[nLoc.row][nLoc.col].priority += priority-radius;
    grid[sLoc.row][sLoc.col].priority += priority-radius;


    return;

}

// void State::chooseAntBFS(const Location loc, const int targetType ){

//     queue<Location> locQ;

//     updateindex++;



//     locQ.push(loc);


//     while (!locQ.empty()){

//         Location currentLoc = locQ.front();
//         locQ.pop();

//         for(int d=0; d<NUMDIRECTIONS; d++){
//             Location nLoc = getLocation(currentLoc,d);
//             if(!(grid[nLoc.row][nLoc.col].isWater)){
//                 grid[nLoc.row][nLoc.col].priority = priority - ((priority*depth)/radius);
//                 locQ.push(nLoc);

//             }
//         }
//     }
//     bug << "locQ was empty! " << depth  << endl;


// }



void State::priorityradiusBFS(const int priority, const Location loc,const int radius ){

    queue<Location> locQ;
    queue<int> depthQ;


    int depth = 1;
    int currentpriority = 1;
    updateIndex++;

    int sign;
    if (priority > 0){
        sign = 1;
    }
    else{
        sign = -1;
        
    }
        

    locQ.push(loc);
    depthQ.push(depth);

    grid[loc.row][loc.col].priority += priority;
    grid[loc.row][loc.col].updateIndex = updateIndex;

    if(radius<1){return;}

    while (!locQ.empty()){

        if(depth > radius){
            bug << "Reached max depth " << depth  << endl;
            return;
        }
        Location currentLoc = locQ.front();
        depth = depthQ.front();
        locQ.pop();
        depthQ.pop();

        for(int d=0; d<NUMDIRECTIONS; d++){
            Location nLoc = getLocation(currentLoc,d);
            if((grid[nLoc.row][nLoc.col].updateIndex != updateIndex) && !(grid[nLoc.row][nLoc.col].isWater)){
                grid[nLoc.row][nLoc.col].updateIndex = updateIndex;
                int x= priority - (max(1,(priority*depth)/radius)); //Fast sqr
                grid[nLoc.row][nLoc.col].priority += sign* (x*x);
                locQ.push(nLoc);
                depthQ.push(depth+1);

            }
        }
    }
    bug << "locQ was empty! " << depth  << endl;


}


void State::setPriorities(){
    bug << "setting priority" <<endl;

    vector<Location>::iterator it;
    for(it = food.begin();it < food.end(); it ++){
        priorityradiusBFS(PriFood,*it, RadFood);
    }
    for(it = myHills.begin();it < myHills.end(); it++){
        priorityradiusBFS(PriHill,*it, RadHill);
    }


    for(it = enemyHills.begin();it < enemyHills.end(); it ++){
        priorityradiusBFS(PriBadHill,*it, RadBadHill);

    }


    // priorityradiusBFS(4,*myHills.begin(), 4);
    // priorityradiusBFS(2,*food.begin(), 2);

    // for(it = myAnts.begin();it < myAnts.end(); it ++){
    //     priorityradiusBFS(PriAnt,*it, RadAnt);
    // }
    // for(it = enemyAnts.begin();it < enemyAnts.end(); it ++){
    //     priorityradius(PriBadAnt,*it, RadBadAnt);
    // }


}


void State::checkForDeadEnds(const int row, const int col){

    Location water = Location(row,col);
    for(int d=0; d<NUMDIRECTIONS; d++){

        Location candidate = getLocation(water,d);
        if(grid[candidate.row][candidate.col].isWater){
            continue;
        }
        else{
            int count = 0;
            for(int dir=0; dir<NUMDIRECTIONS; dir++){
                Location nloc = getLocation(candidate,dir);
                if(grid[nloc.row][nloc.col].isWater){
                    count++;
                    grid[candidate.row][candidate.col].priority += PriNearWater;
                }
            }
            if (count>2){
                grid[candidate.row][candidate.col].isWater = 1;
            }
        }


    }
}





//returns the new location from moving in a given direction with the edges wrapped
Location State::getLocation(const Location &loc, int direction)
{
    return Location( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
                     (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};

/*
  This function will update update the lastSeen value for any squares currently
  visible by one of your live ants.

  BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
  THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
  A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
  IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
*/
void State::updateVisionInformation()
{
    std::queue<Location> locQueue;
    Location sLoc, cLoc, nLoc;

    for(int a=0; a<(int) myAnts.size(); a++)
    {
        sLoc = myAnts[a];
        locQueue.push(sLoc);

        std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, 0));
        grid[sLoc.row][sLoc.col].isVisible = 1;
        visited[sLoc.row][sLoc.col] = 1;

        while(!locQueue.empty())
        {
            cLoc = locQueue.front();
            locQueue.pop();

            for(int d=0; d<NUMDIRECTIONS; d++)
            {
                nLoc = getLocation(cLoc, d);

                if(!visited[nLoc.row][nLoc.col] && distance(sLoc, nLoc) <= viewradius)
                {
                    grid[nLoc.row][nLoc.col].isVisible = 1;
                    locQueue.push(nLoc);
                }
                visited[nLoc.row][nLoc.col] = 1;
            }
        }
    }
};

/*
  This is the output function for a state. It will add a char map
  representation of the state to the output stream passed to it.

  For example, you might call "cout << state << endl;"
*/
ostream& operator<<(ostream &os, const State &state)
{
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
        {
            if(state.grid[row][col].isWater)
                os << '%';
            else if(state.grid[row][col].isFood)
                os << '*';
            else if(state.grid[row][col].isHill)
                os << (char)('A' + state.grid[row][col].hillPlayer);
            else if(state.grid[row][col].ant >= 0)
                os << (char)('a' + state.grid[row][col].ant);
            else if(state.grid[row][col].isVisible)
                os << '.';
            else
                os << '?';
        }
        os << ' ';
        os << '|';
        os << ' ';
        for(int col=0; col<state.cols; col++)
        {
            if(state.grid[row][col].priority > -1){
                os << char(state.grid[row][col].priority+48);
            }
            else{
                os << '~';
            }
        }
        os << endl;
    }

    return os;
};

//input function
istream& operator>>(istream &is, State &state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            state.gameover = 1;
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.turn;
            break;
        }
        else //unknown line
            getline(is, junk);
    }

    if(state.turn == 0)
    {
        //reads game parameters
        while(is >> inputType)
        {
            if(inputType == "loadtime")
                is >> state.loadtime;
            else if(inputType == "turntime")
                is >> state.turntime;
            else if(inputType == "rows")
                is >> state.rows;
            else if(inputType == "cols")
                is >> state.cols;
            else if(inputType == "turns")
                is >> state.turns;
            else if(inputType == "viewradius2")
            {
                is >> state.viewradius;
                state.viewradius = sqrt(state.viewradius);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.attackradius;
                state.attackradius = sqrt(state.attackradius);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.spawnradius;
                state.spawnradius = sqrt(state.spawnradius);
            }
            else if(inputType == "ready") //end of parameter input
            {
                state.timer.start();
                break;
            }
            else    //unknown line
                getline(is, junk);
        }
    }
    else
    {
        //reads information about the current turn
        while(is >> inputType)
        {
            if(inputType == "w") //water square
            {
                is >> row >> col;
                state.grid[row][col].isWater = 1;
                state.grid[row][col].priority = -1000;
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
                state.grid[row][col].isFood = 1;
                state.food.push_back(Location(row, col));
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
                state.grid[row][col].ant = player;
                if(player == 0)
                    state.myAnts.push_back(Location(row, col));
                else
                    state.enemyAnts.push_back(Location(row, col));
            }
            else if(inputType == "d") //dead ant square
            {
                is >> row >> col >> player;
                state.grid[row][col].deadAnts.push_back(player);
            }
            else if(inputType == "h")
            {
                is >> row >> col >> player;
                state.grid[row][col].isHill = 1;
                state.grid[row][col].hillPlayer = player;
                if(player == 0)
                    state.myHills.push_back(Location(row, col));
                else
                    state.enemyHills.push_back(Location(row, col));

            }
            else if(inputType == "players") //player information
                is >> state.noPlayers;
            else if(inputType == "scores") //score information
            {
                state.scores = vector<double>(state.noPlayers, 0.0);
                for(int p=0; p<state.noPlayers; p++)
                    is >> state.scores[p];
            }
            else if(inputType == "go") //end of turn input
            {
                if(state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }
    }

    return is;
};
