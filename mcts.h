#include <stdlib.h> 
#include <algorithm> 
#include <vector>
#include <string>
#include <stdio.h>
#include <cmath>

using namespace std;

typedef struct Move{
    int x, y;
}Move;

// typedef struct Node{
//     int t, n, color;
//     float ucb;
//     vector<pNode>
// }

bool comp(Move mov1, Move mov2);


class Node{
    public:
        int t, n, color;
        float ucb;
        vector<Node*> children;
        vector<Move> availMov;
        Node* parent;
        int** board;
        Move mov1, mov2;

        Node(int curr_color, Node* myparent);//, Move mov1, Move mov2);
        Node(int curr_color);

        void addC(Node* child);
        void printC();
        void printUCB();
        void printBoard();
        void printAvailMov();
        float getUCB();
        void setUCB(float num);
        void appendAvailMov(Move mov);
        void freeNode();
};
typedef Node* pNode;



class Mcts{
    public:
    Mcts();

    void runGame();
    float calcUCB(pNode node);
    void expansion(pNode currnode);
    pNode backprop(pNode currNode, int value);
    void select(pNode rootNode);
    void findMoves(pNode node);
    int rollout(pNode currnode);
    int ifBigger(int num);
    int ifSmaller(int num);
    void chkTime();
    int chkVic(pNode currnode);
    void placeStones(const Move stone, int color, pNode checknode);
    bool isNotFull(pNode checknode);
    pNode searchBigUCB(pNode parentNode);
};

