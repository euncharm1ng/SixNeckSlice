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

    b.addC(&c);
    b.addC(&d);
    b.printC();
    b.printName();
    b.printBoard();

    b.setUCB(100);
    
    b.printUCB();
    b.t = 20;
    b.n = 1;
    m.calcUCB(&b);

    b.printUCB();

    b.printAvailMov();
    
}
