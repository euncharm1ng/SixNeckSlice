#include <stdlib.h> 
#include <algorithm> 
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>
#define INFI 3e10
#define BOARDSIZE 19

using namespace std;

typedef struct Move {
    short int x, y;
}Move;
typedef Move* pMove;
bool comp(Move mov1, Move mov2);

typedef struct Node{
    short t, n, color, moveSize; //the color to place to the board that yet not placed
    float ucb;
    vector<Node*> *children;
    Node* parent;
    pMove movesLog;
} Node;
typedef Node* pNode;

pNode createNode(short paraColor);
pNode createNode(short paraColor, pNode paraParent, Move mov1, Move mov2);
void freeNode(pNode paraNode);
void addC(pNode paraParent, pNode paraChild);

class Mcts{
    public:
    short** board;
    pNode root;
    short aiColor;

    Mcts(short **paraBoard, short paraAiColor);

    pNode runGame();
    pNode select(pNode parentNode);
    void expansion(pNode currnode);
    int rollout(pNode currnode);
    pNode backprop(pNode currNode, short value);
    
    void findMoves(pNode currNode, vector<Move> &oneGridAway, vector<Move> &availMoves);
    int chkVic(short board[][BOARDSIZE], Move mov1, Move mov2);
    pNode returnMov();
    void printAvailMoves(vector<Move> availMov);
    void findMovesOneGrid(short board[][BOARDSIZE], vector<Move> &moveVec, int tagToAvoid);
};
