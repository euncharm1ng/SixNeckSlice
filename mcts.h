#include <stdlib.h>
#include <algorithm> //copy(&array[][],&array[][]+lengh,&array_copy[][])
//color,  usercolor, aicolor
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>

using namespace std;

typedef struct move{
    int x, y;
}Move;

class Node{
    private:
        // int t;
        // int n;
        // float ucb;
        // int color;
        // vector<Node*> children;
        // vector<Move> availMov;
    public:
        int t;
        int n;
        float ucb;
        int color;
        vector<Node*> children;
        vector<Move> availMov;
        Node* parent;
        int board[19][19] = {0, };
        char name[10];
        Node(char const *name);
        void printName();
        void addC(Node* child);
        void printC();
        void printUCB();
        void printBoard();
        void printAvailMov();
        float getUCB();
        void setUCB(float num);
};


class Mcts
{
    private:

    public:
    Mcts();
    void runGame();
    void calcUCB(Node* node);
    void expansion();
    void backprop();
    void select();
    void rollout();
    void findMoves();
    void ifBigger();
    void ifSmaller();
    void chkTime();
    void chkVic();
    void placeStones(const Move stone, int color, Node& checknode);
    void isFull(Node& checknode);
};

