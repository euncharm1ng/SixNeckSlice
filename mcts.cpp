#include "mcts.h"
#include <stdlib.h>
#include <time.h>
#include <algorithm> //copy(&array[][],&array[][]+lengh,&array_copy[][])
//color,  usercolor, aicolor
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>
#include <string.h>

using namespace std;

#define DEBUG 0


bool comp(Move mov1, Move mov2){
    if(mov1.x < mov2.x) return true;
    else if(mov1.y < mov2.y) return true;
    else return false;
}

bool operator == (const Move &m1, const Move &m2)
{
   if(m1.x == m2.x && m1.y == m2.y)
     return true;
   else
     return false;
}


const int BLACK = 1;
const int WHITE = 2;
#define BOARDSIZE 19
#define MOVRANGE 2


//-----class Node-----//
Node::Node(int curr_color, Node* myparent){//char const *name){
    color = curr_color;
    parent = myparent;
    t = 0;
    n = 0;
    ucb = 0;
    vector<Node*> children;
    vector<Move> availMov;
    //strcpy(this->name, name);

    this->board = (int**)malloc(sizeof(int*) * BOARDSIZE);
    for(int i = 0; i< BOARDSIZE; i++){
        this->board[i] = (int*)malloc(sizeof(int) * BOARDSIZE);
    }
    // fill( &(this->board[0][0]), &(this->board[0][0]) + sizeof(this->board), 0);
    for(int i =0; i< BOARDSIZE; i++){
        for(int j =0; j< BOARDSIZE; j++){
            this->board[i][j] = 0;
        }
    }
}

void Node::freeNode(){
    for(int i = 0; i < BOARDSIZE; ++i) {
        free(this->board[i]);
    }
    free(this->board);

}

// Node::Node(int curr_color, Node* myparent){
//     color = curr_color;
//     parent = myparent;
// }

void Node::printName(){
    printf("from print name %s\n", this->name);
}

void Node::addC(Node* child){
    this->children.push_back(child);
}
void Node::printC(){
    #if DEBUG
    printf("--print child\n");
    #endif
    for(auto& c : this->children){
        printf(" %s\n", c->name);
    }
    #if DEBUG
    printf("--end of print child\n");
    #endif
}

void Node::printUCB(){
    printf("ucb: %f\n", this->ucb);
}

void Node::printBoard(){
    #if DEBUG
    printf("print board\n");
    #endif
    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
            printf("%d ",this->board[i][j]);
        }
        printf("\n");
    }
    #if DEBUG
    printf("end of print board\n");
    #endif
}

void Node::printAvailMov(){
    #if DEBUG
    printf("==print Avail Mov\n");
    #endif
    int i =0;
    for(auto& m : this->availMov){
        if(i++ % 5 == 0) printf("\n");
        printf("x: %3d, y: %3d \t", m.x, m.y);
    }
    #if DEBUG
    printf("==end of print Avail Mov\n");
    #endif
}

float Node::getUCB(){
    return this->ucb;
}

void Node::setUCB(float num){
    this->ucb = num;
}

void Node::appendAvailMov(Move mov){
    this->availMov.push_back(mov);
}

void Node::rmDupInAvailMov(){
    sort(this->availMov.begin(), this->availMov.end(), comp);
    // if (binary_search(this->availMov.begin(), this->availMov.end(), mov))
    //     printf("exists\n");
    // else 

    this->availMov.erase(unique(this->availMov.begin(), this->availMov.end()), this->availMov.end());
    this->printAvailMov();
}



//-----class Mcts-----//
Mcts::Mcts(){
    printf("Mcts constructor\n");
}
void Mcts::runGame(){
    printf("in rungame\n");
}
void Mcts::calcUCB(Node* node){
    node->ucb = node->t + 2*(sqrt(log(node->parent->n)/node->n));
}
void Mcts::expansion(Node* currnode){
    // this->findMoves(&currnode);
    int child_color = (currnode->color == BLACK) ? WHITE : BLACK;
    for(Move mov : currnode->availMov){
        Node tempnode = Node(child_color,currnode);
        this->placeStones(mov,child_color,&tempnode);
        currnode->addC(&tempnode);
    }
}
void Mcts::backprop(){

}
void Mcts::select(){

}
void Mcts::findMoves(Node& node){
    int tempBoard[19][19];
    //fill( &(tempBoard[0][0]), &(tempBoard[0][0]) + sizeof(tempBoard), 1 );

    for(int i =0; i< 19; i++){
        for(int j =0; j < 19; j++){
            tempBoard[i][j] = 1;
        }
    }

    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
            if(node.board[i][j]){
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
                node.appendAvailMov(tempMove);
            }
        }
        #if DEBUG
        printf("\n");
        #endif
    }
    #if DEBUG
    node.printAvailMov();
    #endif
}


// 1 : BLACK win, 2 : WHITE win
int Mcts::rollout(Node* currnode){
    int aicolor = currnode->color;
    int usercolor = (aicolor == BLACK) ? WHITE : BLACK;
    int turn=0;
    Node tempnode = Node(5, NULL);  
    while( this->isNotFull(&tempnode) ) {
        tempnode.availMov.clear();
        this->findMoves(tempnode);
        srand(time(NULL));
        turn = (aicolor == BLACK) ? WHITE : BLACK;
        this->placeStones( currnode->availMov[rand()%currnode->availMov.size()] ,turn,&tempnode);
        this->placeStones( currnode->availMov[rand()%currnode->availMov.size()] ,turn,&tempnode);
        if(chkVic(tempnode)==aicolor) return 20;
        else return 5;
    }
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


int Mcts::chkVic(Node& node){
    int color = 0, cnt = 0;
    #if DEBUG
    printf("dir1---------\n");
    #endif
    // dir1 horizontal
    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
            if (node.board[i][j]){
                color = node.board[i][j];
                while(j < 19 && color == node.board[i][j]){
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
            if(node.board[i][j]){
                color = node.board[i][j];
                while(i<19 && color == node.board[i][j]){
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
            if(node.board[i][j]){
                color = node.board[i][j];
                while(i < 19 && color == node.board[i][j]){
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
            if(node.board[i][j]){
                color = node.board[i][j];
                while(j < 19 && color == node.board[i][j]){
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
            if(node.board[i][j]){
                color = node.board[i][j];
                while(j > -1 && color == node.board[i][j]){
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
            if(node.board[i][j]){
                color = node.board[i][j];
                while(i < 19 && color == node.board[i][j]){
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

bool Mcts::isNotFull(Node* checknode){
    int* data = &checknode->board[0][0];
    if(find(data,data+361,0)==data+361) return false;
    else return true;
}

