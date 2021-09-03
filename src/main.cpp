#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
// #include "mcts.h"
#include "minimax.h"

#define BOARDSIZE 19

using namespace std;

int main(){
    // runGUI();
    Mini min = Mini();
    short **board;
    board = (short**)malloc(sizeof(short*) * BOARDSIZE);
    for(int i =0; i < BOARDSIZE; i++){
        board[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        for(int j =0; j < BOARDSIZE; j++){
            board[i][j] = 0;
        }
    }

    // board[10][10] = 1;
    // board[9][7] = 2;
    // board[8][8] = 2;
    board[9][10] = 1;
    board[8][10]= 1;
    board[8][11] = 1;
    board[8][9]= 2;
    board[7][9]= 2;
    board[9][11] = 2;
    // board[6][10]= 1;
    // board[9][11] = 1;
    // board[9][4] = 1;
    // board[9][13] = 1;


    // board[18][2] = 1;
    board[18][3] = 1;
    board[18][4] = 2;
    // board[18][5] = 2;
    board[18][6] = 2;
    // board[18][7] = 2;
    board[18][8] = 2;
    board[18][9] = 2;
    board[18][10] = 1;
    // board[18][11] = 1;
    // board[18][12] = 3;
    // board[18][13] = 1;
    board[18][14] = 2;

    board[5][11] = 1;
    board[6][12] = 1;
    board[7][13] = 1;
    board[8][14] = 1;

    board[11][5] = 1;
    board[12][6] = 1;
    board[13][7] = 1;
    board[14][8] = 1;


    puts("y\\x 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8");
    for (int i = 0; i < 19; i++) {
        printf("  %d ", i%10);
        for (int j = 0; j < 19; j++) {
            if (board[i][j] == BLACK) printf(CYAN "o " NORM);
            else if (board[i][j] == WHITE) printf("o ");
            else if (board[i][j] == OBSTACLE) printf(RED "o " NORM);
            else printf(YELLOW "+ " NORM);
        }
        printf("\n");
    }

    min.evalRoot(board, 2);
    min.evalAccum1(board, {0, 12}, 2);
        puts("y\\x 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8");
    for (int i = 0; i < 19; i++) {
        printf("  %d ", i%10);
        for (int j = 0; j < 19; j++) {
            if (board[i][j] == BLACK) printf(CYAN "o " NORM);
            else if (board[i][j] == WHITE) printf("o ");
            else if (board[i][j] == OBSTACLE) printf(RED "o " NORM);
            else printf(YELLOW "+ " NORM);
        }
        printf("\n");
    }

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







