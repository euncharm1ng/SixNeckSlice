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
    // board[9][10] = 1;
    // board[8][10] = 1;
    // board[6][10] = 1;
    // board[9][11] = 1;

    // board[9][7] = 2;
    // board[8][8] = 2;
    // board[8][9] = 2;
    // board[7][9] = 2;
    Mcts m = Mcts(board, WHITE);

    m.root->movesLog = (pMove)malloc(sizeof(Move)*8);
    m.root->moveSize = 8;

    m.root->movesLog[0] = {7, 9};
    m.root->movesLog[1] = {8, 8};

    m.root->movesLog[2] = {10, 9};
    m.root->movesLog[3] = {10, 8};

    m.root->movesLog[4] = {9, 8};
    m.root->movesLog[5] = {9, 7};

    m.root->movesLog[6] = {10, 6};
    m.root->movesLog[7] = {11, 9};

    
    

    printf("before select\n");
    //m.select();
    // m.findMoves(m.root);
    //int chk = m.rollout(m.root);
    m.expansion(m.root); // TODO:
    printf("%d\n", chk);
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
/*
    Mcts m = Mcts(1);
    Node b = Node(1);

    Move start1 = {8,8};
    Move start2 = {10,8};
    Move start3 = {12,8};
    Move start4 = {9,9};
    Move start5 = {10,9};
    Move start6 = {11,9};
    Move start7 = {12,9};
    Move start8 = {8,10};
    Move start9 = {10,10};
    Move start10 = { 12,10 };
    Move start11 = { 8,12 };
    Move start12 = { 10,12 };
    Move start13 = { 12,12 };

    Move start14 = { 8,9 };
    Move start15 = { 9,10 };
    Move start16 = { 8,11 };
    Move start17 = { 9,11 };
    Move start18 = { 10,11 };
    Move start19 = { 11,11 };
    Move start20 = { 12,11 };
    Move start21 = { 11,12 };
    Move start22 = { 12,13 };





    m.placeStones(start1, 1, &b);
    m.placeStones(start2, 1, &b);
    m.placeStones(start3, 1, &b);
    m.placeStones(start4, 1, &b);
    m.placeStones(start5, 1, &b);
    m.placeStones(start6, 1, &b);
    m.placeStones(start7, 1, &b);
    m.placeStones(start8, 1, &b);
    m.placeStones(start9, 1, &b);
    m.placeStones(start10, 1, &b);
    m.placeStones(start11, 1, &b);
    m.placeStones(start12, 1, &b);
    m.placeStones(start13, 1, &b);

    m.placeStones(start14, 2, &b);
    m.placeStones(start15, 2, &b);
    m.placeStones(start16, 2, &b);
    m.placeStones(start17, 2, &b);
    m.placeStones(start18, 2, &b);
    m.placeStones(start19, 2, &b);
    m.placeStones(start20, 2, &b);
    m.placeStones(start21, 2, &b);
    m.placeStones(start22, 2, &b);
    b.printBoard();

    Move check1 = {9,0};
    Move check2 = {9,18};
    m.placeStones(check1, 2, &b);
    m.placeStones(check2, 2, &b);

    printf("%d\n",m.chkVic(b.board, check1, check2));


    b.printBoard();
    */

}




