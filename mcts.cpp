#include <stdlib.h>
#include <time.h>
#include <algorithm> //copy(&array[][],&array[][]+lengh,&array_copy[][])
//color,  usercolor, aicolor
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include "mcts.h"
using namespace std;

int movRange = 2;
const int BLACK = 1;
const int WHITE = 2;
#define BOARDSIZE 19
#define MOVRANGE 2

//-----class Node-----//
Node::Node(char const *name){
    t = 0;
    n = 0;
    ucb = 0;
    color = 0;
    vector<Node*> children;
    vector<Move> availMov;
    strcpy(this->name, name);

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

Node::Node(int curr_color, Node* myparent){
    color = curr_color;
    parent = myparent;
}

void Node::printName(){
    printf("from print name %s\n", this->name);
}

void Node::addC(Node* child){
    this->children.push_back(child);
}
void Node::printC(){
    for(auto& c : this->children){
        printf("print ccccc : %s\n", c->name);
    }
    // for(int i =0; i < this->children.length(); i++){
    //     printf("%d %s\n", i, this->children[i].name);
    // }
}

void Node::printUCB(){
    printf("ucb: %f\n", this->ucb);
}

void Node::printBoard(){
    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; j++){
            printf("%d ",this->board[i][j]);
        }
        printf("\n");
    }
    printf("end of print board\n");
}

void Node::printAvailMov(){
    printf("indise print avail mov\n");

    for(auto& m : this->availMov){
        printf("x: %d, y: %d", m.x, m.y);
    }
}

float Node::getUCB(){
    return this->ucb;
}

void Node::setUCB(float num){
    this->ucb = num;
}



//-----class Mcts-----//
Mcts::Mcts(){
    printf("hello world\n");
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
// 1 : BLACK win, 2 : WHITE win
int Mcts::rollout(Node* currnode){
    int aicolor = currnode->color;
    int usercolor = (aicolor == BLACK) ? WHITE : BLACK;
    int turn=0;
    Node tempnode = Node("temp");  
    while( this->isNotFull(&tempnode) ) {
        tempnode.availMov.clear();
        this->findMoves(&tempnode);
        srand(time(NULL));
        turn = (aicolor == BLACK) ? WHITE : BLACK;
        this->placeStones( currnode->availMov[rand()%currnode->availMov.size()] ,turn,&tempnode);
        this->placeStones( currnode->availMov[rand()%currnode->availMov.size()] ,turn,&tempnode);
        if(chkVic(&tempnode)==aicolor) return 20;
        else return 5;
    }
    return 10;
}

void Mcts::findMoves(Node* node){
    // int tempBoard[19][19] = {1, };
    // for(int i = 0; i < 19; i++){
    //     for(int j = 0; j < 19; i++){
    //         if(node.board[i][j]){
    //             tempBoard[i][j] = 0;
    //             int ksmall = ifSmaller(i - movRange);
    //             int kbig = ifBigger(i + movRange);
    //             int lsmall = ifSmaller(j - movRange);
    //             int lbig = ifBigger(j + movRange);
    //             for(int k = ksmall; k < kbig; k++){
    //                 for(int l = lsmall; l < lbig; l++){
    //                     tempBoard[k][l] *= 2;
    //                 }
    //             }
    //         }
    //     }
    // }

    // for(int i = 0; i < 19; i++){
    //     for(int j = 0; j < 19; j++){
    //         if(tempBoard[i][j] > 1){
                
    //         }
    //     }
    // }
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
int Mcts::chkVic(Node* currnode){
    // int color = 0, cnt = 0;
    // for(int i = 0; i < 19; i++){
    //     for(int j = 0; j < 19; j++){
    //         if node->
    //     }
    // }
}
void Mcts::placeStones(const Move stone, int color, Node* checknode){
    checknode->board[stone.x][stone.y] = color;
}

bool Mcts::isNotFull(Node* checknode){
    int* data = &checknode->board[0][0];
    if(find(data,data+361,0)==data+361) return false;
    else return true;
}

