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
static float rollTime = 0;
static float selTime = 0;
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

        //check vertical 
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
            // if(gapCnt > 1) rightBlocked = false;
            gapCnt = 0;
        }
        else if(board[mov.y][i] == EMPTY){
            if(firstGap) firstGap = false;
            else secondGap = false;
            allCount++;
            gapCnt++;
        } 
        else break;
        if(gapCnt > 1) rightBlocked = false;
    }
    firstGap = true; secondGap = true; gapCnt = 0;
    for(int i = mov.x -1; i > -1; i--){
        if(board[mov.y][i] == aiColor){
            if(firstGap) stoneCenter++;
            else if(secondGap) stoneLeft++;
            allCount++;
            // if(gapCnt > 1) leftBlocked = false;
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
        printf("%d stones\n", stoneCenter + stoneRight + stoneLeft);
        if(leftBlocked) printf("left blocked\n");
        if(rightBlocked) printf("right blocked\n");
    }
    else {
        if(leftBlocked) printf("%d at left blocked\n", stoneCenter + stoneLeft);
        else printf("%d at left unblocked\n", stoneCenter + stoneLeft);
        if(rightBlocked) printf("%d at right blocked\n", stoneCenter + stoneRight);
        else printf("%d at right unblocked\n", stoneCenter + stoneLeft);
    }
}

int 
Mini::chkRelation(Move mov1, Move mov2)
{
    if(mov1.x == mov2.x) return 1; // horizontal
    else if(mov1.y == mov2.y) return 2; // vertical
    else if(mov1.x - mov2.x == mov1.y - mov2.y) return 3; // top left to bot right
    else if(mov1.x - mov2.x == 0 - mov1.y - mov2.y) return 4; // top right to bot left
    else return 0;
}