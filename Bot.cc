#include "Bot.h"

using namespace std;

//constructor
Bot::Bot()
{

};

//plays a single game of Ants.
void Bot::playGame()
{
    //reads the game parameters and sets up
    cin >> state;
    state.setup();
    endTurn();

    //continues making moves while the game is not over
    while(cin >> state)
    {
        state.updateVisionInformation();
        state.bug << "time taken after vision: " << state.timer.getTime() << "ms" << endl << endl;
        state.setPriorities();
        state.bug << "time taken after priorities: " << state.timer.getTime() << "ms" << endl << endl;
        state.foodPathing();
        state.bug << "time taken after foodpathing: " << state.timer.getTime() << "ms" << endl << endl;

        state.basicCombat();
        state.bug << "time taken after combat: " << state.timer.getTime() << "ms" << endl << endl;


        state.setDefenders();
        state.bug << "time taken after defnders: " << state.timer.getTime() << "ms" << endl << endl;
        makeMoves();
        state.bug << "time taken after makemoves: " << state.timer.getTime() << "ms" << endl << endl;
        endTurn();
    }
};

//makes the bots moves for the turn
void Bot::makeMoves()
{
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    state.bug << "gathers" << state.gatherer.size() << endl;
    state.bug << "myants" << state.myAnts.size() << endl;

    // gatherer movement
    foodPathingMove();

    // combat movement
    combatMove();

    // Defender movement
    defenderMove();

    //myAnts movement
    myAntMove();

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};


void Bot::combatMove(){

    state.bug << "combat movement" << endl;

    for(vector<Combat>::iterator itr = state.combats.begin();itr < state.combats.end(); itr++){

        Combat c = (*itr);

        if(state.grid[c.friendlyAnt.row][c.friendlyAnt.col].moved > 0){
            continue;
        }

        int moveToMake = -1;
        int dist = state.taxidistance(c.friendlyAnt,c.enemyAnt);
        for(int d=0; d<NUMDIRECTIONS; d++)
        {
            Location loc = state.getLocation(c.friendlyAnt, d);
            if(!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0)
            {
                int newdist = state.taxidistance(loc,c.enemyAnt);
                state.bug << "friends" << c.numfriendlyinrangeofbad << "bad" << c.numenemiesinrangeofgood << endl;
                if( c.numfriendlyinrangeofbad > c.numenemiesinrangeofgood){
                    state.bug << "ATTACK" << endl;
                    if(newdist < dist  ) {
                        moveToMake = d;
                        dist = newdist;
                    }
                }else if( c.numfriendlyinrangeofbad <= c.numenemiesinrangeofgood){
                    state.bug << "RUNAWAY" << endl;
                    if(newdist > dist  ) {
                        moveToMake = d;
                        dist = newdist;
                    }
                }else{
                    state.bug << "STAY" << endl;
                    if(newdist == dist) {
                        moveToMake = d;
                        dist = newdist;
                    }
                }
            }
        }
        if (moveToMake != -1){
            state.makeMove(c.friendlyAnt, moveToMake);
        }else{
            state.bug << "no good combat move founnd" << endl;
        }

    } // End loop over combats

}

void Bot::myAntMove(){
    vector<Location>::iterator it;

    for(it = state.myAnts.begin();it < state.myAnts.end(); it++){

        if(state.grid[it->row][it->col].moved > 0){
            continue;
        }


        int moveToMake = -1;
        for(int d=0; d<NUMDIRECTIONS; d++)
        {
            Location loc = state.getLocation((*it), d);
            if(!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0)
            {
                if (moveToMake == -1){
                    // state.bug << "was -1" << endl;
                    moveToMake = d;
                }
                else{
                    Location move = state.getLocation((*it), moveToMake);
                    if(state.grid[loc.row][loc.col].priority > state.grid[move.row][move.col].priority) {
                        // state.bug << "found better move" << endl;
                        moveToMake = d;
                    }
                }
            }
        }
        if (moveToMake != -1){
            state.makeMove((*it), moveToMake);
        }
    }

}

void Bot::defenderMove(){
    vector<Location>::iterator it;

    for(it = state.defenders.begin();it < state.defenders.end(); it++){
        int moveToMake = -1;
        for(int d=0; d<NUMDIRECTIONS; d++)
        {
            Location loc = state.getLocation((*it), d);
            if(!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0)
            {
                Location move = (*it);
                if (moveToMake != -1){
                    Location move = state.getLocation((*it), moveToMake);
                }
                if(state.grid[loc.row][loc.col].defensepriority > state.grid[move.row][move.col].defensepriority) {
                    // state.bug << "found better move" << endl;
                    moveToMake = d;
                }
            }
        }
        if (moveToMake != -1){
            state.makeMove((*it), moveToMake, false);
        }
    }
}

void Bot::foodPathingMove(){
    for(vector<Path>::iterator itr = state.gatherer.begin();itr < state.gatherer.end(); itr++){
        if(state.grid[itr->start.row][itr->start.col].moved > 0){
            continue;
        }
        Location loc = state.getLocation(itr->start, *(itr->steps.begin()));
        if(!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0){
            state.makeMove(itr->start, *(itr->steps.begin()), false);
            state.bug << "gatherer row " <<itr->start.row << " col " << itr->start.col << endl << endl;
        }
    }
}

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};
