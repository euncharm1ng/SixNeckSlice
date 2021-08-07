#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "mcts.h"
#define WHITE 2
#define BLACK 1

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

    
    

    //m.select();
    // m.findMoves(m.root);
    // time_t startTime = clock();
    
    // for(int i =0; i < 1000; i++)
    //     m.rollout(m.root);
    // time_t endTime = clock();
    // float time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    // printf("DEBUG MODE: rollout(): =============== %f sec\n", time);

    //printf("%d\n", chk);
    //m.expansion(m.root); // TODO:
    

    pNode result = m.runGame();

    return 0;
    // int turn = m.aiColor;
    // short boardToPrint[BOARDSIZE][BOARDSIZE];
    // for(int i =0; i< 19; i++){
    //     for(int j =0; j < 19; j++)
    //         boardToPrint[i][j] = m.board[i][j];
    // }
    // int k = result->moveSize;
    // for(int i =0; i< k; i+=2){ //TODO: gotta handle the case with black place 1 stone only
    //     boardToPrint[result->movesLog[i].y][result->movesLog[i].x] = turn;    
    //     boardToPrint[result->movesLog[i+1].y][result->movesLog[i+1].x] = turn; 
    //     turn = (turn == BLACK) ? WHITE : BLACK; 
    // }
    // for(int i =0; i< 19; i++){
    //     for(int j =0; j < 19; j++)
    //         printf("%d ", boardToPrint[i][j]);
    //     printf("\n");
    // }

}




