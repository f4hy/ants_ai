#ifndef PATH_H_
#define PATH_H_

#include <vector>
#include <list>


/*
  struct for representing paths in the grid.
*/
struct Path
{

    Location start;
    Location end;

    std::list<int> testlist;
    
    std::list<int> steps;

    Path(){
    }
    
    Path(Location st ){
        start = st;
        end = st;
        steps.clear();
            
    }
    
    Path(Location st, Location e,std::list<int> sp ){
        start = st;
        end = e;
        steps = sp;
    };

    void reverse(void){
        Location tmp;
        tmp = start;
        start = end;
        end = tmp;
        steps.reverse();
    };
    
};

#endif //PATH_H_
