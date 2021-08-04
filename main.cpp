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
    b.board[10][10] = 1;
    b.board[12][12] = 2;
    b.printBoard();
    printf("main\n");
    m.findMoves(b);

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
