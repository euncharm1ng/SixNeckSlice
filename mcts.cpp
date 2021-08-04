#include "mcts.h"
#include <stdlib.h>
#include <algorithm> //copy(&array[][],&array[][]+lengh,&array_copy[][])
//color,  usercolor, aicolor
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>
#include <string.h>

using namespace std;

#define MOVRANGE 2
#define BOARDSIZE 19


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
    printf("print Avail Mov");
    int i =0;
    for(auto& m : this->availMov){
        if(i++ % 5 == 0) printf("\n");
        printf("x: %3d, y: %3d \t", m.x, m.y);
    }
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
    printf("hello world\n");
}
void Mcts::runGame(){
    printf("in rungame\n");
}
void Mcts::calcUCB(Node* node){
    node->ucb = node->t + 2*(sqrt(log(node->parent->n)/node->n));
}
void Mcts::expansion(){

}
void Mcts::backprop(){

}
void Mcts::select(){

}
void Mcts::rollout(){
    printf("rollout\n");
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
            printf("%2d ", tempBoard[i][j]);
            if(tempBoard[i][j] > 1){
                Move tempMove;
                tempMove.x = i;
                tempMove.y = j;
                node.appendAvailMov(tempMove);
            }
        }
        printf("\n");
    }
    node.printAvailMov();
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

void Mcts::chkVic(){
    // int color = 0, cnt = 0;
    // for(int i = 0; i < 19; i++){
    //     for(int j = 0; j < 19; j++){
    //         if node->
    //     }
    // }
}
void Mcts::placeStones(const Move stone, int color, Node& checknode){
    checknode.board[stone.x][stone.y] = color;
}
void Mcts::isFull(Node& checknode){
    
}

