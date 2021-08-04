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

bool comp(Move mov1, Move mov2);

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
        int** board;//[19][19];// = {0, };

        char name[10];
        Node(char const *name);
        Node(int curr_color, Node* myparent);
        void printName();
        void addC(Node* child);
        void printC();
        void printUCB();
        void printBoard();
        void printAvailMov();
        float getUCB();
        void setUCB(float num);

        void appendAvailMov(Move mov);
        void rmDupInAvailMov();

        void freeNode();
};


class Mcts
{
    private:

    public:
    Mcts();
    void runGame();
    void calcUCB(Node* node);
    void expansion(Node* currnode);
    void backprop();
    void select();

    void findMoves(Node& node);

    int rollout(Node* currnode);
    int ifBigger(int num);
    int ifSmaller(int num);
    void chkTime();
    int chkVic(Node& currnode);
    void placeStones(Move stone, int color, Node* checknode);
    bool isNotFull(Node* checknode);
};

