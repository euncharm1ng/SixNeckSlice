#include "mcts.h"
#define DEBUG 0 //to 1 to run in debug mode
#define BLACK 1
#define WHITE 2
#define BOARDSIZE 19
#define MOVRANGE 2
#define TIMELIMIT 20
#define INFI 3e10
#define C_BLUE "\033[34m"
#define C_AQUA "\033[36m"
#define C_NRML "\033[0m"



bool comp(Move mov1, Move mov2) {
    if (mov1.x < mov2.x) return true;
    else if (mov1.y < mov2.y) return true;
    else return false;
}

bool operator == (const Move& m1, const Move& m2) {
    if (m1.x == m2.x && m1.y == m2.y)
        return true;
    else
        return false;
}

Node::Node(short int curr_color, pNode myparent) { //find move with 
    color = curr_color;
    parent = myparent;
    t = 0;
    n = 0;
    ucb = INFI;
    //children.resize(128);
    //availMov.resize(32);

    this->board = (unsigned short int**)malloc(sizeof(unsigned short int*) * BOARDSIZE);
    for (int i = 0; i < BOARDSIZE; i++) {
        this->board[i] = (unsigned short int*)malloc(sizeof(unsigned short int) * BOARDSIZE);
        for (int j = 0; j < BOARDSIZE; j++) {
            this->board[i][j] = myparent->board[i][j];
        }
    }
}

Node::Node(short int curr_color) {
    color = curr_color;
    parent = NULL;
    t = 0;
    n = 0;
    ucb = INFI;
    vector<pNode> children;
    vector<Move> availMov;

    this->board = (unsigned short int**)malloc(sizeof(unsigned short int*) * BOARDSIZE);
    for (int i = 0; i < BOARDSIZE; i++) {
        this->board[i] = (unsigned short int*)malloc(sizeof(unsigned short int) * BOARDSIZE);
        for (int j = 0; j < BOARDSIZE; j++) {
            this->board[i][j] = 0;
        }
    }
}


void Node::freeNode() {
    for (int i = 0; i < BOARDSIZE; ++i) {
        free(this->board[i]);
    }
    free(this->board);

}


void Node::addC(pNode child) {
    this->children.push_back(child);
}

void Node::printC() {
#if DEBUG
    printf("--print child\n");
#endif
    int i = 0;
    for (auto& c : this->children) {
        i++;
        printf("printing board: \n");
        c->printBoard();
    }
    printf("end of print child with %d iterations\n", i);
#if DEBUG
    printf("--end of print child\n");
#endif
}

void Node::printBoard() {
#if DEBUG
    printf("print board\n");
#endif
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            if(this->board[i][j] == 1) printf("○");
            else if (this->board[i][j] == 2) printf("●");
            else printf("＃");
            printf("%s", C_NRML);
        }
        printf("\n");
    }
#if DEBUG
    printf("end of print board\n");
#endif
}

void Node::printAvailMov() {
#if DEBUG
    printf("==print Avail Mov\n");
#endif
    int i = 0;
    for (auto& m : this->availMov) {
        if (i++ % 10 == 0) printf("\n");
        printf("x: %2d, y: %2d \t", m.x, m.y);
    }
    printf("\n");
#if DEBUG
    printf("==end of print Avail Mov\n");
#endif
}

float Node::getUCB() {
    return this->ucb;
}

void Node::setUCB(float num) {
    this->ucb = num;
}

void Node::printUCB() {
    printf("ucb: %f\n", this->ucb);
}

void Node::appendAvailMov(Move mov) {
    this->availMov.push_back(mov);
}