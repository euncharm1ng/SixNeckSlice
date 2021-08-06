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


Mcts::Mcts(){
    printf("Mcts constructor\n");
}

void Mcts::runGame(){}

void Mcts::select(pNode rootNode){
    //vector<pNode> parentNodes;
    time_t startTime = clock();
    float time = 0;
    int value = 0;
    int i = 0, j =0;

    this->expansion(rootNode);

    for(pNode child : rootNode->children){
        value = this->rollout(child);
        this->backprop(child, value);
        this->expansion(child);
    }
    rootNode->n += rootNode->children.size();
    printf("end of 1st expansion\n");
    pNode tempnode = NULL;
    do {
        tempnode = rootNode;
        tempnode = this->searchBigUCB(tempnode);
        while(!tempnode->children.empty()) tempnode = this->searchBigUCB(tempnode); //not empty
 
        value = this->rollout(tempnode);
        this->expansion(tempnode);
        tempnode = this->backprop(tempnode, value);

        //time_t endTime = clock();
        //time = (endTime - startTime) / double(CLOCKS_PER_SEC);
        i++;
        if(i == 100){
            printf("------------------------------------------------%d\n", j);
            i = 0;
            j++;
            time_t endTime = clock();
            time = (endTime - startTime) / double(CLOCKS_PER_SEC);
            if(time > 20) break;
        }
    }while(1);//time < 20);
    time_t endTime = clock();
    time = (endTime - startTime) / double(CLOCKS_PER_SEC);
    printf("time: %f\n", time);
    printf("%d runs\n", 5000*j);

    //pNode result = this->searchBigUCB(rootNode);
    pNode result = this->returnMov(rootNode);
    printf("result print board\n");
    result->printBoard();
}

void Mcts::expansion(pNode currnode) {
    this->findMoves(currnode);
    int child_color = (currnode->color == BLACK) ? WHITE : BLACK;
    int size = currnode->availMov.size();
    //printf("in expansion %d\n", size);
    Move mov1, mov2;
    for (int i = 0; i < size; i++) {
        mov1 = currnode->availMov[i];
        for (int j = i + 1; j < size; j++) {
            mov2 = currnode->availMov[j];
            pNode tempnode = new Node(child_color, currnode);
            this->placeStones(mov1, child_color, tempnode);
            this->placeStones(mov2, child_color, tempnode);
            currnode->addC(tempnode);
        }
    }
    currnode->availMov.clear();
    currnode->availMov.resize(0);
    currnode->availMov.shrink_to_fit();
}

// 1 : BLACK win, 2 : WHITE win
int Mcts::rollout(pNode currnode){
    int aicolor = currnode->color;
    int usercolor = (aicolor == BLACK) ? WHITE : BLACK;
    int turn = (aicolor == BLACK) ? WHITE : BLACK;
    int size = 0;
    Move mov1={0,0},mov2={0,0};
    pNode tempnode = new Node(1, currnode);
    srand(time(NULL));
    do {
        tempnode->availMov.clear();
        vector <Move>().swap(tempnode->availMov);
        this->findMoves(tempnode);
        size = tempnode->availMov.size();
        if(size == 0) break;
        turn = (turn == BLACK) ? WHITE : BLACK;
        mov1 = tempnode->availMov[rand()%(size)];
        do {
            mov2 = tempnode->availMov[rand()%(size)];
        }while(mov1.x == mov2.x && mov1.y == mov2.y);
        this->placeStones(mov1, turn, tempnode);
        this->placeStones(mov2, turn, tempnode);
        if(chkVic(tempnode)) {
            #if DEBUG
            tempnode->printBoard();
            #endif
            //currnode->setUCB(20);
            delete tempnode;
            return 20;
        }
        //if(chkVic(tempnode)==aicolor) return 20;
        //else if(return 5;
    }while(1);
    //tempnode->printBoard();
    //currnode->setUCB(10);

    delete tempnode;
    return 10;
}

//(update ucb, t of ancestor node for root)
// input :
pNode Mcts::backprop(pNode currNode, int value){//vector<Node> &parent, int updateT){
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

    for(pNode tempNode : parentNode->children) {
        if(tempNode->n ==0) return tempNode;
        chk = this->calcUCB(tempNode);
        if(chk > max) {
            max = chk;
            returnNode = tempNode;
        }
    }
    return returnNode;
    //node->ucb = node->t + 2*(sqrt(log(node->parent->n)/node->n));
}

void Mcts::findMoves(pNode node){
    unsigned short int tempBoard[19][19];

    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++){
            tempBoard[i][j] = 2;
        }
    }

    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
            if(node->board[i][j]){
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
                tempMove.x = i;
                tempMove.y = j;
                node->appendAvailMov(tempMove);
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

float Mcts::calcUCB(pNode node){
    node->ucb = (node->t / node->n) + 2*(sqrt(log(node->parent->n)/node->n));
    return node->ucb;
}

int Mcts::chkVic(pNode node){
    int color = 0, cnt = 0;
    #if DEBUG == 2
    printf("dir1---------\n");
    #endif
    // dir1 horizontal
    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
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

void Mcts::placeStones(const Move stone, int color, pNode checknode){
    checknode->board[stone.x][stone.y] = color;
}

pNode Mcts::returnMov(pNode rootNode){
    float max = -1, value = 0;
    pNode toReturn = NULL;
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