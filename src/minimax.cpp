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

using namespace std;

static int nodeCnt = 0;
const int scoreMY[13] = {0, MY_H1, MY_F1, MY_H2, MY_F2, MY_H3, MY_F3, MY_H4, MY_F4, MY_H5, MY_F5, MY_H6, MY_F6};
const int scoreOP[13] = {0, OP_H1, OP_F1, OP_H2, OP_F2, OP_H3, OP_F3, OP_H4, OP_F4, OP_H5, OP_F5, OP_H6, OP_F6};
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
    newNode->color = paraColor;
    newNode->moveSize = 0;
    newNode->parent = NULL;
    newNode->movesLog = (pMove)malloc(1);
    newNode->children = new vector<pNode>;
    return newNode;
}


pNode 
createNode(short paraColor, pNode paraParent, Move mov1, Move mov2, int accum)
{
    nodeCnt++;
    pNode newNode = (pNode)malloc(sizeof(Node));
    newNode->color = paraColor;
    short k = paraParent->moveSize;
    newNode->moveSize = k + 2;
    newNode->value = accum;
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
write(pNode root, short** board, pNode result){
    file.open("test.txt", std::ofstream::out | std::ofstream::app);
    if(file.is_open()){
        file << "\nwith the board being : \n";
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
        file << "mov1x " << result->movesLog[0].x << " mov1y " << result->movesLog[0].y 
                << "\tmov2x " << result->movesLog[1].x << " mov2y " << result->movesLog[1].y 
                << "\tvalue " << result->value << "\n\n";
       
        vector<pNode> &iter = *(root->children);
        for (pNode child : iter) {
            file << "mov1x " << child->movesLog[0].x << " mov1y " << child->movesLog[0].y 
                << "\tmov2x " << child->movesLog[1].x << " mov2y " << child->movesLog[1].y 
                << "\tvalue " << child->value << "\n";
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

    treeRoot->value = this->evalRoot(this->board);
    printf("tree root val: %d\n", treeRoot->value);
    this->expansion(treeRoot);
    printf("end of 1st expansion, nodecnt: %d\n", nodeCnt);
    /*
    vector<pNode> &iter = *(treeRoot->children);
    for (pNode child : iter) {
        this->expandChild(child);
        // printf("%d, %d and %d, %d scored: %d\n", child->movesLog[0].x, child->movesLog[0].y, child->movesLog[1].x, child->movesLog[1].y, child->value);
    }
    printf("end of 2st expansion nodecnt: %d\n", nodeCnt);
    */
    time_t endTime = clock();
    time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("%f sec\n\n", time);
    nodeCnt = 0;
    pNode result = select(treeRoot);
    write(treeRoot, this->board, result);
    return result;
}// end of runGame()

pNode
Mini::select(pNode root)
{
    vector<pNode> &iter = *(root->children);
    pNode toReturn = iter.at(0);
    int max = iter.at(0)->value;
    for (pNode child : iter) {
        if(child->value > max){
            toReturn = child;
            max = child->value;
        }
    }
    return toReturn;
}

void 
Mini::expansion(pNode currNode)
{
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

void 
Mini::expandChild(pNode currNode)
{
    // printf("curr val: %d\n", currNode->value);
    int gridMoveSize, availMoveSize, twoGridAwaySize, childScore1, childScore2, min = 99999;
    Move mov1, mov2;
    vector<Move> oneGridAway, twoGridAway;
    short board[BOARDSIZE][BOARDSIZE];
    short turn = this->aiColor, childrenColor = (currNode->color == BLACK)? WHITE: BLACK;


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
        childScore1 = this->evalAccum1(board, mov1, currNode->color);
        for (int j = i + 1; j < gridMoveSize; j++) {
            mov2 = oneGridAway[j];
            childScore2 = this->evalAccum1(board, mov2, currNode->color);
            if(min > currNode->value + childScore1 + childScore2) 
                min = currNode->value + childScore1 + childScore2;
            board[mov2.y][mov2.x] = EMPTY;
        }
        for (int j = 0; j < twoGridAwaySize; j++) {
            mov2 = twoGridAway[j];
            childScore2 = this->evalAccum1(board, mov2, currNode->color);
            if(min > currNode->value + childScore1 + childScore2) 
                min = currNode->value + childScore1 + childScore2;
            board[mov2.y][mov2.x] = EMPTY;
        }
        board[mov1.y][mov1.x] = EMPTY;
    }
    currNode->value = min;
    // printf("min val: %d\n", currNode->value);
}

bool 
Mini::chkPossible(short** board, Move mov1) 
{
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

int 
Mini::evalRoot(short** board)
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
        score += this->evalOneRow(type, count);
        // printf("%d %d\n", i, this->evalOneRow(type, count));
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

        score += this->evalOneRow(type, count);
        // printf("%d %d\n", j, this->evalOneRow(type, count));
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

        // printf("%d %d\n", i, this->evalOneRow(type, count));
        score += this->evalOneRow(type, count);
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

        // printf("%d %d\n", i, this->evalOneRow(type, count));
        score += this->evalOneRow(type, count);
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

        // printf("%d %d\n", i, this->evalOneRow(type, count));
        score += this->evalOneRow(type, count);
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

        // printf("%d %d\n", i, this->evalOneRow(type, count));
        score += this->evalOneRow(type, count);
    }
    return score;
    
}

int 
Mini::evalAccum1(short board[][BOARDSIZE], Move mov, short inputStone)
{
    short movx = mov.x, movy = mov.y;
    short currColor, currCnt = 1, index = 0;
    short type[BOARDSIZE], count[BOARDSIZE];
    int score = 0, scoreBefore = 0;
    memset(type, -1, sizeof(short) * BOARDSIZE);
    memset(count, -1, sizeof(short) * BOARDSIZE);
    for(int looper = 0; looper < 2; looper++){
        scoreBefore = score;
        score = 0;
        //horizontal
        index = 0; currCnt = 1; 
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
        score += this->evalOneRow(type, count);

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
        score += this->evalOneRow(type, count);

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
        score += this->evalOneRow(type, count);

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
        score += this->evalOneRow(type, count);

        board[movy][movx] = inputStone;
    }
    // printf("%2d %2d - score before: %d, score after: %d\n", movx, movy, scoreBefore, score);
    score -= scoreBefore;
    return score;
}
