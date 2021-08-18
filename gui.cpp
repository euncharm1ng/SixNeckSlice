#include<cstdio>
#include "mcts.h"
#include <iostream>

#define WHITE 2
#define BLACK 1


void freeAll(pNode currnode);

int freeCount;

int main(void)
{
    
    // int * temparr = NULL;
    // temparr = (int*)malloc(sizeof(int) * 10);
    // printf("%p\n", temparr);
    // temparr[0] = 1;
    // free(temparr);
    // printf("%p %d\n", temparr, sizeof(temparr));
    // temparr[0] = 2;
    // printf("%d\n", temparr[0]);
    // freeCount = 0;

    Move mov1, mov2;
    short temp = 0, win = 0;
    short aiColor=0, userColor=0;

    short** board = (short**)malloc(sizeof(short*) * BOARDSIZE);
    for (int i = 0; i < BOARDSIZE; i++) {
        board[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        for (int j = 0; j < BOARDSIZE; j++) board[i][j] = 0;
    }

    printf("Choose your color (BLACK is '1', WHITE is '2'): ");
    scanf_s("%hd", &userColor);
    aiColor = (userColor == BLACK) ? WHITE : BLACK;
    printf("Then AiColor is %hd\n", aiColor);
    
    if (aiColor == BLACK) {
        board[10][10] = aiColor;
        printf("AI place first stone!!!\n");
    }

    do {
        printf("First stone : ");
        temp = scanf_s("%hd %hd", &mov1.x, &mov1.y);
        board[mov1.y][mov1.x] = userColor;

        printf("Second stone : ");
        temp = scanf_s("%hd %hd", &mov2.x, &mov2.y);
        board[mov2.y][mov2.x] = userColor;

        Mcts m = Mcts(board, aiColor);

        
        if (win = m.chkVic(board, mov1, mov2)) {

            if (win == aiColor) {
                printf("AI Win!!!\n");
                break;
            }
            else {
                printf("User Win!!!\n");
                break;
            }

        }
        

        pNode result = m.runGame();

        board[result->movesLog[0].y][result->movesLog[0].x] = aiColor;
        board[result->movesLog[1].y][result->movesLog[1].x] = aiColor;

        if (win = m.chkVic(board, mov1, mov2)) {

            if (win == aiColor) {
                printf("AI Win!!!\n");
                break;
            }
            else {
                printf("User Win!!!\n");
                break;
            }

        }

        freeAll(m.root);
        freeNode(m.root);

        freeCount = 0;

    } while (1);

}

void freeAll(pNode currnode) {
    
    while(!currnode->children->empty()){
        freeAll(currnode->children->back());
        currnode->children->pop_back();
    }
    // freeCount++;

    // if (currnode->children->empty()){ 
    //     freeNode(currnode);
    //     return;
    // }

    // do {
    //     freeAll(currnode->children->back());
    //     // freeNode(currnode->children->back());
    //     currnode->children->pop_back();
    // } while (!currnode->children->empty());
    freeNode(currnode);

    return;

}