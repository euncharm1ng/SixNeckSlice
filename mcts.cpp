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
//#include <limits>


#define DEBUG 0 //to 1 to run in debug mode
#define BLACK 1
#define WHITE 2
#define BOARDSIZE 19
#define MOVRANGE 2
#define TIMELIMIT 20
#define INFI 3e10

using namespace std;
//int a = numeric_limits<int>::max();

/*
todo:
    top-down: selection (update n of all passing nodes )
        calc ucb every time
        remove parent node
    bottom-up: backpropagation (update ucb, t of ancestor node)
    rungame (selction -> expansion -> rollout -> backprop) for TIMELIMIT == 20 sec
*/







//-----class Mcts-----//
Mcts::Mcts(){
    printf("Mcts constructor\n");
}

void Mcts::runGame(){    
}
float Mcts::calcUCB(Node* node){
    node->ucb = node->t + 2*(sqrt(log(node->parent->n)/node->n));
    return node->ucb;
}

void Mcts::expansion(Node* currnode) {
    this->findMoves(currnode);
    int child_color = (currnode->color == BLACK) ? WHITE : BLACK;
    int size = currnode->availMov.size();
    printf("in expansion, size: %d\n", size);
    currnode->printAvailMov();
    Move mov1, mov2;
    for (int i = 0; i < size; i++) {
        mov1 = currnode->availMov[i];
        for (int j = i + 1; j < size; j++) {
            mov2 = currnode->availMov[j];
            Node* tempnode = new Node(child_color, currnode);
            this->placeStones(mov1, child_color, tempnode);
            this->placeStones(mov2, child_color, tempnode);
            currnode->addC(tempnode);
        }
    }
    currnode->availMov.clear();
    currnode->availMov.resize(0);
    currnode->availMov.shrink_to_fit();
}

//(update ucb, t of ancestor node for root)
// input : 

Node* Mcts::backprop(Node* currNode, int value){//vector<Node> &parent, int updateT){
    while(currNode->parent != NULL){
        currNode->t += value;
        currNode = currNode->parent;
    }
    return currNode;
}


void Mcts::select(Node* rootNode){
    //vector<Node*> parentNodes;
    time_t startTime = clock();
    float time = 0;
    int value = 0;
    int i =0;

    this->expansion(rootNode);
    for(int i =0; i< 10; i++){
        Node* child = rootNode->children[i];
        value = this->rollout(child);
        this->backprop(child, value);
        this->expansion(child);
    }
    // for(Node* child : rootNode->children){
    //     value = this->rollout(child);
    //     this->backprop(child, value);
    //     this->expansion(child);
    // }
    rootNode->n += rootNode->children.size();
    printf("in selection, root: %d\n", rootNode->n);
    Node* mytempnode = this->searchBigUCB(rootNode);
    printf("t= %d, ucb = %f, n = %d\n", mytempnode->t, mytempnode->ucb, mytempnode->n);
    
    Node* tempnode = NULL;
    do {
        printf("in select: %d\n", 1);

        tempnode = rootNode;
        tempnode = this->searchBigUCB(tempnode);
        printf("in do while: t= %d, ucb = %f, n = %d\n", tempnode->t, tempnode->ucb, tempnode->n);
        while(!tempnode->children.empty()) tempnode = this->searchBigUCB(tempnode); //not empty

        
        value = this->rollout(tempnode);
        this->expansion(tempnode);
        tempnode = this->backprop(tempnode, value);


        // if(tempnode->children.empty()) {
        //     this->expansion(tempnode);
        //     tempnode = this->searchBigUCB(tempnode);
        //     value = this->rollout(tempnode);
        //     tempnode = this->backprop(tempnode, value);
        // }
        // else {
        //     tempnode = this->isChildsUCB(tempnode);
        //     if(tempnode->ucb != INFI) continue;           
        //     value = this->rollout(tempnode);
        //     tempnode = this->backprop(tempnode,value);
        // }

        time_t endTime = clock();
        time = (endTime - startTime) / double(CLOCKS_PER_SEC);
        i++;
    }while(time < 20);
    printf("%d runs\n", i);
    Node* result = this->searchBigUCB(rootNode);
    printf("result print board\n");
    result->printBoard();
}
/*
select: (currnode = currnode.biggestchild)
    check for child.empty()
        if empty - expansion ->> rollout random ->> backprop
        if not empty - choose biggest child ->> jump to select
*/
Node* Mcts::searchBigUCB(Node* parentNode){
    float max = -100, chk=0;
    Node* returnNode = NULL;

    for(Node* tempNode : parentNode->children) {
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

void Mcts::findMoves(Node* node){
    int tempBoard[19][19];

    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++){
            tempBoard[i][j] = 1;
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
                        tempBoard[k][l] *= 2;
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
            if(tempBoard[i][j] > 1){
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


// 1 : BLACK win, 2 : WHITE win
int Mcts::rollout(Node* currnode){
    int aicolor = currnode->color;
    int usercolor = (aicolor == BLACK) ? WHITE : BLACK;
    int turn = (aicolor == BLACK) ? WHITE : BLACK;
    int size = 0;
    Move mov1={0,0},mov2={0,0};
    Node* tempnode = new Node(1, currnode);
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
    tempnode->printBoard();
    //currnode->setUCB(10);

    delete tempnode;
    return 10;
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


int Mcts::chkVic(Node* node){
    int color = 0, cnt = 0;
    #if DEBUG
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
                #if DEBUG
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG 
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
    #if DEBUG
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
                #if DEBUG
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG 
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
    #if DEBUG
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
            #if DEBUG
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
            #endif
                if(cnt > 5){ 
                    #if DEBUG 
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
                #if DEBUG
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG 
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
    #if DEBUG
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
                #if DEBUG
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG 
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
                #if DEBUG
                printf("outside while, cnt = %d, i = %d, j = %d\n", cnt, i, j);
                #endif
                if(cnt > 5){ 
                    #if DEBUG 
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
    }
    #if DEBUG
    printf("end of chkvic()\n");
    #endif
    return 0;
}// end of chkVic()

void Mcts::placeStones(const Move stone, int color, Node* checknode){
    checknode->board[stone.x][stone.y] = color;
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