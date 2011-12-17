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
    
    for(vector<Path>::iterator itr = state.gatherer.begin();itr < state.gatherer.end(); itr++){
        state.makeMove(itr->start, *(itr->steps.begin()), false);
        state.bug << "gatherer row " <<itr->start.row << " col " << itr->start.col << endl << endl;


    }

    for(int ant=0; ant<(int)state.defenders.size(); ant++){
        int moveToMake = -1;
        for(int d=0; d<NUMDIRECTIONS; d++)
        {
            Location loc = state.getLocation(state.defenders[ant], d);
            if(!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0)
            {
                Location move = state.defenders[ant];
                if (moveToMake != -1){
                    Location move = state.getLocation(state.defenders[ant], moveToMake);
                }
                if(state.grid[loc.row][loc.col].defensepriority > state.grid[move.row][move.col].defensepriority) {
                    // state.bug << "found better move" << endl;
                    moveToMake = d;
                }
            }
        }
        if (moveToMake != -1){
            state.makeMove(state.defenders[ant], moveToMake, false);
        }
    }


//picks out moves for each ant
    for(int ant=0; ant<(int)state.myAnts.size(); ant++)
    {
        int moveToMake = -1;
        for(int d=0; d<NUMDIRECTIONS; d++)
        {
            Location loc = state.getLocation(state.myAnts[ant], d);
            if(!state.grid[loc.row][loc.col].isWater && state.grid[loc.row][loc.col].ant != 0)
            {
                if (moveToMake == -1){
                    // state.bug << "was -1" << endl;
                    moveToMake = d;
                }
                else{
                    Location move = state.getLocation(state.myAnts[ant], moveToMake);
                    if(state.grid[loc.row][loc.col].priority > state.grid[move.row][move.col].priority) {
                        // state.bug << "found better move" << endl;
                        moveToMake = d;
                    }
                }
            }
        }
        if (moveToMake != -1){
            state.makeMove(state.myAnts[ant], moveToMake);
        }
    }
    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl << endl;
};

//finishes the turn
void Bot::endTurn()
{
    if(state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
};
