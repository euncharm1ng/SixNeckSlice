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

#include "minimax.h"
#include <stdlib.h>
#include <time.h>
#include <algorithm> 
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

#define ROLLWINVAL 1
#define ROLLLOSEVAL 1
#define UCBMULT 3

#define SIX 12
#define FIVE 10
#define FOUR 8
#define THREE 6
#define TWO 4
#define ONE 2
#define HALFSIX 11
#define HALFFIVE 9
#define HALFFOUR 7
#define HALFTHREE 5
#define HALFTWO 3
#define HALFONE 1

using namespace std;

static int nodeCnt = 0;
const int scoreArr[13] = {0, HALFONE, ONE, HALFTWO, TWO, HALFTHREE, THREE, HALFFOUR, FOUR, HALFFIVE, FIVE, HALFSIX, SIX};
ofstream file;

bool
Move::operator==(Move lhs)
{
    if(lhs.x == this->x && lhs.y == this->y) return true;
    return false;
}

pNode 
createNode(short paraColor) 
{
    pNode newNode = (pNode)malloc(sizeof(Node));
    newNode->t = 0;
    newNode->n = 0;
    newNode->color = paraColor;
    newNode->moveSize = 0;
    newNode->mean = 0;
    newNode->parent = NULL;
    newNode->movesLog = (pMove)malloc(1);
    newNode->children = new vector<pNode>;
    newNode->prevSel = NULL;
    return newNode;
}


pNode 
createNode(short paraColor, pNode paraParent, Move mov1, Move mov2, int toAccumulate)
{
    nodeCnt++;
    pNode newNode = (pNode)malloc(sizeof(Node));
    newNode->t = 0;
    newNode->n = 0;
    newNode->color = paraColor;
    short k = paraParent->moveSize;
    newNode->moveSize = k + 2;
    newNode->mean = 0;
    newNode->accumulate = toAccumulate;
    newNode->parent = paraParent;
    newNode->movesLog = (pMove)malloc(sizeof(Move) * newNode->moveSize);
    for (short i = 0; i < k; i++) {
        newNode->movesLog[i].x = paraParent->movesLog[i].x;
        newNode->movesLog[i].y = paraParent->movesLog[i].y;
    }
    newNode->movesLog[k].x = mov1.x;
    newNode->movesLog[k++].y = mov1.y;
    newNode->movesLog[k].x = mov2.x;
    newNode->movesLog[k].y = mov2.y;
    newNode->children = new vector<pNode>;
    newNode->prevSel = NULL;
    return newNode;

}

void*
freeAll (void* inputNode) 
{
    pNode currNode = (pNode) inputNode;
    while(!currNode->children->empty()){
        freeAll(currNode->children->back());
        currNode->children->pop_back();
    }
    freeNode(currNode);
    return NULL;
}

void 
freeNode(pNode paraNode) 
{
    free(paraNode->movesLog);
    for (int i = 0; i < paraNode->children->size(); i++)
        delete &(paraNode->children[i]);
    delete paraNode->children;
    free(paraNode);
}

void 
appendUCB(pNode root, short** board, pNode result){
    file.open("test.txt", std::ofstream::out | std::ofstream::app);
    if(file.is_open()){
        file << "with the board being : \n";
        file << "y\\x 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8\n";
        for(int i = 0; i < BOARDSIZE; i++){
            int printout = i% 10;
            file << "  " << printout << " ";
            for(int j =0; j < BOARDSIZE; j++){
                if (board[i][j] == BLACK) file << "o ";
                else if (board[i][j] == WHITE) file <<"x ";
                else if (board[i][j] == OBSTACLE) file <<"* ";
                else file << "+ ";
            }
            file <<"\n";
        }
        float parentN = log((float)root->n);

        float chkre = result->mean + UCBMULT * (sqrt(parentN / (float)result->n));
        file << "roll win: " << ROLLWINVAL << " roll lose: " << ROLLLOSEVAL << " ucb mult: " 
                << UCBMULT <<" "<< root->children->size() << " " << root->n
                << " selected: \n" << "move 1: " << result->movesLog[0].x << ", " << result->movesLog[0].y 
                << "\tmove 2: " << result->movesLog[1].x << ", "<< result->movesLog[1].y 
                << "\tucb: " << chkre << "\tn: " << result->n << "\tt: "<< result->t 
                << "\tmean: " << result->mean << "\n\n";

        
        
        vector<pNode> &iter = *(root->children);
        for (pNode child : iter) {
            float chk = child->mean + UCBMULT * (sqrt(parentN / (float)child->n));
            file << "move 1: " << child->movesLog[0].x << ", " << child->movesLog[0].y 
                << "\tmove 2: " << child->movesLog[1].x << ", "<< child->movesLog[1].y 
                << "\tucb: " << chk << "\tn: " << child->n << "\tt: "<< child->t 
                << "\tmean: " << child->mean << "\n";
        }

    }
    file.close();
}


/*---------- class minimax ----------*/

Mini::Mini(){}
Mini::Mini(short** paraBoard, short paraAiColor) 
{
    this->aiColor = paraAiColor;
    this->board = paraBoard;
    this->root = createNode(paraAiColor);
}

void 
Mini::setRoot(short paraAiColor)
{
    this->root = createNode(paraAiColor);
}

pNode 
Mini::runGame(Move userMov1, Move userMov2) 
{
    time_t startTime = clock();
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    float time = 0;
    pNode treeRoot = this->root;

    this->expansion(treeRoot);
    printf("end of 1st expansion with size: %lum nodecnt: %d\n", treeRoot->children->size(), nodeCnt);
    
    vector<pNode> &iter = *(treeRoot->children);
    for (pNode child : iter) {
        this->expansion(child);
    }
    printf("end of 2st expansion nodecnt: %d\n", nodeCnt);
    
    time_t endTime = clock();
    time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("%f sec\n\n", time);
    nodeCnt= 0;
    // return NULL; //FIXME
    return treeRoot->children->at(0);
}// end of runGame()



bool Mini::chkPossible(short** board, Move mov1) {
    //TODO: add cases to handle red stone.
    short i = mov1.y, j = mov1.x;

    //check vertical
    if (j < 16 && board[i][j+2] != EMPTY && board[i][j+2] == board[i][j+3]) return true;
    else if (j > 2 && board[i][j-2] != EMPTY && board[i][j-2] == board[i][j-3]) return true;

    //check horizion
    else if (i < 16 && board[i+2][j] != EMPTY && board[i+2][j] == board[i+3][j]) return true;
    else if (i > 2 && board[i-2][j] != EMPTY && board[i-2][j] == board[i-3][j]) return true;

    //check right-up diagonal
    else if (i < 16 && j > 2 && board[i+2][j-2] != EMPTY && board[i+2][j-2] == board[i+3][j-3]) return true;
    else if (i > 2 && j < 16 && board[i-2][j+2] != EMPTY && board[i-2][j+2] == board[i-3][j+3]) return true;

    //check left-up diagonal
    else if (i > 2 && j > 2 && board[i-2][j-2] != EMPTY && board[i-2][j-2] == board[i-3][j-3]) return true;
    else if (i < 16 && j < 16 & board[i+2][j+2] != EMPTY && board[i+2][j+2] == board[i+3][j+3]) return true;
    else return false;
}

void 
Mini::expansion(pNode currNode)
{
    short child_color = (currNode->color == BLACK) ? WHITE : BLACK;
    int gridMoveSize, availMoveSize, twoGridAwaySize;
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

    // puts("y\\x 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8");
    // for (int i = 0; i < 19; i++) {
    //     printf("  %d ", i%10);
    //     for (int j = 0; j < 19; j++) {
    //         if (board[i][j] == BLACK) printf(CYAN "o " NORM);
    //         else if (board[i][j] == WHITE) printf(PURPLE "o " NORM);
    //         else if (board[i][j] == OBSTACLE) printf(RED "o " NORM);
    //         else if (board[i][j] == EMPTY) printf(YELLOW "+ " NORM);
    //         else printf("%d ", board[i][j]);
    //     }
    //     printf("\n");
    // }

    gridMoveSize = oneGridAway.size();
    twoGridAwaySize = twoGridAway.size();

    pNode tempNode = NULL;
    for (int i = 0; i < gridMoveSize; i++) {
        mov1 = oneGridAway[i]; //calc first move first then, check the relationship btw mov1 and mov2, if no relationship just accumulate, if there is relationship, deduct mov1's points and accumulate mov2.
        for (int j = i + 1; j < gridMoveSize; j++) {
            mov2 = oneGridAway[j];
            tempNode = createNode(child_color, currNode, mov1, mov2, 0);
            currNode->children->push_back(tempNode);
        }
        for (int j = 0; j < twoGridAwaySize; j++) {
            mov2 = twoGridAway[j];
            tempNode = createNode(child_color, currNode, mov1, mov2, 0);
            currNode->children->push_back(tempNode);
        }
    }
    currNode->children->shrink_to_fit();
}// end of expansion()


void 
Mini::findMovesOneGrid(short board[][BOARDSIZE], vector<Move>& moveVec, int tagToAvoid) 
{
    int iPlus = 1, iMinus = 1, jPlus = 1, jMinus = 1;
    Move oneGridMove;
    for(int i = 0; i < BOARDSIZE; i++){
        for(int j = 0; j < BOARDSIZE; j++){
            if(board[i][j] > 0 && board[i][j] < tagToAvoid){
                if(i == 0)
                    iMinus = 0;
                else if (i == 18)
                    iPlus = 0;
                if (j == 0)
                    jMinus = 0;
                else if (j == 18)
                    jPlus = 0;
                for (int k = i - iMinus; k < i + iPlus + 1; k++) {
                    for (int l = j - jMinus; l < j + jPlus + 1; l++) {
                        if (board[k][l]) continue;
                        board[k][l] = tagToAvoid;
                        oneGridMove.x = (short)l;
                        oneGridMove.y = (short)k;
                        moveVec.push_back(oneGridMove);
                    }
                }
                iPlus = 1; iMinus = 1; jPlus = 1; jMinus = 1;
            }
        }
    }
}

void 
Mini::findMoves(pNode currNode, vector<Move>& oneGridAway, vector<Move>& twoGridAway, short board[][BOARDSIZE]) 
{
    vector<Move> availMoves;
    this->findMovesOneGrid(board, oneGridAway, 4);
    this->findMovesOneGrid(board, availMoves, 5);

    int availMoveSize = availMoves.size();
    for(int i =0; i< availMoveSize; i++){
        if(chkPossible(this->board, availMoves[i]))
            twoGridAway.push_back(availMoves[i]);
    }
}

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

pNode
Mini::returnMov() 
{
    float max = -1000000, value = 0;
    pNode toReturn = NULL, rootNode = this->root;
    vector<pNode>& iter = *(rootNode->children);
    for (pNode child : iter) {
        value = child->mean;
        if (max < value) {
            max = value;
            toReturn = child;
        }
    }
    return toReturn;
}


int
Mini::evalOneRow(short type[BOARDSIZE], short count[BOARDSIZE], short aiColor)
{
    // for(int i =0; i < BOARDSIZE; i++){
    //     printf("%2d ", type[i]);
    // }
    // puts("");
    // for(int i =0; i < BOARDSIZE; i++)
    //     printf("%2d ", count[i]);
    // puts("");
        //TODO: gotta hadle red stone
    short currColor, currCnt = 1, gapCnt = 0, frontGap = 0;
    int looper = 0, score = 0;
    bool blockedL = true, blockedR = false;

    currColor = type[looper];
    do{
        if(currColor == BLACK || currColor == WHITE){
            currCnt = count[looper];
            int tempLooper = looper;
            while(1){
                tempLooper++;
                if(type[tempLooper] == EMPTY){
                    gapCnt += count[tempLooper];
                    if(gapCnt < 3) continue;
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
            // printf("stone %d with %d count\n", currColor, currCnt);
            
            if(frontGap + currCnt + gapCnt < 6) ;
            else{
                currCnt *= 2;
                if(blockedL || blockedR) currCnt--;
                if(currColor == aiColor) 
                    score += scoreArr[currCnt];
                else   
                    score -= scoreArr[currCnt];
            }
            currCnt = 0; gapCnt = 0;
        }
        else if (currColor == EMPTY) frontGap = count[looper]; //FIXME, frontGap is tricky

        blockedL = blockedR; blockedR = false; //FIXME, |_0 blockedL uncatched
        currColor = type[++looper];
    }while(currColor != -1);
    return score;
}

void 
Mini::evalRoot(short** board, short aiColor)
{
    short currColor, currCnt = 1, index = 0;
    short type[BOARDSIZE], count[BOARDSIZE];
    int k, score = 0;
    memset(type, -1, sizeof(short) * BOARDSIZE);
    memset(count, -1, sizeof(short) * BOARDSIZE);

    for(int i =0; i < BOARDSIZE; i++){ //horizontal 
        index = 0; currCnt = 1;   
        currColor = board[i][0];
        for(int j = 1; j < BOARDSIZE; j++){
            if(board[i][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[i][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;
        score += this->evalOneRow(type, count, aiColor);
        // printf("%d %d\n", i, this->evalOneRow(type, count, aiColor));
    }

    for(int j = 0; j < BOARDSIZE; j++){ //vertical
        index = 0; currCnt = 1;   
        currColor = board[0][j];
        for(int i = 1; i < BOARDSIZE; i++){
            if(board[i][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[i][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;

        score += this->evalOneRow(type, count, aiColor);
        // printf("%d %d\n", j, this->evalOneRow(type, count, aiColor));
    }

    for(int i = 0; i < 14; i++){ //top left to bot right, left top down
        k = i+1;
        index = 0; currCnt =1;
        currColor = board[i][0];
        for(int j = 1; j < BOARDSIZE - i; j++, k++){
            if(board[k][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[k][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;

        // printf("%d %d\n", i, this->evalOneRow(type, count, aiColor));
        score += this->evalOneRow(type, count, aiColor);
    }
    

    for(int i =1; i < 14; i++){ //top left to bot right, top left to right
        k = 1;
        index = 0; currCnt = 1;
        currColor = board[0][i];
        for(int j = i+1; j < BOARDSIZE; j++, k++){
            if(board[k][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[k][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;

        // printf("%d %d\n", i, this->evalOneRow(type, count, aiColor));
        score += this->evalOneRow(type, count, aiColor);
    }

    for(int i = 18; i > 4; i--){ // bot left to top right, left bot to top
        k = i -1;
        index = 0; currCnt = 1;
        currColor = board[i][0];
        for(int j = 1; j < i + 1; j++, k--){
            if(board[k][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[k][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;

        // printf("%d %d\n", i, this->evalOneRow(type, count, aiColor));
        score += this->evalOneRow(type, count, aiColor);
    }

    for(int i =1; i < 14; i++){ // bot left to top right, bot left to right
        k = 17;
        index = 0; currCnt = 1;
        currColor = board[18][i];
        for(int j = i+1; j < BOARDSIZE; j++, k--){
            if(board[k][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[k][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;

        // printf("%d %d\n", i, this->evalOneRow(type, count, aiColor));
        score += this->evalOneRow(type, count, aiColor);
    }
    printf("root score: %d\n", score);
}

int 
Mini::evalAccum1(short** board, Move mov, short aiColor)
{
    short movx = mov.x, movy = mov.y;
    printf("%d %d %d %d\n", mov.x, mov.y, movx, movy);
    short currColor, currCnt = 1, index = 0;
    short type[BOARDSIZE], count[BOARDSIZE];
    int score = 0, scoreBefore = 0;
    memset(type, -1, sizeof(short) * BOARDSIZE);
    memset(count, -1, sizeof(short) * BOARDSIZE);
    for(int looper = 0; looper < 2; looper++){
        scoreBefore = score;
        //horizontal
        currColor = board[movy][0];
        for(int j = 1; j < BOARDSIZE; j++){
            if(board[movy][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[movy][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;
        score += this->evalOneRow(type, count, aiColor);

        //vertical
        index = 0; currCnt = 1;   
        currColor = board[0][movx];
        for(int i = 1; i < BOARDSIZE; i++){
            if(board[i][movx] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[i][movx];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;
        score += this->evalOneRow(type, count, aiColor);

        //top left to bot right
        int i , j;
        index = 0; currCnt = 1;      
        if(movx > movy) {i = 0; j = movx - movy;}
        else {j = 0; i = movy - movx;}

        currColor = board[i][j];
        for(i++, j++; i < BOARDSIZE && j < BOARDSIZE; i++, j++){
            if(board[i][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[i][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;
        score += this->evalOneRow(type, count, aiColor);

        //bot left to top right
        index = 0; currCnt = 1;
        if(movx + movy < 18) {i = movx + movy; j = 0;}
        else {i = 18; j = movx + movy - 18;}

        currColor = board[i][j];
        for(i--, j++; i > -1 && j < BOARDSIZE; i--, j++){
            if(board[i][j] == currColor) currCnt++;
            else{
                type[index] = currColor;
                count[index++] = currCnt;
                currColor = board[i][j];
                currCnt = 1;
            }
        }
        type[index] = currColor;
        count[index++] = currCnt;
        type[index] = -1;
        count[index++] = -1;
        score += this->evalOneRow(type, count, aiColor);

printf("%d %d %d %d\n", mov.x, mov.y, movx, movy);
        board[movy][movx] = aiColor;
    }
    printf("score before: %d, score after: %d\n", scoreBefore, score);
}


/*
int
Mini::evalAccum1(short** board, Move mov, short aiColor)
{
    //TODO: check if a side is blocked
    int stoneRight = 0, stoneLeft = 0, stoneCenter = 0, allCount = 1, gapCnt = 0;
    bool firstGap = true, secondGap = true, leftBlocked = true, rightBlocked = true;
    for(int i = mov.x + 1; i < 19; i++){
        if(board[mov.y][i] == aiColor){
            if(firstGap) stoneCenter++;
            else if(secondGap) stoneRight++;
            allCount++;
            gapCnt = 0;
        }
        else if(board[mov.y][i] == EMPTY){
            if(firstGap) firstGap = false;
            else secondGap = false;
            allCount++;
            gapCnt++;
        } 
        else{
            if(board[mov.y][i] == OBSTACLE) ;
            else{
                if(stoneRight == 0){
                    printf("oppo is blocked\n");
                }
            }
            break;
        }
        if(gapCnt > 1) rightBlocked = false;
    }
    firstGap = true; secondGap = true; gapCnt = 0;
    for(int i = mov.x -1; i > -1; i--){
        if(board[mov.y][i] == aiColor){
            if(firstGap) stoneCenter++;
            else if(secondGap) stoneLeft++;
            allCount++;
            gapCnt = 0;
        }
        else if(board[mov.y][i] == EMPTY){
            if(firstGap) firstGap = false;
            else secondGap = false;
            allCount++;
            gapCnt++;
        }
        else break;
        if(gapCnt > 1) leftBlocked = false;
    }
    
    printf("left: %d, center: %d, right, %d, all %d\n", stoneLeft, stoneCenter, stoneRight, allCount);

    if(allCount < 6) printf("dead stones\n");
    else if(stoneRight == 0 || stoneLeft == 0) {  
        if(leftBlocked || rightBlocked) printf("connected and blocked: %d\n", scoreArr[(stoneCenter + stoneRight + stoneLeft)*2-1]);
        else printf("connected: %d\n", scoreArr[(stoneCenter + stoneRight + stoneLeft)*2]);
    }
    else {
        if(leftBlocked) printf("left and blocked: %d\n", scoreArr[(stoneCenter + stoneLeft)*2 -1]);
        else printf("left: %d\n", scoreArr[(stoneCenter + stoneLeft)*2]);
        if(rightBlocked) printf("right and blocked: %d\n", scoreArr[(stoneCenter + stoneRight)*2 -1]);
        else printf("right: %d\n", scoreArr[(stoneCenter + stoneRight)*2]);
    }
}
*/

int 
Mini::chkRelation(Move mov1, Move mov2)
{
    if(mov1.x == mov2.x) return 1; // horizontal
    else if(mov1.y == mov2.y) return 2; // vertical
    else if(mov1.x - mov2.x == mov1.y - mov2.y) return 3; // top left to bot right
    else if(mov1.x - mov2.x == 0 - mov1.y - mov2.y) return 4; // top right to bot left
    else return 0;
}