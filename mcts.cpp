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
    return newNode;
}

void freeNode(pNode paraNode){
    free(paraNode->movesLog);
    paraNode->children.clear();
    paraNode->children.shrink_to_fit();
    paraNode->availMov.clear();
    paraNode->availMov.shrink_to_fit();
    free(paraNode);
}

void addC(pNode paraParent, pNode paraChild){
    paraParent -> children.push_back(paraChild);
}

void appendAvailMove(pNode paraParent, Move mov){
    paraParent->availMov.push_back(mov);
}

Mcts::Mcts(short **paraBoard, short paraAiColor){
    printf("Mcts constructor\n");
    this->board = paraBoard;
    this->root = createNode(paraAiColor);
}



void Mcts::select(){
    time_t startTime = clock();
    float time = 0;
    short value = 0;
    int i = 0, j =0;

    this->expansion(this->root);
    printf("end of 1st expansion with size: %d\n", this->root->children.size());

    for(pNode child : this->root->children){
        value = this->rollout(child);
        this->backprop(child, value);
        this->expansion(child);
    }
    printf("end of 2nd expansion\n");

    pNode tempnode = NULL;
    do {
        tempnode = this->root;
        do{
            tempnode = this->searchBigUCB(tempnode);
        }while(!tempnode->children.empty()); 
 
        value = this->rollout(tempnode);
        this->backprop(tempnode, value);
        //this->expansion(tempnode);

        i++;
        if(i == 100){
            printf("------------------------------------------------%d\n", j);
            i = 0;
            j++;
            // time_t endTime = clock();
            // time = (endTime - startTime) / double(CLOCKS_PER_SEC);
            // if(time > 20) break;
        }
    }while(j<100);//time < 20);
    time_t endTime = clock();
    time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("time: %f\n", time);
    printf("%d runs\n", 5000*j);

    //pNode result = this->searchBigUCB(rootNode);
    pNode result = this->returnMov();
    printf("result print board\n");
    //result->printBoard();
}

void Mcts::expansion(pNode currNode) {
    int child_color = (currNode->color == BLACK) ? WHITE : BLACK;
    int size = currNode->availMov.size();
    Move mov1, mov2;

    this->findMoves(currNode);
    for (int i = 0; i < size; i++) {
        mov1 = currNode->availMov[i];
        for (int j = i + 1; j < size; j++) {
            mov2 = currNode->availMov[j];
            pNode tempNode = createNode(child_color, currNode, mov1, mov2);
            //this->placeStones(mov1, child_color, tempnode);
            //this->placeStones(mov2, child_color, tempnode);
            //currnode->addC(tempnode);
            addC(currNode, tempNode);
        }
    }
    currNode->availMov.clear();
    currNode->availMov.resize(0);
    currNode->availMov.shrink_to_fit();
}

// 1 : BLACK win, 2 : WHITE win
int Mcts::rollout(pNode currNode){
    //int aicolor = this->aiColor;
    //int usercolor = (aicolor == BLACK) ? WHITE : BLACK;
    int turn = this->aiColor;//(currNode->color == BLACK) ? BLACK : WHITE;
    int size = 0;
    vector<Move> availMoves;
    Move mov1, mov2;

    srand(time(NULL));

    short boardToRoll[BOARDSIZE][BOARDSIZE];
    short boardToChkMove[BOARDSIZE][BOARDSIZE];

    // board to roll: 
    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++)
            board[i][j] = this->board[i][j];
    }
    int k = currNode->moveSize;
    for(int i =0; i< k; i+=2){ //TODO: gotta handle the case with black place 1 stone only
        board[currNode->movesLog[i].y][currNode->movesLog[i].x] = turn;    
        board[currNode->movesLog[i+1].y][currNode->movesLog[i+1].x] = turn; 
        turn = (turn == BLACK) ? WHITE : BLACK; 
    }

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

        //chkVic();

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


    // this->findMoves(currNode);
    // mov1 = currNode->availMov[rand()%(size)];
    // do {
    //     mov2 = currNode->availMov[rand()%(size)];
    // }while(mov1.x == mov2.x && mov1.y == mov2.y);
    
    // //pNode tempnode = createNode(currnode->color, currnode, )//new Node(1, currnode);
    // pNode tempNode = createNode(currNode->color);
    // do {
    //     tempNode->availMov.clear();
    //     vector <Move>().swap(tempnode->availMov);
    //     this->findMoves(tempnode);
    //     size = tempnode->availMov.size();
    //     if(size == 0) break;
    //     turn = (turn == BLACK) ? WHITE : BLACK;
    //     mov1 = tempnode->availMov[rand()%(size)];
    //     do {
    //         mov2 = tempnode->availMov[rand()%(size)];
    //     }while(mov1.x == mov2.x && mov1.y == mov2.y);
    //     this->placeStones(mov1, turn, tempnode);
    //     this->placeStones(mov2, turn, tempnode);
    //     if(chkVic(tempnode)) {
    //         #if DEBUG
    //         tempnode->printBoard();
    //         #endif
    //         //currnode->setUCB(20);
    //         delete tempnode;
    //         return 20;
    //     }
    //     //if(chkVic(tempnode)==aicolor) return 20;
    //     //else if(return 5;
    // }while(1);
    // //tempnode->printBoard();
    // //currnode->setUCB(10);

    // delete tempnode;
    // return 10;
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

pNode Mcts::searchBigUCB(pNode parentNode){
    float max = -100, chk=0;
    pNode returnNode = NULL;
    short parentN = parentNode->n;

    random_shuffle(parentNode->children.begin(), parentNode->children.end());

    for(pNode tempNode : parentNode->children) {
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

void Mcts::findMoves(pNode currNode){
    short tempBoard[BOARDSIZE][BOARDSIZE];
    short board[BOARDSIZE][BOARDSIZE];

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
                for(int k = ksmall; k < kbig; k++){
                    for(int l = lsmall; l < lbig; l++){
                        tempBoard[k][l] *= 1.5;
                    }
                }
            }
        }
    }

    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
            #if DEBUG
            printf("%2d ", tempBoard[i][j]);
            #endif
            if(tempBoard[i][j] > 2){
                Move tempMove;
                tempMove.x = (short) j;
                tempMove.y = (short) i;
                appendAvailMove(currNode, tempMove);
            }
        }
        #if DEBUG
        printf("\n");
        #endif
    }
    #if DEBUG
    node->printAvailMov();
    #endif
}

int Mcts::ifBigger(int num){
    if(num > 19) return 19;
    else return num;
}

int Mcts::ifSmaller(int num){
    if(num < 0) return 0;
    else return num;
}

void Mcts::chkTime(){

}


/*
int Mcts::chkVic(pNode node){
    int color = 0, cnt = 0;
    #if DEBUG == 2
    printf("dir1---------\n");
    #endif
    // dir1 horizontal
    for(short i = 0; i < 19; i++){
        for(short j = 0; j < 19; j++){
            if (node->board[i][j]){
                color = node->board[i][j];
                while(j < 19 && color == node->board[i][j]){
                    cnt++;
                    j++;
                }
                #if DEBUG == 2
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG == 2
                    printf("found winner\n"); 
                    #endif 
                    return color;}
                else{
                    cnt = 0; 
                    j--;
                }
            }
        }
    } // enf of dir1
    #if DEBUG == 2
    printf("dir2---------\n");
    #endif
    // dir2 vertical
    for(int j = 0; j< 19; j++){
        for(int i =0; i < 19; i++){
            if(node->board[i][j]){
                color = node->board[i][j];
                while(i<19 && color == node->board[i][j]){
                    cnt++;
                    i++;
                }
                #if DEBUG == 2
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG == 2
                    printf("found winner\n"); 
                    #endif 
                    return color;}
                else{
                    cnt = 0;
                    i--;
                }
            }
        }
    }// end of dir2
    #if DEBUG == 2
    printf("dir3---------\n");
    #endif
    // dir3 diag top left to bot right
    for(int k = 0; k < 14; k++){
        int i = k, j = 0;
        while(i < 19){
            if(node->board[i][j]){
                color = node->board[i][j];
                while(i < 19 && color == node->board[i][j]){
                    cnt++;
                    i++;
                    j++;
                }
            #if DEBUG == 2
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
            #endif
                if(cnt > 5){ 
                    #if DEBUG == 2
                    printf("found winner\n"); 
                    #endif 
                    return color;}
                else{
                    cnt = 0;
                    continue;
                }
            }
            j++;
            i++;
        }
    }// end of dir3 pt1

    for(int k = 1; k < 14; k++){
        int j = k, i = 0;
        while(j < 19){
            if(node->board[i][j]){
                color = node->board[i][j];
                while(j < 19 && color == node->board[i][j]){
                    cnt++;
                    i++;
                    j++;
                }
                #if DEBUG == 2
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG == 2
                    printf("found winner\n"); 
                    #endif 
                    return color;}
                else{
                    cnt = 0;
                    continue;
                }
            }
            j++;
            i++;
        }
    }// end of dir3 pt2
    #if DEBUG == 2
    printf("dir4---------\n");
    #endif
    // dir4 diag top right to bot left
    for(int k = 5; k < 19; k++){
        int j = k, i = 0;
        while(j > -1){
            if(node->board[i][j]){
                color = node->board[i][j];
                while(j > -1 && color == node->board[i][j]){
                    cnt++;
                    i++;
                    j--;
                }
                #if DEBUG == 2
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG == 2
                    printf("found winner\n"); 
                    #endif 
                    return color;}
                else{
                    cnt = 0;
                    continue;
                }
            }
            i++;
            j--;
        }
    }// end of dir4 pt1
    for(int k =1; k < 14; k++){
        int i = k, j = 18;
        while(i < 19){
            if(node->board[i][j]){
                color = node->board[i][j];
                while(i < 19 && color == node->board[i][j]){
                    cnt++;
                    i++;
                    j--;
                }
                #if DEBUG == 2
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG == 2
                    printf("found winner\n"); 
                    #endif 
                    return color;}
                else{
                    cnt = 0;
                    continue;
                }
            }
            i++;
            j--;
        }
    }// end of dir4 pt2
    #if DEBUG == 2
    printf("end of chkvic()\n");
    #endif
    return 0;
}// end of chkVic()
*/
/*
void Mcts::placeStones(const Move stone, int color, pNode checknode){
    checknode->board[stone.x][stone.y] = color;
}
*/


pNode Mcts::returnMov(){
    float max = -1, value = 0;
    pNode toReturn = NULL, rootNode = this->root;
    for(pNode child : rootNode->children){
        value = float(child->t) / float(child->n);
        if(max < value){
            max = value;
            toReturn = child;
        }
    }
    return toReturn;
}



/*
expand root and rollout all child
 1:
//우리가 rollout 하는 node는 expansion이 된다. 
select: (currnode = currnode.biggestchild)
    check for child.empty()
        if empty - expansion ->> rollout random ->> backprop
        if not empty - choose biggest child ->> jump to select

2:
//모든 node가 expansion이 되지 않는다. 
select: (currnode = currnode.biggestchild)
    check for child.empty()
        if empty - expansion ->> rollout random ->> backprop
        if not empty - choose biggest child ->
            if biggest child's ucb == INFI - rollout child ->> backprop
            else - jump to select jump to select
*/

//자식 노드들 중 제일 큰 거 선택 
//if(child가 없다){
//      expansion()
//      child node 들 중 하나 rollout & backprop
//      올라가기
// }
//else(있다) {
//     if(모든 ucb 값들이 무한이 아니다){
//              ucb 값 들 중 큰 거 선택
//     }
//     else {
//      ucb 값 들 중 무한인 거 rollout & backprop
//      올라가기
//     }
// }
