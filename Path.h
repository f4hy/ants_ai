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

    int distance;

    Path(){
    }

    Path(Location st ){
        start = st;
        end = st;
        steps.clear();
        distance = 0;
    }

    Path(Location st, Location e,std::list<int> sp ,int d){
        start = st;
        end = e;
        steps = sp;
        distance = d;
    };

    void reverse(void){
        Location tmp;
        tmp = start;
        start = end;
        end = tmp;
        steps.reverse();
    };

    void move(Location nloc){
        if (steps.size() > 0){
            steps.pop_front();
        }
        start = nloc;
        distance--;
    }

};

#endif //PATH_H_
