#ifndef LOCATION_H_
#define LOCATION_H_

/*
  struct for representing locations in the grid.
*/
struct Location
{
    int row, col;

    Location(){
        row = col = 0;
    };

    Location(int r, int c){
        row = r;
        col = c;
    };

    bool operator==(const Location &other) const {
        return (row == other.row && col == other.col);
    }

    bool operator!=(const Location &other) const {
        return (row != other.row || col != other.col);
    }

};

struct PriLocation : Location
{
    int priority;

    PriLocation(){
        priority = 0;
    };

    PriLocation(int r, int c, int p){
        row = r;
        col = c;
        priority = p;
    };

    PriLocation(Location l, int p){
        row = l.row;
        col = l.col;
        priority = p;
    };

    


    bool operator>(const PriLocation &other) const {
        return (priority  > other.priority);
    }

    bool operator<(const PriLocation &other) const {
        return (priority < other.priority);
    }


};


// bool operator== ( const Nick &n1, const Nick &n2) 
// {
//         return n1.username == n2.username;
// }

// bool operator==(const Location &loc1, const Location &loc2) const {
//     return (loc1.row == loc2.row && loc1.col == loc2.col);
// }

// bool operator!=(const Location &loc1, const Location &loc2) const {
//     return (loc1.row != loc2.row && loc1.col != loc2.col);
// }


#endif //LOCATION_H_
