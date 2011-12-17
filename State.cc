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

    std::vector<Location>::iterator it;
    for(it = enemyHills.begin();it < enemyHills.end(); it ++){
        if(grid[it->row][it->col].ant != -1){
            enemyHills.erase(it);
        }
    }

    myAnts.clear();
    enemyAnts.clear();
    myHills.clear();
    // enemyHills.clear();


    gatherer.clear();

    food.clear();
    defenders.clear();
    edge_of_view.clear();
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
void State::makeMove(const Location &loc, int direction, bool antistuck)
{
    cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;

    Location nLoc = getLocation(loc, direction);
    grid[nLoc.row][nLoc.col].ant = grid[loc.row][loc.col].ant;


    // bug << "set " << nLoc.row << "," <<nLoc.col <<"to value" <<grid[nLoc.row][nLoc.col].ant << endl;
    grid[loc.row][loc.col].ant = -1;
    if(antistuck){
        grid[loc.row][loc.col].permpriority += PriStuck;
    }
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

//returns the Manhattan distance between two locations with the edges wrapped
int State::taxidistance(const Location &loc1, const Location &loc2)
{
    int d1 = abs(loc1.row-loc2.row),
        d2 = abs(loc1.col-loc2.col),
        dr = min(d1, rows-d1),
        dc = min(d2, cols-d2);
    return dr + dc;
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

Location State::findClosestInBFS(const Location loc, vector<Location> haystack  ){

    queue<Location> locQ;

    updateIndex++;


    std::vector<Location>::iterator it = std::find(haystack.begin(),haystack.end(),loc);
    if(it != haystack.end()){
        bug << "find needle row " <<it->row << " col " << it->col << endl << endl;
        return *it;
    }

    locQ.push(loc);

    grid[loc.row][loc.col].updateIndex = updateIndex;




    while (!locQ.empty()){

        Location currentLoc = locQ.front();
        locQ.pop();

        for(int d=0; d<NUMDIRECTIONS; d++){
            Location nLoc = getLocation(currentLoc,d);
            if((grid[nLoc.row][nLoc.col].updateIndex != updateIndex) && !(grid[nLoc.row][nLoc.col].isWater)){


                it = std::find(haystack.begin(),haystack.end(),nLoc);

                if(it == haystack.end()){
                    grid[nLoc.row][nLoc.col].updateIndex = updateIndex;
                    locQ.push(nLoc);
                }
                else{
                    bug << "in find needle row " <<it->row << " col " << it->col << endl << endl;
                    return *it;

                }



            }
        }
    }
    bug <<"search failed!!"  << endl;

//    return loc;      // Did not find it, just return the end.
    return loc;      // Did not find it, just return where we started


}


std::vector<Location> State::findAllAnts(const Location loc,const bool friendly ,const int distance ){

    queue<Location> locQ;
    std::vector<Location> foundAnts;

    updateIndex++;

    locQ.push(loc);

    grid[loc.row][loc.col].updateIndex = updateIndex;

    while (!locQ.empty()){

        Location currentLoc = locQ.front();
        locQ.pop();

        for(int d=0; d<NUMDIRECTIONS; d++){
            Location nLoc = getLocation(currentLoc,d);

            if (taxidistance(nLoc,loc) > distance)
            {
                continue;
            }
            
            if((grid[nLoc.row][nLoc.col].updateIndex != updateIndex) && !(grid[nLoc.row][nLoc.col].isWater)){
                
                if(friendly && grid[nLoc.row][nLoc.col].ant == 0){
                    foundAnts.push_back(nLoc);
                }
                else if(grid[nLoc.row][nLoc.col].ant > 0){
                    foundAnts.push_back(nLoc);
                }
                grid[nLoc.row][nLoc.col].updateIndex = updateIndex;
                locQ.push(nLoc);
            }
        }
    }

    return foundAnts;


}



Path State::Dijkstra(const Location loc, vector<Location> haystack  ){

    priority_queue<PriLocation > locQ;

    updateIndex++;

    bug << "starting dijkstra" << endl;

    std::vector<Location>::iterator it = std::find(haystack.begin(),haystack.end(),loc);
    if(it != haystack.end()){
        bug << "find needle row " <<it->row << " col " << it->col << endl << endl;
        return Path(loc);
    }

    PriLocation start = PriLocation(loc,0);

    locQ.push(start);

    grid[loc.row][loc.col].updateIndex = updateIndex;


    int heuristic;

    while (!locQ.empty()){

        PriLocation currentLoc = locQ.top();

        // bug<< "now serveing: " << currentLoc.priority << endl;

        locQ.pop();

        for(int d=0; d<NUMDIRECTIONS; d++){
            Location nLoc = getLocation(currentLoc,d);
            if((grid[nLoc.row][nLoc.col].updateIndex != updateIndex) && !(grid[nLoc.row][nLoc.col].isWater)){


                it = std::find(haystack.begin(),haystack.end(),nLoc);

                if(it == haystack.end()){
                    grid[nLoc.row][nLoc.col].updateIndex = updateIndex;
                    grid[nLoc.row][nLoc.col].parent = REVERSE[d];

                    //int heuristic  = -taxidistance(nLoc,start)*(1+100*grid[nLoc.row][nLoc.col].isVisible);

                    if(grid[nLoc.row][nLoc.col].isVisible){
                        heuristic = -taxidistance(nLoc,start);
                    }
                    else{
                        heuristic = -taxidistance(nLoc,start) * 6;
                    }

                    locQ.push(PriLocation(nLoc,heuristic));
                }
                else{
                    bug << "in find needle row " <<it->row << " col " << it->col << endl << endl;

                    list<int> steps;

                    Location start = nLoc;
                    Location end = loc;

                    int direction;

                    // bug << "reverse?" << endl;

                    grid[nLoc.row][nLoc.col].parent = REVERSE[d];
                    // bug << "reverse!" << REVERSE[d] << endl;
                    while(nLoc != loc){
                        direction = grid[nLoc.row][nLoc.col].parent;
                        // bug << "parent" << direction << endl;
                        steps.push_back(direction);
                        if (direction < 0){
                            bug << "throwing exceptin because direction fail" << endl;
                            throw;

                        }

                        
                        nLoc = getLocation(nLoc,direction);
                        // bug << "nloc " <<nLoc.row << " col " << nLoc.col << endl;
                        // bug << "loc " <<loc.row << " col " << loc.col << endl << endl;
                    }

                    return Path(start,end,steps);

                }



            }
        }
    }

    return Path(loc);      // Did not find it, just return where we started


} // End Dijkstra



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

    grid[loc.row][loc.col].priority += sign*priority*priority;
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

void State::priorityDefense(const Location loc){

    queue<Location> locQ;
    queue<int> depthQ;


    int depth = 1;
    int currentpriority = 1;

    int defensepriority = defenseDistance +1;

    updateIndex++;


    locQ.push(loc);
    depthQ.push(depth);

    grid[loc.row][loc.col].defensepriority = -100000;
    grid[loc.row][loc.col].updateIndex = updateIndex;

    if(defenseDistance<1){return;}

    while (!locQ.empty()){

        if(depth > defenseDistance){
            bug << "Reached max depth " << depth  << endl;
            break;
        }
        Location currentLoc = locQ.front();
        depth = depthQ.front();
        locQ.pop();
        depthQ.pop();

        for(int d=0; d<NUMDIRECTIONS; d++){
            Location nLoc = getLocation(currentLoc,d);
            if((grid[nLoc.row][nLoc.col].updateIndex != updateIndex) && !(grid[nLoc.row][nLoc.col].isWater)){
                grid[nLoc.row][nLoc.col].updateIndex = updateIndex;
                int x= defensepriority - (max(1,(defensepriority*depth)/defenseDistance)); //Fast sqr


                grid[nLoc.row][nLoc.col].defensepriority = (x);
                locQ.push(nLoc);
                depthQ.push(depth+1);

            }
        }
    }

    // corners better than edges
    for(int d=0; d<NUMDIRECTIONS; d++){
        Location edge = getLocation(loc,d);
        grid[edge.row][edge.col].defensepriority -= 1;
    }
}

void State::foodPathing(){

    Path testpath;

    vector<Location>::iterator it;

    for(vector<Location>::iterator fooditr = food.begin();fooditr < food.end(); fooditr ++){
        bug << "Testing path to food" << endl;
        // for(it = food.begin();it < food.end(); it ++){
        //     testpath = Dijkstra(*it,myAnts);
        // }

        bug << "Dijkstra about to start?" << endl; 
        if(myAnts.size() < 1){
            break;
        }
        testpath = Dijkstra(*fooditr,myAnts);
        bug << "Dijkstra passed " << endl; 


        bug << "path done" << endl;
        bug << "start" << testpath.start.row << " " << testpath.start.col << endl;
        bug << "end" << testpath.end.row << " " << testpath.end.col << endl;
        // for(    list<int>::iterator itr = testpath.steps.begin();itr < testpath.steps.end(); itr ++){
        //     bug << "steps " << CDIRECTIONS[*itr]  << endl;
        // }

        // grid[testpath.start.row][testpath.start.col].priority = 1;
        // Location nLoc = testpath.start;
        // for(    list<int>::iterator itr = testpath.steps.begin();itr < testpath.steps.end(); itr ++){
        //     bug << "steps " << CDIRECTIONS[*itr]  << endl;
        //     nLoc = getLocation(nLoc,*itr);
        //     grid[nLoc.row][nLoc.col].priority = 2;

        // }
        gatherer.push_back(testpath); // Add to gatherer
        vector<Location>::iterator foundAnt = find(myAnts.begin(),myAnts.end(),testpath.start);
        bug << "trying to erase";
        myAnts.erase(foundAnt); // no longer a regular ant.
        bug << "erased" << endl;
    }
}

void State::setPriorities(){
    bug << "setting priority" <<endl;

    vector<Location>::iterator it;
    vector<Location>::iterator ant;
    // for(it = food.begin();it < food.end(); it ++){
    //     priorityradiusBFS(PriFood,*it, RadFood);
    // }
    for(it = myHills.begin();it < myHills.end(); it++){
        priorityradiusBFS(PriHill,*it, RadHill);
        priorityDefense(*it);
    }

    for(it = edge_of_view.begin();it < edge_of_view.end(); it ++){
        priorityradiusBFS(viewradius*4,*it, viewradius*8);
    }


    // priorityradiusBFS(4,*myHills.begin(), 4);
    // priorityradiusBFS(2,*food.begin(), 2);

    int count = 1;
    for(it = enemyHills.begin();it < enemyHills.end(); it ++){
        priorityradiusBFS(PriBadHill/count,*it, RadBadHill/count);
        priorityradiusBFS(1000,*it, 3);
        count+= 10;


        for(ant = myAnts.begin();ant < myAnts.end(); ant ++){
            int d = (int)(distance(*it,*ant));
            if(d>3 && d<RadBadHill/count){
                priorityradiusBFS(PriAntAttack/d,*it, RadAntAttack);
            }
        }
        for(ant = enemyAnts.begin();ant < enemyAnts.end(); ant ++){
            int d = (int)(distance(*it,*ant));
            if(d>0 && d<RadBadHill/count){
                priorityradius(PriBadAntAttack/d,*it, RadBadAntAttack);
            }
        }
    }

    if (enemyHills.size() == 0){ // Nothing to attack
        for(it = myAnts.begin();it < myAnts.end(); it ++){
            priorityradiusBFS(PriAnt,*it, RadAnt);
        }

        for(it = enemyAnts.begin();it < enemyAnts.end(); it ++){
            priorityradius(PriBadAnt,*it, RadBadAnt);
        }
    }



}

void State::setDefenders(){
    bug << "setting defenders" <<endl;

    if(myHills.size() < 1){return;}

    numberOfDefenders = myAnts.size() / (defenderThreshhold*myHills.size());

    if(numberOfDefenders > (maxDefenders*myHills.size()) ){
        numberOfDefenders = maxDefenders*myHills.size();
    }


    if(numberOfDefenders < 1){
        bug << "not enough to defend" << numberOfDefenders <<endl;
        return;
    } // Not enough to start defense

    vector<Location>::iterator it;

    // vector<Location>::iterator needle;



    bug << "myAnt before size" << myAnts.size() <<endl;


    for(int i=0; i < numberOfDefenders; ){
        for(it = myHills.begin();it < myHills.end(); it++){

            if(grid[it->row][it->col].ant != 0){

                bug << "num def's : i" << numberOfDefenders << "  " << i <<endl ;

                Location needle = findClosestInBFS(*it,myAnts);
                bug << "needle row " <<needle.row << " col " << needle.col << endl << endl;

                vector<Location>::iterator foundAnt = find(myAnts.begin(),myAnts.end(),needle);

                if(grid[foundAnt->row][foundAnt->col].ant != 0){
                    bug << "error!! Tried to add no existant defender";
                    return;
                }

                if(distance(*foundAnt,*it) > defenseDistance){
                    bug << "Closest defender too far, wait for new defender" << endl;
                    return;
                }


                defenders.push_back(*foundAnt); // Add to defenders

                myAnts.erase(foundAnt); // no longer a regular ant.

            }

            i++;
            if(i>=numberOfDefenders){break;}

        }
    }


    // check
    bug<< "myant after size" << myAnts.size() <<endl;
    bug<< "defender after size" << defenders.size() <<endl <<endl;

    for(it = myAnts.begin();it < myAnts.end(); it++){
        bug << "Ant row " <<it->row << " col " << it->col << endl;
    }


    for(it = myHills.begin();it < myHills.end(); it++){
        bug << "Hill row " <<it->row << " col " << it->col << endl << endl;
    }


    for(it = defenders.begin();it < defenders.end(); it++){
        bug << "Defender row " <<it->row << " col " << it->col << endl << endl;
    }

}


void State::basicCombat(){
    bug << "BasicCombat" <<endl << endl;

    vector<Location>::iterator it;

    // vector<Location>::iterator needle;

    for(it = enemyAnts.begin();it < enemyAnts.end(); it++){

        bug << "evaluating enemy " ;
        bug << "enemy row " <<it->row << " col " << it->col << endl << endl;
        
        vector<Location> friendlys = findAllAnts(*it,true,attackradius);
        bug << "found friendlies " <<endl ;

        int numfriendinrange = friendlys.size();

        bug << "counted " << numfriendinrange <<endl ;

        if(numfriendinrange < 1){
            continue;
        }

        vector<Location>::iterator goodit;
        for(goodit = friendlys.begin();goodit < friendlys.end(); goodit++){
            bug << "erasing found ants for combat " << goodit->row << "  " << goodit->col <<endl ;
            vector<Location>::iterator foundAnt = find(myAnts.begin(),myAnts.end(),*goodit);
            myAnts.erase(foundAnt);
            bug << "counting bads " <<endl ;
            int numbadinrange = findAllAnts(*goodit,false,attackradius).size();
            bug << "storing combat " <<endl ;
            combats.push_back(Combat(*it,*goodit,numfriendinrange,numbadinrange));          
                
        }
    }
}


void State::checkForDeadEnds(const int row, const int col){

    Location water = Location(row,col);
    for(int d=0; d<NUMDIRECTIONS; d++){

        Location candidate = getLocation(water,d);
        if(grid[candidate.row][candidate.col].isWater
           || grid[candidate.row][candidate.col].isFood
           || grid[candidate.row][candidate.col].hillPlayer > -1)
        {
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
                os << (char)('0' + state.grid[row][col].priority);
            }
            else{
                os << '~';
            }
        }
        // for(int col=0; col<state.cols; col++)
        // {
        //     if(state.grid[row][col].parent > -1){
        //         os << CDIRECTIONS[(state.grid[row][col].parent)];
        //     }
        //     else{
        //         os << '.';
        //     }
        // }
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
                state.attackradius = state.attackradius;
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
