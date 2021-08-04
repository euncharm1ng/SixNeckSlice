#include <stdlib.h>
#include <algorithm> //copy(&array[][],&array[][]+lengh,&array_copy[][])
//color,  usercolor, aicolor
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include "mcts.h"
using namespace std;

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

void Node::appendAvailMov(Move mov){
    sort(this->availMov.begin(), this->availMov.end(), comp);
    this->printAvailMov();
}

bool comp(Move mov1, Move mov2){
    if(mov1.x < mov2.x) return true;
    else if(mov1.y < mov1.y) return true;
    else return false;
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
    int tempBoard[19][19] = {1, };
    for(int i = 0; i < 19; i++){
        for(int j = 0; j < 19; i++){
            if(node.board[i][j]){
                tempBoard[i][j] = 0;
                int ksmall = ifSmaller(i - MOVRANGE);
                int kbig = ifBigger(i + MOVRANGE);
                int lsmall = ifSmaller(j - MOVRANGE);
                int lbig = ifBigger(j + MOVRANGE);
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
            if(tempBoard[i][j] > 1){
                
            }
        }
    }
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

