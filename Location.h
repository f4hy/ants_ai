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


};


bool operator==(const Location &loc1, const Location &loc2) {
    return (loc1.row == loc2.row && loc1.col == loc2.col);
}

bool operator!=(const Location &loc1, const Location &loc2) {
    return (loc1.row != loc2.row && loc1.col != loc2.col);
}


#endif //LOCATION_H_
