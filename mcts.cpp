/*
refereced:
    https://www.analyticsvidhya.com/blog/2019/01/monte-carlo-tree-search-introduction-algorithm-deepmind-alphago/
    https://towardsdatascience.com/monte-carlo-tree-search-an-introduction-503d8c04e168
*/
/*
todo:

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

#define DEBUG 0 //to 1 to run in debug mode
#define DEBUGVIC 0 //chkVic()
#define DEBUGROLL 0 //rollout()
#define BLACK 1
#define WHITE 2
#define BOARDSIZE 19
#define MOVRANGE 2
#define TIMELIMIT 20
#define INFI 3e10
#define ROLLWINVAL 10
#define ROLLLOSEVAL -20
#define UCBMULT 5.0


#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"
#define NORM "\033[37m"

using namespace std;

static int nodeCnt = 0;

pNode createNode(short paraColor) {
    pNode newNode = (pNode)malloc(sizeof(Node));
    newNode->t = 0;
    newNode->n = 0;
    newNode->prevMax = 0;
    newNode->color = paraColor;
    newNode->moveSize = 0;
    newNode->mean = 0;
    newNode->parent = NULL;
    newNode->movesLog = (pMove)malloc(1);
    newNode->children = new vector<pNode>(0);
    return newNode;
}

pNode createNode(short paraColor, pNode paraParent, Move mov1, Move mov2) {
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
    newNode->children = new vector<pNode>(0);
    return newNode;
}

void freeNode(pNode paraNode) {
    free(paraNode->movesLog);
    
    for (int i = 0; i < paraNode->children->size(); i++) {
        delete &(paraNode->children[i]);
    }

    /*
    paraNode->children->clear();
    paraNode->children->shrink_to_fit();
    free(paraNode->children);
    free(paraNode);
    */

    delete paraNode->children;
    delete paraNode;
    
}

void addC(pNode paraParent, pNode paraChild) {
    (paraParent->children)->push_back(paraChild);
}

Mcts::Mcts(short** paraBoard, short paraAiColor) {
    this->aiColor = paraAiColor;
    this->board = paraBoard;
    this->root = createNode(paraAiColor);
}

pNode Mcts::runGame() {
    time_t startTime = clock();
    float time = 0;
    short value = 0;
    int i = 0, j = 0;
    pNode treeRoot = this->root;

    this->expansion(treeRoot);
    printf("end of 1st expansion with size: %dm nodecnt: %d\n", treeRoot->children->size(), nodeCnt);

    vector<pNode>& iter = *(treeRoot->children);
    pNode child = NULL;
    for (pNode child : iter) {
        value = this->rollout(child);
        this->backprop(child, value);
        this->expansion(child);
        random_shuffle(child->children->begin(), child->children->end());
    }
    printf("end of 2nd expansion nodeCnt: %d\n", nodeCnt);
    pNode tempNode;
    random_shuffle(treeRoot->children->begin(), treeRoot->children->end());
    do {
        tempNode = treeRoot;
        do {
            tempNode = this->select(tempNode);
        } while (!tempNode->children->empty());

        value = this->rollout(tempNode);
        this->backprop(tempNode, value);


        if (i++ == 5000) {
            printf("------------------------------------------------%d with nodeCnt: %d\n", j, nodeCnt);
            j++; i = 0;
        }
        time_t endTime = clock();
        time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    } while (time < 20);//time < 20);

    nodeCnt = 0;
    //time_t endTime = clock();
    //time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("time: %f\n", time);
    printf("%d runs\n", 5000 * j);


    pNode result = this->returnMov();
    int turn = this->aiColor;
    short boardToPrint[BOARDSIZE][BOARDSIZE];
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++)
            boardToPrint[i][j] = this->board[i][j];
    }
    int k = result->moveSize;
    for (int i = 0; i < k; i += 2) { //TODO: gotta handle the case with black place 1 stone only
        boardToPrint[result->movesLog[i].y][result->movesLog[i].x] = turn;
        boardToPrint[result->movesLog[i + 1].y][result->movesLog[i + 1].x] = turn;
        turn = (turn == BLACK) ? WHITE : BLACK;
    }
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            if (boardToPrint[i][j] == 1) printf(RED "o " NORM);//, boardToRoll[i][j]);
            else if (boardToPrint[i][j] == 2) printf("o ");//, boardToRoll[i][j]);
            else printf(YELLOW "+ " NORM);//, boardToRoll[i][j]);
        }
        printf("\n");
    }
    return result;
}// end of runGame()

pNode Mcts::select(pNode parentNode) {
    float max = -100, chk = 0;
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
    return returnNode;
}// end of select()

void Mcts::expansion(pNode currNode) {
    short child_color = (currNode->color == BLACK) ? WHITE : BLACK;
    int gridMoveSize = 0, availMoveSize = 0;
    Move mov1, mov2;
    vector<Move> oneGridAway, availMoves;

    this->findMoves(currNode, oneGridAway, availMoves);

    gridMoveSize = oneGridAway.size();
    availMoveSize = availMoves.size();
#if DEBUG
    printf("DEBUG MODE expansion(): movesize: %d\n", moveSize);
#endif
    pNode tempNode = NULL;
    for (int i = 0; i < gridMoveSize; i++) {
        mov1 = oneGridAway[i];
        for (int j = i + 1; j < gridMoveSize; j++) {
            mov2 = oneGridAway[j];
            tempNode = createNode(child_color, currNode, mov1, mov2);
            addC(currNode, tempNode);
        }
        for (int j = 0; j < availMoveSize; j++) {
            mov2 = availMoves[j];
            tempNode = createNode(child_color, currNode, mov1, mov2);
            addC(currNode, tempNode);
        }
    }
#if DEBUG
    printf("currnode -> children.size: %d, avail move size: %d\n", currNode->children->size(), gridMoveSize);
#endif
    currNode->children->shrink_to_fit();
}// end of expansion()

float Mcts::rollout(pNode currNode) {
#if DEBUGROLL
    time_t startTime = clock();
#endif
    int size = 0, turn = this->aiColor, vicChk = 0;
    vector<Move> availMoves;
    Move mov1, mov2;
    //short boardToRoll[BOARDSIZE][BOARDSIZE];
    short** boardToRoll = (short**)malloc(sizeof(short*) * BOARDSIZE);
    for (int i = 0; i < BOARDSIZE; i++) {
        boardToRoll[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        for (int j = 0; j < BOARDSIZE; j++) boardToRoll[i][j] = 0;
    }

    srand(time(NULL));

    // board to roll: 
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++)
            boardToRoll[i][j] = this->board[i][j];
    }
    int k = currNode->moveSize;
    for (int i = 0; i < k; i += 2) { //TODO: gotta handle the case with black place 1 stone only
        boardToRoll[currNode->movesLog[i].y][currNode->movesLog[i].x] = turn;
        boardToRoll[currNode->movesLog[i + 1].y][currNode->movesLog[i + 1].x] = turn;
        turn = (turn == BLACK) ? WHITE : BLACK;
    }
    for (short i = 0; i < 19; i++) {
        for (short j = 0; j < 19; j++)
            if (!boardToRoll[i][j]) {
                availMoves.push_back({ j,i });
            }
    }

#if DEBUGROLL
    printf("DEBUG MODE: rollout(): ===============\n");
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++)
            printf("%d ", boardToRoll[i][j]);
        printf("\n");
    }

    printf("DEBUG MODE: rollout() -> printAvailMoves() ===============\n");
    this->printAvailMoves(availMoves);
    int cnt = 0;
#endif

    turn = currNode->color;
    Move temp1;
    Move temp2;
    random_shuffle(availMoves.begin(), availMoves.end());
    do {
        temp1 = availMoves.back();
        availMoves.pop_back();
        temp2 = availMoves.back();
        availMoves.pop_back();
        boardToRoll[temp1.y][temp1.x] = turn;
        boardToRoll[temp2.y][temp2.x] = turn;
        turn = (turn == WHITE) ? BLACK : WHITE;

#if DEBUGROLL
        printf("\nDEBUG MODE: rollout(): do while cnt: %d===============\n", cnt);
        for (int i = 0; i < 19; i++) {
            for (int j = 0; j < 19; j++) {
                if (boardToRoll[i][j] == 1) printf(RED "@ " NORM);//, boardToRoll[i][j]);
                else if (boardToRoll[i][j] == 2) printf("@ ");//, boardToRoll[i][j]);
                else printf(YELLOW "+ " NORM);//, boardToRoll[i][j]);
            }
            printf("\n");
        }
        cnt++;
#endif

        vicChk = this->chkVic(boardToRoll, temp1, temp2);
        if (vicChk) {
#if DEBUGROLL
            time_t endTime = clock();
            float time = (endTime - startTime) / double(CLOCKS_PER_SEC);
            printf("DEBUG MODE: rollout(): =============== %f sec\n", time);
            printf("winner: %d move: x:%2d y:%2d, x:%2d y:%2d \n", vicChk, temp1.x, temp1.y, temp2.x, temp2.y);
#endif

            if (vicChk == this->aiColor) {
                for (int i = 0; i < BOARDSIZE; i++) {
                    free(boardToRoll[i]);
                }
                free(boardToRoll);
                return ROLLWINVAL;
            }
            else {
                for (int i = 0; i < BOARDSIZE; i++) {
                    free(boardToRoll[i]);
                }
                free(boardToRoll);
                return ROLLLOSEVAL;
            }
        }
    } while (availMoves.size() > 0);
    for (int i = 0; i < BOARDSIZE; i++) {
        free(boardToRoll[i]);
    }
    free(boardToRoll);
    return 0;
}// end of rollout()

void Mcts::backprop(pNode currNode, float value) {
    while (currNode->parent != NULL) {
        currNode->n += 1;
        currNode->t += value;
        currNode->mean = currNode->t / (float)currNode->n;
        currNode = currNode->parent;
    }
    currNode->n += 1;
    //return currNode;
}// end of backprop()

void Mcts::findMovesOneGrid(short board[][BOARDSIZE], vector<Move>& moveVec, int tagToAvoid) {
    int iPlus = 1, iMinus = 1, jPlus = 1, jMinus = 1;
    Move oneGridMove;
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            if (board[i][j] > 0 && board[i][j] < tagToAvoid) {

                if (i == 0)
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

void Mcts::findMoves(pNode currNode, vector<Move>& oneGridAway, vector<Move>& availMoves) {
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

void Mcts::printAvailMoves(vector<Move> availMov) {
    int i = 0;
    for (Move temp : availMov) {
        printf("x:%2d y:%2d\t", temp.x, temp.y);
        i++;
        if (i % 10 == 0) printf("\n");
    }
}

int Mcts::chkVic(short** board, Move mov1, Move mov2) {

#if DEBUGVIC
    printf("\nDEBUG MODE: chkVic(): ===============\n");
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++)
            printf("%d ", board[i][j]);
        printf("\n");
    }
#endif

    int count = 0;
    short color = board[mov1.y][mov1.x];

    //CHECK MV1

    //check vertical
    for (int x = mov1.y, y = mov1.x; ; y++) {
        if (board[x][y] == color) count++;
        if (y == 18) break;
        else {
            if (board[x][y] == board[x][y + 1]) continue;
            else break;
        }
    }
    for (int x = mov1.y, y = mov1.x; ; y--) {
        if (board[x][y] == color) count++;
        if (y == 0) break;
        else {
            if (board[x][y] == board[x][y - 1]) continue;
            else break;
        }
    }

    if (--count > 5) return color;
    count = 0;

    //check horizion
    for (int x = mov1.y, y = mov1.x; ; x++) {
        if (board[x][y] == color) count++;
        if (x == 18) break;
        else {
            if (board[x][y] == board[x + 1][y]) continue;
            else break;
        }
    }
    for (int x = mov1.y, y = mov1.x; ; x--) {
        if (board[x][y] == color) count++;
        if (x == 0) break;
        else {
            if (board[x][y] == board[x - 1][y]) continue;
            else break;
        }
    }

    if (--count > 5) return color;
    count = 0;

    //check right-up diagonal
    for (int x = mov1.y, y = mov1.x; ; x++, y--) {
        if (board[x][y] == color) count++;
        if (x == 18 || y == 0) break;
        else {
            if (board[x][y] == board[x + 1][y - 1]) continue;
            else break;
        }
    }
    for (int x = mov1.y, y = mov1.x; ; x--, y++) {
        if (board[x][y] == color) count++;
        if (x == 0 || y == 18) break;
        else {
            if (board[x][y] == board[x - 1][y + 1]) continue;
            else break;
        }
    }
    if (--count > 5) return color;
    count = 0;

    //check left-up diagonal
    for (int x = mov1.y, y = mov1.x; ; x--, y--) {
        if (board[x][y] == color) count++;
        if (x == 0 || y == 0) break;
        else {
            if (board[x][y] == board[x - 1][y - 1]) continue;
            else break;
        }
    }
    for (int x = mov1.y, y = mov1.x; ; x++, y++) {
        if (board[x][y] == color) count++;
        if (x == 18 || y == 18) break;
        else {
            if (board[x][y] == board[x + 1][y + 1]) continue;
            else break;
        }
    }
    if (--count > 5) return color;
    count = 0;

    //CHECK MV2

    //check vertical
    for (int x = mov2.y, y = mov2.x; ; y++) {
        if (board[x][y] == color) count++;
        if (y == 18) break;
        else {
            if (board[x][y] == board[x][y + 1]) continue;
            else break;
        }
    }
    for (int x = mov2.y, y = mov2.x; ; y--) {
        if (board[x][y] == color) count++;
        if (y == 0) break;
        else {
            if (board[x][y] == board[x][y - 1]) continue;
            else break;
        }
    }

    if (--count > 5) return color;
    count = 0;

    //check horizion
    for (int x = mov2.y, y = mov2.x; ; x++) {
        if (board[x][y] == color) count++;
        if (x == 18) break;
        else {
            if (board[x][y] == board[x + 1][y]) continue;
            else break;
        }
    }
    for (int x = mov2.y, y = mov2.x; ; x--) {
        if (board[x][y] == color) count++;
        if (x == 0) break;
        else {
            if (board[x][y] == board[x - 1][y]) continue;
            else break;
        }
    }

    if (--count > 5) return color;
    count = 0;

    //check right-up diagonal
    for (int x = mov2.y, y = mov2.x; ; x++, y--) {
        if (board[x][y] == color) count++;
        if (x == 18 || y == 0) break;
        else {
            if (board[x][y] == board[x + 1][y - 1]) continue;
            else break;
        }
    }
    for (int x = mov2.y, y = mov2.x; ; x--, y++) {
        if (board[x][y] == color) count++;
        if (x == 0 || y == 18) break;
        else {
            if (board[x][y] == board[x - 1][y + 1]) continue;
            else break;
        }
    }
    if (--count > 5) return color;
    count = 0;

    //check left-up diagonal
    for (int x = mov2.y, y = mov2.x; ; x--, y--) {
        if (board[x][y] == color) count++;
        if (x == 0 || y == 0) break;
        else {
            if (board[x][y] == board[x - 1][y - 1]) continue;
            else break;
        }
    }
    for (int x = mov2.y, y = mov2.x; ; x++, y++) {
        if (board[x][y] == color) count++;
        if (x == 18 || y == 18) break;
        else {
            if (board[x][y] == board[x + 1][y + 1]) continue;
            else break;
        }
    }
    if (--count > 5) return color;
    count = 0;

    return 0;
}// end of chkVic()

pNode Mcts::returnMov() {
    float max = -1000, value = 0;
    pNode toReturn = NULL, rootNode = this->root;
    vector<pNode>& iter = *(rootNode->children);
    for (pNode child : iter) {
        //value = child->t / (float)child->n;
        value = child->t;
        if (max < value) {
            max = value;
            toReturn = child;
        }
    }
    return toReturn;
}
