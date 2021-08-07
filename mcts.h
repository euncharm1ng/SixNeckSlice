/*
TODO: free all nodes when result node is decided.
*/


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
    //vector<Move> availMov;
    Node* parent;
    pMove movesLog;
} Node;
typedef Node* pNode;

pNode createNode(short paraColor);

pNode createNode(short paraColor, pNode paraParent, Move mov1, Move mov2);

void freeNode(pNode paraNode);

void addC(pNode paraParent, pNode paraChild);

void appendAvailMove(pNode paraParent, Move mov);

class Mcts{
    public:
    short** board;
    pNode root;
    short aiColor;

    Mcts(short **paraBoard, short paraAiColor);

    float calcUCB(pNode node);
    void expansion(pNode currnode);
    pNode backprop(pNode currNode, short value);
    pNode runGame();
    void findMoves(pNode node, vector<Move> &availMoves);
    int rollout(pNode currnode);
    int ifBigger(int num);
    int ifSmaller(int num);
    void chkTime();
    int chkVic(short board[][BOARDSIZE], Move mov1, Move mov2);
    void placeStones(const Move stone, int color, pNode checknode);
    bool isNotFull(pNode checknode);
    pNode select(pNode parentNode);
    pNode returnMov();
    void printAvailMoves(vector<Move> availMov);
};
