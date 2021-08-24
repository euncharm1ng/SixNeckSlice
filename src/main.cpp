#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "mcts.h"


using namespace std;

int main(){
    runGUI();


    // short **board = (short**)malloc(sizeof(short*)*BOARDSIZE);
    // for (int i =0; i< BOARDSIZE; i++){
    //     board[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
    //     for(int j = 0; j < BOARDSIZE; j++) board[i][j] = 0;
    // }
    // board[10][10] = 1;

    // Mcts m = Mcts(board, WHITE);

    // m.root->movesLog = (pMove)malloc(sizeof(Move)*8);
    // m.root->moveSize = 8;

    // m.root->movesLog[0] = {7, 9};
    // m.root->movesLog[1] = {8, 8};

    // m.root->movesLog[2] = {10, 9};
    // m.root->movesLog[3] = {10, 8};

    // m.root->movesLog[4] = {9, 8};
    // m.root->movesLog[5] = {9, 7};

    // m.root->movesLog[6] = {10, 6};
    // m.root->movesLog[7] = {11, 9};    

    // pNode result = m.runGame();

    // return 0;
}




