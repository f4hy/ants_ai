#include "Square.h"
#include "Location.h"
#include <iostream>
using namespace std;


bool Locationtest(void) {

    Location l1 = Location(0,0);
    Location l2 = Location(1,1);

    Location l3 = Location(1,1);

    if(l1 == l2){
        return false;
    }

    if(l1 == l3){
        return false;
    }

    if(l2 != l3){
        return false;
    }

    
    if(l2 == l3){
        return true;
    }
    else{
        return false;
    }
}


int main(int argc, char *argv[])
{
    if(!Locationtest() ){
        cout << "Locationtest failed" << endl;
        return 1;
    }
    else{
        cout << "Locationtest passed" << endl;
    }

    cout << "tests passed" << endl;
    
    
    return 0;
}


