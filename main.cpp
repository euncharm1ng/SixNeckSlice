#include <stdlib.h>
#include <stdio.h>
#include "mcts.h"

using namespace std;

int main(){
    // printf("hello\n");
    Mcts m = Mcts();
    // a.rollout();
    Node b = Node(1, NULL);



    // Move mov1 = {1,1};
    // Move mov2 = {2,2};
    // Move start = {9,9};
    // m.placeStones(start, 1, &b);
    // b.board[9][10]=1;
    // b.board[9][11]=1;
    // b.board[9][12]=1;
    // b.board[9][13]=1;
    // b.board[9][14]=2;

    // b.board[5][0] = 1;
    // b.board[6][1] = 1;
    // b.board[7][2] = 1;
    // b.board[8][3] = 2;
    // b.board[9][4] = 1;
    // b.board[10][5] = 1;

    // b.board[3][7] = 2;
    // b.board[4][8] = 2;
    // b.board[5][9] = 2;
    // b.board[6][10] = 2;
    // b.board[7][11] = 2;
    // b.board[8][12] = 2;

    b.board[3][9] = 2;
    b.board[4][8] = 1;
    b.board[5][7] = 1;
    b.board[6][6] = 1;
    b.board[7][5] = 1;
    b.board[8][4] = 1;


    b.board[3][18] = 1;
    b.board[4][17] = 1;
    b.board[5][16] = 1;
    b.board[6][15] = 1;
    b.board[7][14] = 1;
    b.board[8][13] = 1;

    b.board[0][0]=1;
    b.board[0][18]=1;
    b.board[18][0]=1;
    b.board[18][18]=1;

    m.chkVic(b);
    printf("after chkvic\n");
    b.printBoard();
    m.findMoves(b);

}


