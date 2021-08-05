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



class Mcts{
    public:
    Mcts();

    void runGame();
    float calcUCB(Node* node);
    void expansion(Node& currnode);
    Node* backprop(Node* currNode, int value);
    void select(Node& rootNode);
    void findMoves(Node& node);
    int rollout(Node& currnode);
    int ifBigger(int num);
    int ifSmaller(int num);
    void chkTime();
    int chkVic(Node& currnode);
    void placeStones(const Move stone, int color, Node& checknode);
    bool isNotFull(Node& checknode);
    Node* searchBigUCB(Node& parentNode);
};

