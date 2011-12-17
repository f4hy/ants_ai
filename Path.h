#ifndef PATH_H_
#define PATH_H_

#include <vector>


/*
  struct for representing paths in the grid.
*/
struct Path
{

    Location start;
    Location end;

    std::vector<int> steps;

    Path(){
    }
    
    Path(Location st ){
        start = st;
        end = st;
        steps.clear();
            
    }
    
    Path(Location st, Location e,std::vector<int> sp ){
        start = st;
        end = e;
        steps = sp;
    };

};

#endif //PATH_H_
