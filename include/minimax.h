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
    //inline bool operator==(const Move lhs);
}Move;
typedef Move* pMove;

typedef struct Node {
    Move mv1, mv2;
} Node;
typedef Node* pNode;


class Mini{
    short** board;
    pNode root;
    short home_color;

    Mini();
    Mini(short paraAiColor);

    int expansion();
    pNode expand_root();
    void mv1_top(pMove mvs, int legnth, int color);
    void mv2_top(pMove mvs, int length, int color);

    //void expandChild(pNode currNode); // make children from top

    

    pNode run_mini(Move away_move1, Move away_move2);
    
    //void setRoot(short paraAiColor);
    
    // int evalRoot(short** board);

    int evalOneRow(short* type, short* count);

    /* evaluate the node's board with accumulated val*/
    // int evalAccum1(short board[][BOARDSIZE], Move mov, short inputStone);

    /*
    * checks the board according to the mov1 and mov2, return 1 or 2 if 
    * the user with the color wins, else return 0 for no winner found.
    */
    //int chkVic(short** board,Move mov1, Move mov2);
};