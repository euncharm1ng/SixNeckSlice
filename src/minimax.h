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
#define EMPTY 0
#define OBSTACLE 3

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
    inline bool operator==(const Move lhs);
}Move;
typedef Move* pMove;

typedef struct Node {
    short color, moveSize; //the color to place to the board that yet not placed
    int value;
    vector<Node*> *children;
    Node* parent;
    pMove movesLog;
} Node;
typedef Node* pNode;


pNode createNode(short paraColor);
pNode createNode(short paraColor, pNode paraParent, Move mov1, Move mov2, int toAccumulate);
void* freeAll (void* inputNode);
void freeNode(pNode paraNode);

class Mini{
    void expansion(pNode currnode);

    void findMoves(pNode currNode, vector<Move>& oneGridAway, vector<Move>& availMoves, short board[][BOARDSIZE]);
    
    void findMovesOneGrid(short board[][BOARDSIZE], vector<Move>& moveVec, int tagToAvoid);
    
    void expandChild(pNode currNode);

    pNode select(pNode root);
    
    bool chkPossible(short** board, Move mov1);

    public:
        short** board;
        pNode root;
        short aiColor;
        
        Mini();

        Mini(short** paraBoard, short paraAiColor);

        pNode runGame(Move userMov1, Move userMov2);
        
        void setRoot(short paraAiColor);
        
        int evalRoot(short** board);

        int evalOneRow(short* type, short* count);

        /* evaluate the node's board with accumulated val*/
        int evalAccum1(short board[][BOARDSIZE], Move mov, short inputStone);

        /*
        * checks the board according to the mov1 and mov2, return 1 or 2 if 
        * the user with the color wins, else return 0 for no winner found.
        */
        int chkVic(short** board,Move mov1, Move mov2);
};


/*---------- gui.cpp ----------*/

void initGUI();

void aiPlayFirst();

/* 
* return 1 for valid input, 0 for invalid input and prints an error message 
*/
int chkBoundary(Move inputMove);

void userPlayFirst();

/*
* return 1 on success, return -1 for placing stone in occupied location
* the boundary (-1 < x < 19) has to be checked first
*/
int placeStone (Move inputMove, short inputColor);

/* return 1 if user wins, 0 for continue the game */
int receiveUserInput ();

void printBoard();

/* ai makes its move; run and return result from chkVin() from mcts.c */
int aiPlays() ;

void runGUI();

void setRedStone();

void* runDahunGUI(void*);