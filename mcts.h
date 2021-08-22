#include <stdlib.h> 
#include <algorithm> 
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>

#define INFI 3e10
#define BOARDSIZE 19
#define WHITE 2
#define BLACK 1
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"
#define NORM "\033[37m"

using namespace std;

typedef struct Move {
    short x, y;
}Move;
typedef Move* pMove;
bool comp(Move mov1, Move mov2);

typedef struct Node {
    short color, moveSize; //the color to place to the board that yet not placed
    int n;
    float mean, t, prevMax;
    vector<Node*> *children;
    Node* parent, *prevSel;
    pMove movesLog;
} Node;
typedef Node* pNode;

pNode createNode(short paraColor);
pNode createNode(short paraColor, pNode paraParent, Move mov1, Move mov2);
void freeNode(pNode paraNode);
void addC(pNode paraParent, pNode paraChild);

class Mcts {
    public:
        short** board;
        pNode root;
        short aiColor;
        Mcts();
        Mcts(short** paraBoard, short paraAiColor);

        pNode runGame();
        pNode select(pNode parentNode);
        void expansion(pNode currnode);
        float rollout(pNode currnode);
        void backprop(pNode currNode, float value);

        void setRoot(short paraAiColor);
        void findMoves(pNode currNode, vector<Move>& oneGridAway, vector<Move>& availMoves);
        int chkVic(short** board, Move mov1, Move mov2);
        pNode returnMov();
        void printAvailMoves(vector<Move> availMov);
        void findMovesOneGrid(short board[][BOARDSIZE], vector<Move>& moveVec, int tagToAvoid);
};

void initGUI ();

void aiPlayFirst ();

int chkBoundary (Move inputMove);

void userPlayFirst ();

int placeStone (Move inputMove, short inputColor);

void receiveUserInput ();

void aiPlays () ;

void runGUI();

void freeAll (pNode currnode) ;