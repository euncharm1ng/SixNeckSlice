#include <stdlib.h>
#include <stdio.h>
#include "mcts.h"

using namespace std;

int main(){
    // printf("hello\n");
    Mcts m = Mcts();
    // a.rollout();
    Node b = Node("tom");
    Node c = Node("jerry");
    Node d = Node("frank");

    Move mov1 = {1,1};
    Move mov2 = {2,2};
    Move start = {9,9};
    m.placeStones(start, 1, &b);
    b.printBoard();

    // b.addC(&c);
    // b.addC(&d);
    // b.printC();
    // b.printName();
    // b.printBoard();

    // b.setUCB(100);
    
    // b.printUCB();
    // b.t = 20;
    // b.n = 1;
    // m.calcUCB(&b);

    // b.printUCB();

    // b.printAvailMov();
    
}
