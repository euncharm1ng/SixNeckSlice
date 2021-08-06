#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "mcts.h"
#define WHITE 2

#define INFI 3e10
using namespace std;

int main(){

    short **board = (short**)malloc(sizeof(short*)*BOARDSIZE);
    for (int i =0; i< BOARDSIZE; i++){
        board[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        for(int j = 0; j < BOARDSIZE; j++) board[i][j] = 0;
    }
    board[10][10] = 1;
    Mcts m = Mcts(board, WHITE);

    printf("before select\n");
    m.select();
    printf("after select\n");

    // Move start2 = {9,10};
    // Move start3 = {8,10};
    // Move start4 = {6,10};
    // Move start5 = {9,11};
    // m.placeStones(start2, 1, &b);
    // m.placeStones(start3, 1, &b);
    // m.placeStones(start4, 1, &b);
    // m.placeStones(start5, 1, &b);

    // Move start6 = {9,7};
    // Move start7 = {8,8};
    // Move start8 = {8,9};
    // Move start9 = {7,9};
    // m.placeStones(start6, 2, &b);
    // m.placeStones(start7, 2, &b);
    // m.placeStones(start8, 2, &b);
    // m.placeStones(start9, 2, &b);
}



