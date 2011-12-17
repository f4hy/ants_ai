#ifndef COMBAT_H_
#define COMBAT_H_

#include <vector>
#include <list>


/*
  struct for representing combats.
*/
struct Combat
{
    Location enemyAnt;
    Location friendlyAnt;
    int numfriendlyinrangeofbad;
    int numenemiesinrangeofgood;
    

    Combat(){
    }
    
    Combat(Location e,Location f,int nfirob, int neirog ){
        enemyAnt = e;
        friendlyAnt = f;
        numfriendlyinrangeofbad = nfirob;
        numenemiesinrangeofgood = neirog;
    };

    
};

#endif //COMBAT_H_
