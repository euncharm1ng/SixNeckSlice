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

#include "mcts.h"
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

#define DEBUG 0 //to 1 to run in debug mode
#define DEBUGVIC 0 //chkVic()
#define DEBUGROLL 0 //rollout()
#define MOVRANGE 2
#define TIMELIMIT 20

#define ROLLWINVAL 1
#define ROLLLOSEVAL 1
#define UCBMULT 3
// #define ROLLWINVAL 10
// #define ROLLLOSEVAL -20
// #define UCBMULT 5.0

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
    newNode->prevMax = 0;
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
createNode(short paraColor, pNode paraParent, Move mov1, Move mov2)
{
    nodeCnt++;
    pNode newNode = (pNode)malloc(sizeof(Node));
    newNode->t = 0;
    newNode->n = 0;
    newNode->prevMax = 0;
    newNode->color = paraColor;
    short k = paraParent->moveSize;
    newNode->moveSize = k + 2;
    newNode->mean = 0;
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

/*---------- class mcts ----------*/

Mcts::Mcts(){}
Mcts::Mcts(short** paraBoard, short paraAiColor) 
{
    this->aiColor = paraAiColor;
    this->board = paraBoard;
    this->root = createNode(paraAiColor);
}

void 
Mcts::setRoot(short paraAiColor)
{
    this->root = createNode(paraAiColor);
}

pNode 
Mcts::runGame(Move userMov1, Move userMov2) 
{
    time_t startTime = clock();
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    float time = 0;
    float value = 0;
    int i = 0, j = 0;
    pNode treeRoot = this->root, tempNode;

    this->expansion(treeRoot);
    printf("end of 1st expansion with size: %lum nodecnt: %d\n", treeRoot->children->size(), nodeCnt);
    
    vector<pNode> &iter = *(treeRoot->children);

    for (pNode child : iter) {
        for(int rollcnt = 0; rollcnt < 100; rollcnt++){
            value = this->rollout(child);
            // printf("%f \t", value);
            this->backprop(child, value);
        }
        this->expansion(child);
        shuffle(child->children->begin(), child->children->end(), default_random_engine(seed)); // use shuffle
        // shuffle(availMoves.begin(), availMoves.end(), default_random_engine(seed));
    }
    // printf("end of 2nd expansion nodeCnt: %d\n", nodeCnt);
    // random_shuffle(treeRoot->children->begin(), treeRoot->children->end()); // use shuffle
    
    do {
        tempNode = treeRoot;
        do {
            tempNode = this->select(tempNode);
        }while(!tempNode->children->empty()); 

        value = this->rollout(tempNode);
        // printf("value : %f\t", value);
        this->backprop(tempNode, value);
        if (i++ == 5000) {
            printf("---------%2d with nodeCnt: %d\n", j, nodeCnt);
            time_t endTime = clock();
            time = (endTime - startTime) / double(CLOCKS_PER_SEC);
            j++; i = 0;
        }

    }while(time < 20); // j<20);
    time_t endTime = clock();
    time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("%d runs took: %f sec\n\n", 5000 * j, time);
    printf("sel took : %f sec\n", selTime);
    printf("roll took : %f sec\n", rollTime);
    pNode result = this->returnMov();
    nodeCnt= 0;
    selTime = 0;
    rollTime = 0;
    appendUCB(this->root, this->board, result);
    return result;
}// end of runGame()

pNode 
Mcts::select(pNode parentNode) 
{
    time_t start = clock();
    float max = -1000000, chk = 0;
    pNode returnNode = NULL;
    float parentN = log((float)parentNode->n);

    vector<pNode>& iter = *(parentNode->children);
    float tempNodeN = 0;
    for (pNode tempNode : iter) {
        tempNodeN = (float)tempNode->n;
        if (tempNodeN == 0) return tempNode;
        chk = tempNode->mean + UCBMULT * (sqrt(parentN / tempNodeN));
        if (chk > max) {
            max = chk;
            returnNode = tempNode;
        }
    }
    time_t end = clock(); 
    selTime += (end - start) / double(CLOCKS_PER_SEC);
    return returnNode;
}// end of select()

bool Mcts::chkPossible(short** board, Move mov1) {
    //TODO: add cases to handle red stone.
    short i = mov1.y, j = mov1.x;

    //check vertical
    if (j < 16 && board[i][j+2] != 0 && board[i][j+2] == board[i][j+3]) return true;
    else if (j > 2 && board[i][j-2] != 0 && board[i][j-2] == board[i][j-3]) return true;

    //check horizion
    else if (i < 16 && board[i+2][j] != 0 && board[i+2][j] == board[i+3][j]) return true;
    else if (i > 2 && board[i-2][j] != 0 && board[i-2][j] == board[i-3][j]) return true;

    //check right-up diagonal
    else if (i < 16 && j > 2 && board[i+2][j-2] != 0 && board[i+2][j-2] == board[i+3][j-3]) return true;
    else if (i > 2 && j < 16 && board[i-2][j+2] !=0 && board[i-2][j+2] == board[i-3][j+3]) return true;

    //check left-up diagonal
    else if (i > 2 && j > 2 && board[i-2][j-2] != 0 && board[i-2][j-2] == board[i-3][j-3]) return true;
    else if (i < 16 && j < 16 & board[i+2][j+2] != 0 && board[i+2][j+2] == board[i+3][j+3]) return true;
    else return false;
}

void 
Mcts::expansion(pNode currNode) 
{
    short child_color = (currNode->color == BLACK) ? WHITE : BLACK;
    int gridMoveSize = 0, availMoveSize = 0, twoGridAwaySize = 0;
    Move mov1, mov2;
    vector<Move> oneGridAway, availMoves, twoGridAway;
    // short** placeFirstMov = (short**)malloc(sizeof(short*) * BOARDSIZE);
    // for(int i =0; i< BOARDSIZE; i++){
    //     placeFirstMov[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
    //     for(int j =0; j< BOARDSIZE; j++){
    //         placeFirstMov[i][j] = this->board[i][j];
    //     }
    // }

    this->findMoves(currNode, oneGridAway, availMoves);
    
    gridMoveSize = oneGridAway.size();
    availMoveSize = availMoves.size();

    for(int i =0; i< availMoveSize; i++){
        if(chkPossible(this->board, availMoves[i]))
            twoGridAway.push_back(availMoves[i]);
    }
    twoGridAwaySize = twoGridAway.size();

    pNode tempNode = NULL;
    for (int i = 0; i < gridMoveSize; i++) {
        mov1 = oneGridAway[i];
        for (int j = i + 1; j < gridMoveSize; j++) {
            mov2 = oneGridAway[j];
            tempNode = createNode(child_color, currNode, mov1, mov2);
            currNode->children->push_back(tempNode);
        }
        for (int j = 0; j < twoGridAwaySize; j++) {
            mov2 = twoGridAway[j];
            tempNode = createNode(child_color, currNode, mov1, mov2);
            currNode->children->push_back(tempNode);
        }
    }
    currNode->children->shrink_to_fit();
    
    // for(int i =0; i< BOARDSIZE; i++){
    //     free(placeFirstMov[i]);
    // }
    // free(placeFirstMov);

}// end of expansion()


float 
Mcts::rollout(pNode currNode) 
{
    time_t start = clock();
    short turn = this->aiColor;
    int vicChk = 0;
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    vector<Move> availMoves;
    // srand(time(NULL));
    short** boardToRoll = (short**)malloc(sizeof(short*) * BOARDSIZE);
    for (short i = 0; i < BOARDSIZE; i++) {
        boardToRoll[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        for (short j = 0; j < BOARDSIZE; j++){ 
            boardToRoll[i][j] = this->board[i][j];
            if (!boardToRoll[i][j]) {
                availMoves.push_back({ j,i });
            }
        }
    }

    int k = currNode->moveSize;
    for (int i = 0; i < k; i += 2) { //TODO: gotta handle the case with black place 1 stone only, gotta check the moveslog too
        boardToRoll[currNode->movesLog[i].y][currNode->movesLog[i].x] = turn;
        boardToRoll[currNode->movesLog[i + 1].y][currNode->movesLog[i + 1].x] = turn;
        vicChk = chkVic(boardToRoll, currNode->movesLog[i], currNode->movesLog[i+1]);
        if(vicChk){
            for(int i =0; i < BOARDSIZE; i++){
                free(boardToRoll[i]);
            }
            free(boardToRoll);
            time_t end = clock(); 
            rollTime += (end - start) / double(CLOCKS_PER_SEC);
            // printf("rollout() won immediately\n");
            if(vicChk == this->aiColor) return ROLLWINVAL;
            else return ROLLLOSEVAL;
        }
        turn = (turn == BLACK) ? WHITE : BLACK;
    }
    turn = currNode->color;
    Move temp1, temp2;

    shuffle(availMoves.begin(), availMoves.end(), default_random_engine(seed));
    do {
        temp1 = availMoves.back();
        availMoves.pop_back();
        temp2 = availMoves.back();
        availMoves.pop_back();
        boardToRoll[temp1.y][temp1.x] = turn;
        boardToRoll[temp2.y][temp2.x] = turn;
        
        vicChk = this->chkVic(boardToRoll, temp1, temp2);
        if (vicChk) {
            for(int i =0; i < BOARDSIZE; i++){
                free(boardToRoll[i]);
            }
            free(boardToRoll);
            time_t end = clock(); 
            rollTime += (end - start) / double(CLOCKS_PER_SEC);
            // printf("--%d ", vicChk);
            if (vicChk == this->aiColor) 
                return ROLLWINVAL;
            else 
                return ROLLLOSEVAL;
        }
        turn = (turn == WHITE) ? BLACK : WHITE;
    } while (availMoves.size() > 0);

    for (int i = 0; i < BOARDSIZE; i++) {
        free(boardToRoll[i]);
    }
    free(boardToRoll);
    time_t end = clock(); 
    rollTime += (end - start) / double(CLOCKS_PER_SEC);
    return 0;
}// end of rollout()

void
Mcts::backprop(pNode currNode, float value) 
{
    while (currNode->parent != NULL) {
        currNode->n += 1;
        currNode->t += value;
        currNode->mean = currNode->t / (float)currNode->n;
        currNode = currNode->parent;
    }
    currNode->n += 1;
}// end of backprop()

void 
Mcts::findMovesOneGrid(short board[][BOARDSIZE], vector<Move>& moveVec, int tagToAvoid) 
{
    int iPlus = 1, iMinus = 1, jPlus = 1, jMinus = 1;
    Move oneGridMove;
    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
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
Mcts::findMoves(pNode currNode, vector<Move>& oneGridAway, vector<Move>& availMoves) 
{
    short tempBoard[BOARDSIZE][BOARDSIZE]; //score board
    short board[BOARDSIZE][BOARDSIZE]; // stone board

    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++)
            tempBoard[i][j] = 2;
    }

    // build board according to the movelog
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++)
            board[i][j] = this->board[i][j];
    }
    int k = currNode->moveSize;
    for (int i = 0; i < k; i++) {
        board[currNode->movesLog[i].y][currNode->movesLog[i].x] = 3; //just for find move
    }
    this->findMovesOneGrid(board, oneGridAway, 4);
    this->findMovesOneGrid(board, availMoves, 5);
}

int 
Mcts::chkVic(short** board, Move mov1, Move mov2) 
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
Mcts::returnMov() 
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
    //toReturn = this -> root -> children -> at(0);
    return toReturn;
}
