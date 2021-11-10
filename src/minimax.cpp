/*
refereced:
    https://www.analyticsvidhya.com/blog/2019/01/monte-carlo-tree-search-introduction-algorithm-deepmind-alphago/
    https://towardsdatascience.com/monte-carlo-tree-search-an-introduction-503d8c04e168
*/
/*
todo:
    change algorithm from mcts to minimax.
        rollout, select, backprop will be disgarded
        new function of evaluating board and choice maker will be added
        evaluate: have to accumulate,ggvG based on how stones form the board. 
            find cnt of attackable points or the urgency, can we attack or not?

*/

#include "../include/minimax.h"
#include <algorithm> 
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <chrono>
#include "../include/connsix.h"

#define MY_F6 9999
#define MY_H6 9999
#define MY_F5 10
#define MY_H5 5
#define MY_F4 200
#define MY_H4 100
#define MY_F3 70
#define MY_H3 36
#define MY_F2 50
#define MY_H2 20
#define MY_F1 10
#define MY_H1 10

#define OP_F6 600
#define OP_H6 600
#define OP_F5 600
#define OP_H5 600
#define OP_F4 600
#define OP_H4 600
#define OP_F3 20
#define OP_H3 15
#define OP_F2 15
#define OP_H2 5
#define OP_F1 2
#define OP_H1 1


#define CHILDSIZE 3

using namespace std;

static int nodeCnt = 0;
const int scoreMY[13] = {0, MY_H1, MY_F1, MY_H2, MY_F2, MY_H3, MY_F3, MY_H4, MY_F4, MY_H5, MY_F5, MY_H6, MY_F6};
const int scoreOP[13] = {0, OP_H1, OP_F1, OP_H2, OP_F2, OP_H3, OP_F3, OP_H4, OP_F4, OP_H5, OP_F5, OP_H6, OP_F6};
ofstream file;

/*
bool
Move::operator==(Move lhs)
{
    if(lhs.x == this->x && lhs.y == this->y) return true;
    return false;
}
*/
/*---------- class minimax ----------*/

Mini::Mini(){}

Mini::Mini(short home_color) 
{
    this->home_color = home_color;
    this->board = (short**)malloc(sizeof(short*) * BOARDSIZE);
    for(int i = 0; i < BOARDSIZE; i++){
        this->board[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        memset(this->board[i], 0, BOARDSIZE);
    }
}



pNode
Mini::run_mini(Move away_move1, Move away_move2)
{

    return expand_root();
    //upate_board();
    //return expansion(); 
}


void 
Mini::expansion(int depth) // 
{
    //recursive
    /*
    if depth is 3
        mv1_top();
        mv2_top();

    else
        mv1_top();
        mv2_top();
        expansion(); //find top 3

    return maxgain;
    */
   pMove mv1 = (pMove)malloc(sizeof(Move) * CHILDSIZE);
   mv1_top(mv1, CHILDSIZE);
   for(int i = 0; i < CHILDSIZE; i++){
       //place mv1[i]
       pMove mv2 = (pMove)malloc(sizeof(Move) * CHILDSIZE);
       mv2_top(mv2, CHILDSIZE);
       for(int j = 0; j < CHILDSIZE; j++){
           //place mv2[j]
           expansion()
           //undo place mv2[j]
       }
       //undo place mv1[i]
   }

    short child_color = (currNode->color == BLACK) ? WHITE : BLACK;
    int gridMoveSize, availMoveSize, twoGridAwaySize, childScore1, childScore2;
    Move mov1, mov2;
    vector<Move> oneGridAway, twoGridAway;
    short board[BOARDSIZE][BOARDSIZE];
    short turn = this->aiColor;

    for (int i = 0; i < BOARDSIZE; i++) {
        for (int j = 0; j < BOARDSIZE; j++)
            board[i][j] = this->board[i][j];
    }
    int k = currNode->moveSize;
    for (int i = 0; i < k; i += 2) {
        board[currNode->movesLog[i].y][currNode->movesLog[i].x] = turn;
        board[currNode->movesLog[i + 1].y][currNode->movesLog[i + 1].x] = turn;
        turn = (turn == BLACK) ? WHITE : BLACK;
    }

    this->findMoves(currNode, oneGridAway, twoGridAway, board);

    for(int i = 0; i < BOARDSIZE; i++){
        for(int j = 0; j < BOARDSIZE; j++){
            if(board[i][j] > OBSTACLE) board[i][j] = EMPTY;
        }
    }

    gridMoveSize = oneGridAway.size();
    twoGridAwaySize = twoGridAway.size();

    pNode tempNode = NULL;
    for (int i = 0; i < gridMoveSize; i++) {
        mov1 = oneGridAway[i];
        if(board[mov1.y][mov1.x]==OBSTACLE) { board[mov1.y][mov1.x] = EMPTY; continue; }
        // puts("");
        childScore1 = this->evalAccum1(board, mov1, this->aiColor);
        // printf("--%d\n", board[mov1.y][mov1.x]);
        for (int j = i + 1; j < gridMoveSize; j++) {
            mov2 = oneGridAway[j];
            if(board[mov2.y][mov2.x]==OBSTACLE) { board[mov2.y][mov2.x] = EMPTY; continue; } 
            childScore2 = this->evalAccum1(board, mov2, this->aiColor);
            tempNode = createNode(child_color, currNode, mov1, mov2, currNode->value + childScore1 + childScore2);
            currNode->children->push_back(tempNode);
            board[mov2.y][mov2.x] = EMPTY;
        }
        for (int j = 0; j < twoGridAwaySize; j++) {
            mov2 = twoGridAway[j];
            if(board[mov2.y][mov2.x]==OBSTACLE) { board[mov2.y][mov2.x] = EMPTY; continue; } 
            childScore2 = this->evalAccum1(board, mov2, this->aiColor);
            tempNode = createNode(child_color, currNode, mov1, mov2, currNode->value + childScore1 + childScore2);
            currNode->children->push_back(tempNode);
            board[mov2.y][mov2.x] = EMPTY;
        }
        board[mov1.y][mov1.x] = EMPTY;
    }
    currNode->children->shrink_to_fit();
}// end of expansion()


/*
int 
Mini::chkVic(short** board, Move mov1, Move mov2) 
{
    int count = 0;
    short color = board[mov1.y][mov1.x];
    int movX = mov1.x, movY = mov1.y;
    for(int loopCnt = 0; loopCnt < 2; loopCnt++){
        if(loopCnt == 1){
            movX = mov2.x;
            movY = mov2.y;
        }
        //check horizontal
        for (int j = movX; j < 19 ; j++) {
            if (board[movY][j] == color) count++;
            else break;
        }
        for (int j = movX; j > -1; j--) {
            if (board[movY][j] == color) count++;
            else break;
        }
        if (count > 6) return color;
        count = 0;

        //check ertical 
        for (int i = movY; i < 19; i++) {
            if (board[i][movX] == color) count++;
            else break;
        }
        for (int i = movY; i > -1 ; i--) {
            if (board[i][movX] == color) count++;
            else break;
        }
        if (count > 6) return color;
        count = 0;

        //check right-up diagonal
        for (int i = movY, j = movX; i < 19 && j > -1 ; i++, j--) {
            if (board[i][j] == color) count++;
            else break;
        }
        for (int i = movY, j = movX; i > -1 && j < 19 ; i--, j++) {
            if (board[i][j] == color) count++;
            else break;
        }
        if (count > 6) return color;
        count = 0;

        //check left-up diagonal
        for (int i = movY, j = movX; i > -1 && j > -1; i--, j--) {
            if (board[i][j] == color) count++;
            else break;
        }
        for (int i = movY, j = movX; i < 19 && j < 19 ; i++, j++) {
            if (board[i][j] == color) count++;
            else break;
        }
        if (count > 6) return color;
        count = 0;
    }
    return 0;
}// end of chkVic()
*/

int
Mini::evalOneRow(short type[BOARDSIZE], short count[BOARDSIZE])
{
    short aiColor = this->aiColor;
        //TODO: gotta hadle red stone
    short currColor, currCnt = 1, gapCnt = 0, frontGap = 0;
    int looper = 0, score = 0;
    bool blockedL = true, blockedR = false, continued = true;

    currColor = type[looper];
    do{
        if(currColor == BLACK || currColor == WHITE){
            currCnt = count[looper];
            int tempLooper = looper;
            while(1){
                tempLooper++;
                if(type[tempLooper] == EMPTY){
                    gapCnt += count[tempLooper];
                    if(gapCnt < 3){ 
                        continued = false;
                        continue;
                    }
                    else if(count[tempLooper] > 1) ;
                    else if(type[tempLooper + 1] != currColor) 
                        blockedR = true;
                }
                else if(type[tempLooper] == currColor){
                    currCnt += count[tempLooper];
                    continue;
                }
                else blockedR = true;


                looper = tempLooper - 1;
                break;
            }
            // if(frontGap+ currCnt + gapCnt < 6) printf("%dDEAD ", frontGap + currCnt + gapCnt);
            // if(blockedR) printf("blockedR ");
            // if(blockedL) printf("blockedL ");
            // printf("stone %d --++ %d count, %d gapCnt, %d frontGap\n", currColor, currCnt, gapCnt, frontGap);
            
            if(frontGap + currCnt + gapCnt < 6) ;
            else{
                currCnt *= 2;
                if(blockedL || blockedR) currCnt--;
                if(currColor == aiColor){
                    if(currCnt > 10 && !continued) score += 0; //FIXME
                    else score += scoreMY[currCnt];
                }
                else   
                    score -= scoreOP[currCnt];
            }
            currCnt = 0; gapCnt = 0; 
            frontGap = (type[looper] == EMPTY)? count[looper] : 0;
        }
        else if (currColor == EMPTY) frontGap = count[looper]; //FIXME, frontGap is tricky

        blockedL = blockedR; blockedR = false; //FIXME, |_0 blockedL uncatched
        continued = true;
        currColor = type[++looper];
    }while(currColor != -1);
    return score;
}
