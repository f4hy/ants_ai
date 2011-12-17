#ifndef PRILOCATION_H_
#define PRILOCATION_H_

/*
  struct for representing locations in the grid.
*/
struct PriLocation : Location
{
    int priority;

    PriLocation(){
        priority = 0;
    };

    PriLocation(Location l ,int p){
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


#endif //PRILOCATION_H_
