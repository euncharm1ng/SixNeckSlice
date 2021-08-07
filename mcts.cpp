/*
where monte carlo tree search is taking place.
selection
expansion
sumulation (rollout)
backpropagation
refereced:
    https://www.analyticsvidhya.com/blog/2019/01/monte-carlo-tree-search-introduction-algorithm-deepmind-alphago/
    https://towardsdatascience.com/monte-carlo-tree-search-an-introduction-503d8c04e168
*/
/*
todo:
    top-down: selection (update n of all passing nodes )
        calc ucb every time
        remove parent node
    bottom-up: backpropagation (update ucb, t of ancestor node)
    rungame (selction -> expansion -> rollout -> backprop) for TIMELIMIT == 20 sec
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

#define DEBUG 0 //to 1 to run in debug mode
#define BLACK 1
#define WHITE 2
#define BOARDSIZE 19
#define MOVRANGE 2
#define TIMELIMIT 20
#define INFI 3e10

using namespace std;



pNode createNode(short paraColor){
    pNode newNode = (pNode)malloc(sizeof(Node));
    newNode->t = 0;
    newNode->n = 0;
    newNode->color = paraColor;
    newNode->moveSize = 0;
    newNode->ucb = INFI;
    newNode->parent = NULL;
    newNode->movesLog = (pMove)malloc(1);
    newNode->children = new vector<pNode>(0);
    return newNode;
}

pNode createNode(short paraColor, pNode paraParent, Move mov1, Move mov2){
    pNode newNode = (pNode)malloc(sizeof(Node));
    newNode->t = 0;
    newNode->n = 0;
    newNode->color = paraColor;
    short k = paraParent->moveSize;
    newNode->moveSize = k + 2;
    newNode->ucb = INFI;
    newNode->parent = paraParent;
    newNode->movesLog = (pMove)malloc(sizeof(Move) * newNode->moveSize);
    for(short i = 0; i< k; i++){
        newNode->movesLog[i].x = paraParent->movesLog[i].x;
        newNode->movesLog[i].y = paraParent->movesLog[i].y;
    }
    newNode->movesLog[k].x = mov1.x;
    newNode->movesLog[k++].y = mov1.y;
    newNode->movesLog[k].x = mov2.x;
    newNode->movesLog[k].y = mov2.y;
    newNode->children = new vector<pNode>(0);

    //newNode->children.push_back(paraParent);
    //printf("working?\n");
    return newNode;
}

void freeNode(pNode paraNode){
    free(paraNode->movesLog);
    paraNode->children->clear();
    paraNode->children->shrink_to_fit();
    free(paraNode->children);
    free(paraNode);
}

void addC(pNode paraParent, pNode paraChild){
    (paraParent -> children)->push_back(paraChild);
}


Mcts::Mcts(short **paraBoard, short paraAiColor){
    printf("Mcts constructor\n");
    this->aiColor = paraAiColor;
    this->board = paraBoard;
    this->root = createNode(paraAiColor);
}

pNode Mcts::runGame(){
    time_t startTime = clock();
    float time = 0;
    short value = 0;
    int i = 0, j =0;


    this->expansion(this->root);
    printf("end of 1st expansion with size: %d\n", this->root->children->size());
 
    vector<pNode> &iter = *(this->root->children);
    pNode child = NULL;
    for(pNode child : iter){
        value = this->rollout(child);
        this->backprop(child, value);
        this->expansion(child);
    }
    printf("end of 2nd expansion\n");

    pNode tempNode = NULL;
    do {
        tempNode = this->root;
        do{
            tempNode = this->select(tempNode);
        }while(!tempNode->children->empty()); 
        if(tempNode->n != 0){
            this->expansion(tempNode);
            tempNode = this->select(tempNode);
        } 
        value = this->rollout(tempNode);
        this->backprop(tempNode, value);

        if (i++ == 5000) {
            printf("------------------------------------------------%d\n", j);
            j++;
            i = 0;

            // time_t endTime = clock();
            // time = (endTime - startTime) / double(CLOCKS_PER_SEC);
            // if(time > 20) break;
        }
    }while(j<30);//time < 20);
    time_t endTime = clock();
    time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("time: %f\n", time);
    printf("%d runs\n", 5000*j);


    pNode result = this->returnMov();
    int turn = this->aiColor;
    short boardToPrint[BOARDSIZE][BOARDSIZE];
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            boardToPrint[i][j] = this->board[i][j];
    }
    int k = result->moveSize;
    for(int i =0; i< k; i+=2){ //TODO: gotta handle the case with black place 1 stone only
        boardToPrint[result->movesLog[i].y][result->movesLog[i].x] = turn;    
        boardToPrint[result->movesLog[i+1].y][result->movesLog[i+1].x] = turn; 
        turn = (turn == BLACK) ? WHITE : BLACK; 
    }
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            printf("%d ", boardToPrint[i][j]);
        printf("\n");
    }
    return result;
}

void Mcts::expansion(pNode currNode) {
    short child_color = (currNode->color == BLACK) ? WHITE : BLACK;
    int moveSize = 0;
    Move mov1, mov2;
    vector<Move> availMoves;

    this->findMoves(currNode, availMoves);

    moveSize = availMoves.size();
#if DEBUG
    printf("DEBUG MODE expansion(): movesize: %d\n", moveSize);
#endif

    for (int i = 0; i < moveSize; i++) {
        mov1 = availMoves[i];
        for (int j = i + 1; j < moveSize; j++) {
            mov2 = availMoves[j];
            pNode tempNode = createNode(child_color, currNode, mov1, mov2);
            addC(currNode, tempNode);
        }
    }
#if DEBUG
    printf("currnode -> children.size: %d, avail move size: %d\n", currNode->children.size(), moveSize);
#endif
    currNode->children->shrink_to_fit();
}

// 1 : BLACK win, 2 : WHITE win
int Mcts::rollout(pNode currNode){
#if DEBUG
    time_t startTime = clock();
#endif
    int size = 0, turn = this->aiColor, vicChk = 0;
    vector<Move> availMoves;
    Move mov1, mov2;
    short boardToRoll[BOARDSIZE][BOARDSIZE];
    short boardToChkMove[BOARDSIZE][BOARDSIZE];

    srand(time(NULL));

    // board to roll: 
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            boardToRoll[i][j] = this->board[i][j];
    }
    int k = currNode->moveSize;
    for(int i =0; i< k; i+=2){ //TODO: gotta handle the case with black place 1 stone only
        boardToRoll[currNode->movesLog[i].y][currNode->movesLog[i].x] = turn;    
        boardToRoll[currNode->movesLog[i+1].y][currNode->movesLog[i+1].x] = turn; 
        turn = (turn == BLACK) ? WHITE : BLACK; 
    }
#if DEBUG
    printf("DEBUG MODE: rollout(): ===============\n");
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            printf("%d ", boardToRoll[i][j]);
        printf("\n");
    }
#endif

    // board to check move: 
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++){
            if(boardToRoll[i][j])
                boardToChkMove[i][j] = 0;
            else 
                boardToChkMove[i][j] = 2;
        }
    }
    for(int i =0; i < 19; i++){
        for(int j =0 ; j <19; j++){
            if(boardToChkMove[i][j] == 0){
                int ksmall = (i - MOVRANGE < 0) ? 0 : i - MOVRANGE;
                int kbig = (i + MOVRANGE + 1 > 19) ? 19 : i + MOVRANGE + 1;
                int lsmall = (j - MOVRANGE < 0) ? 0 : j - MOVRANGE;
                int lbig = (j + MOVRANGE + 1 > 19) ? 19 : j + MOVRANGE + 1;
                for(int k = ksmall; k < kbig; k++){
                    for(int l = lsmall; l < lbig; l++){
                        boardToChkMove[k][l] *= 1.5;
                    }
                }
            }
        }
    }

    //build avilMoves: 
    for(short i = 0; i < 19; i++){
        for(short j = 0; j < 19; j++){
            if(boardToChkMove[i][j] > 2){
                Move tempMove = {j, i};
                availMoves.push_back(tempMove);
                boardToChkMove[i][j] = 0;
            }
        }
    }

#if DEBUG
    printf("DEBUG MODE: rollout() -> printAvailMoves() ===============\n");
    this->printAvailMoves(availMoves);
    int cnt =0;
#endif

    turn = currNode->color;
    do{
        random_shuffle(availMoves.begin(), availMoves.end());
        Move temp1 = availMoves.back();
        availMoves.pop_back();
        Move temp2 = availMoves.back();
        availMoves.pop_back();
        boardToRoll[temp1.y][temp1.x] = turn;
        boardToRoll[temp2.y][temp2.x] = turn;
        turn = (turn == WHITE)? BLACK : WHITE;

#if DEBUG
    printf("\nDEBUG MODE: rollout(): do while cnt: %d===============\n", cnt);
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            printf("%d ", boardToRoll[i][j]);
        printf("\n");
    }
    cnt++;
#endif

        vicChk = this->chkVic(boardToRoll, temp1, temp2);
        if(vicChk){
#if DEBUG
    time_t endTime = clock();
    float time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("DEBUG MODE: rollout(): =============== %f sec\n", time);
    printf("winner: %d move: x:%2d y:%2d, x:%2d y:%2d \n", vicChk, temp1.x, temp1.y, temp2.x, temp2.y);
#endif
            if(vicChk == this->aiColor) return 1;
            else return -1;
        }

        //update board to check move
        int ksmall = (temp1.y - MOVRANGE < 0) ? 0 : temp1.y - MOVRANGE;
        int kbig = (temp1.y + MOVRANGE + 1 > 19) ? 19 : temp1.y + MOVRANGE + 1;
        int lsmall = (temp1.x - MOVRANGE < 0) ? 0 : temp1.x - MOVRANGE;
        int lbig = (temp1.x + MOVRANGE + 1 > 19) ? 19 : temp1.x + MOVRANGE + 1;
        for(int k = ksmall; k < kbig; k++){
            for(int l = lsmall; l < lbig; l++){
                boardToChkMove[k][l] *= 1.5;
            }
        }
        ksmall = (temp2.y - MOVRANGE < 0) ? 0 : temp2.y - MOVRANGE;
        kbig = (temp2.y + MOVRANGE + 1 > 19) ? 19 : temp2.y + MOVRANGE + 1;
        lsmall = (temp2.x - MOVRANGE < 0) ? 0 : temp2.x - MOVRANGE;
        lbig = (temp2.x + MOVRANGE + 1 > 19) ? 19 : temp2.x + MOVRANGE + 1;
        for(int k = ksmall; k < kbig; k++){
            for(int l = lsmall; l < lbig; l++){
                boardToChkMove[k][l] *= 1.5;
            }
        }
        
        //append new moves
        for(short i = 0; i < 19; i++){
            for(short j = 0; j < 19; j++){
                if(boardToChkMove[i][j] > 2){
                    Move tempMove = {j, i};
                    availMoves.push_back(tempMove);
                    boardToChkMove[i][j] = 0;
                }
            }
        }
    }while(availMoves.size() > 0);
    return 0;
}

//(update ucb, t of ancestor node for root)
// input :
pNode Mcts::backprop(pNode currNode, short value){//vector<Node> &parent, int updateT){
    while(currNode->parent != NULL){
        currNode->n += 1;
        currNode->t += value;
        currNode = currNode->parent;
    }
    currNode->n += 1;
    return currNode;
}

pNode Mcts::select(pNode parentNode) {
    float max = -100, chk = 0;
    pNode returnNode = NULL;
    short parentN = parentNode->n;

    random_shuffle(parentNode->children->begin(), parentNode->children->end());
    vector<pNode> &iter = *(parentNode->children);
    for (pNode tempNode : iter){
    //for(pNode tempNode : parentNode->children) {
        if(tempNode->n ==0) return tempNode;
        //chk = this->calcUCB(tempNode);
        chk = (tempNode->t / tempNode->n) + 2*(sqrt(log(parentN)/tempNode->n));
        if(chk > max) {
            max = chk;
            returnNode = tempNode;
        }
    }
    return returnNode;
    //node->ucb = node->t + 2*(sqrt(log(node->parent->n)/node->n));
}

float Mcts::calcUCB(pNode node){
    node->ucb = (node->t / node->n) + 2*(sqrt(log(node->parent->n)/node->n));
    return node->ucb;
}

void Mcts::findMoves(pNode currNode, vector<Move> &availMoves){
    short tempBoard[BOARDSIZE][BOARDSIZE]; //score board
    short board[BOARDSIZE][BOARDSIZE]; // stone board

    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            tempBoard[i][j] = 2;
    }

    // build board according to the movelog
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            board[i][j] = this->board[i][j];
    }
    int k = currNode->moveSize;
    for(int i =0; i< k; i++){
        board[currNode->movesLog[i].y][currNode->movesLog[i].x] = 3;    
    }

    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
            if(board[i][j]){
                tempBoard[i][j] = 0;
                int ksmall = ifSmaller(i - MOVRANGE);
                int kbig = ifBigger(i + MOVRANGE + 1);
                int lsmall = ifSmaller(j - MOVRANGE);
                int lbig = ifBigger(j + MOVRANGE + 1);
                for (int k = ksmall; k < kbig; k++) {
                    for (int l = lsmall; l < lbig; l++) {
                        tempBoard[k][l] *= 1.5;
                    }
                }
            }
        }
    }

#if DEBUG
    printf("DEBUG MODE: findMovs():-----------------\n");
#endif

    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
#if DEBUG
            printf("%2d ", tempBoard[i][j]);
#endif
            if (tempBoard[i][j] > 2) {
                Move tempMove;
                tempMove.x = (short) j;
                tempMove.y = (short) i;
                availMoves.push_back(tempMove);
                //appendAvailMove(currNode, tempMove);
            }
        }
#if DEBUG
        printf("\n");
#endif
    }
#if DEBUG
    printf("DEBUG MODE: findMoves() -> printAvailMoves()-----------------\n");
    //this->printAvailMoves(currNode->availMov);
#endif
}

int Mcts::ifBigger(int num) {
    if (num > 19) return 19;
    else return num;
}

int Mcts::ifSmaller(int num) {
    if (num < 0) return 0;
    else return num;
}

void Mcts::chkTime() {

}

void Mcts::printAvailMoves(vector<Move> availMov){
    int i = 0;
    for(Move temp : availMov){
        printf("x:%2d y:%2d\t", temp.x, temp.y);
        i++;
        if(i % 10 == 0) printf("\n");
    }
}

int Mcts::chkVic(short board[][BOARDSIZE], Move mov1, Move mov2) {

#if DEBUG
    printf("\nDEBUG MODE: chkVic(): ===============\n");
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            printf("%d ", board[i][j]);
        printf("\n");
    }
#endif

    int count = 0;
    short color = board[mov1.y][mov1.x];

    //CHECK MV1

    //check vertical
    for (int x = mov1.y, y=mov1.x ; ; y++) {
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
            if (board[x][y] == board[x+1][y]) continue;
            else break;
        }
    }
    for (int x = mov1.y, y = mov1.x; ; x--) {
        if (board[x][y] == color) count++;
        if (x == 0) break;
        else {
            if (board[x][y] == board[x-1][y]) continue;
            else break;
        }
    }

    if (--count > 5) return color;
    count = 0;

    //check right-up diagonal
    for (int x = mov1.y, y = mov1.x; ; x++,y--) {
        if (board[x][y] == color) count++;
        if (x == 18 || y == 0) break;
        else {
            if (board[x][y] == board[x + 1][y-1]) continue;
            else break;
        }
    }
    for (int x = mov1.y, y = mov1.x; ; x--,y++) {
        if (board[x][y] == color) count++;
        if ( x == 0 || y == 18 ) break;
        else {
            if (board[x][y] == board[x - 1][y+1]) continue;
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
}

/*
void Mcts::placeStones(const Move stone, short int color, pNode checknode) {
    checknode->board[stone.y][stone.x] = color;
}
*/


pNode Mcts::returnMov(){
    float max = -1, value = 0;
    pNode toReturn = NULL, rootNode = this->root;
    vector<pNode> &iter = *(rootNode->children);
    for (pNode child : iter){
    //for(pNode child : rootNode->children){
        value = float(child->t) / float(child->n);
        if (max < value) {
            max = value;
            toReturn = child;
        }
    }
    return toReturn;
}

