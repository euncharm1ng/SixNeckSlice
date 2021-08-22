
#include<cstdio>
#include "mcts.h"
#include <iostream>

int freeCount;
short** guiBoard;
short aiColor, userColor;
Move mov1, mov2;
Mcts m;

void 
initGUI ()
{
    guiBoard = (short**)malloc(sizeof(short*) * BOARDSIZE);
    for (int i = 0; i < BOARDSIZE; i++) {
        guiBoard[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        for (int j = 0; j < BOARDSIZE; j++) guiBoard[i][j] = 0;
    }
    userColor = -1;
    do {
        printf("Choose your color (BLACK is '1', WHITE is '2'): ");
        scanf("%hd", &userColor);
    }while(userColor < 0 && userColor > 3);
    aiColor = (userColor == BLACK) ? WHITE : BLACK;
    printf("Then AiColor is %hd\n", aiColor);

    m = Mcts(guiBoard, aiColor);
}

void
aiPlayFirst ()
{
    //TODO: run mcts that handlee first move
    guiBoard[10][10] = aiColor;
}

/*return 1 for valid input*/
int 
chkBoundary (Move inputMove)
{
    if(inputMove.x > -1 && inputMove.x < 19 && inputMove.y > -1 & inputMove.y < 19){
        if(guiBoard[inputMove.y][inputMove.x] == 0) return 1;
    }
    printf("invalid input: %d, %d; please retry\n", inputMove.x, inputMove.y);
    return 0;
}

void 
userPlayFirst ()
{
    do{
        printf("input the first move: ");
        scanf("%hd %hd", &mov1.x, &mov1.y);
    }while(!chkBoundary(mov1));
    placeStone(mov1, userColor);
}

/*return 1 on success return -1 for placing stone in occupied location*/
int
placeStone (Move inputMove, short inputColor)
{
    if(guiBoard[inputMove.y][inputMove.x] != 0 ) return -1;
    guiBoard[inputMove.y][inputMove.x] = inputColor;
    return 1;
}

void 
receiveUserInput ()
{
    do{
        printf("input the first move: ");
        scanf("%hd %hd", &mov1.x, &mov1.y);
        printf("input the second move: ");
        scanf("%hd %hd", &mov2.x, &mov2.y);
    }while(!chkBoundary(mov1) && !chkBoundary(mov2));
    printf("placing stones: \n");
    placeStone(mov1, userColor);
    placeStone(mov2, userColor);
}
void 
printBoard()
{
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            if (guiBoard[i][j] == 1) printf(RED "o " NORM);
            else if (guiBoard[i][j] == 2) printf("o ");
            else printf(YELLOW "+ " NORM);
        }
        printf("\n");
    }
}
void 
aiPlays () 
{
    puts("in aiplays()");
    pNode result = m.runGame();
    puts("in aiplays()");
    guiBoard[result->movesLog[0].y][result->movesLog[0].x] = aiColor;
    guiBoard[result->movesLog[1].y][result->movesLog[1].x] = aiColor;
    puts("aiplays here");
    printBoard();
    puts("aiplays here");
    freeAll(m.root);
    puts("aiplays here");
    freeNode(m.root);
    puts("aiplays here");
    m.setRoot(aiColor);
    puts("aiplays here");
}



void 
runGUI()
{
    initGUI();
    
    if(userColor == BLACK){ 
        userPlayFirst();
        aiPlays();
    }
    else aiPlayFirst();

    do{
        receiveUserInput();
        if (m.chkVic(guiBoard, mov1, mov2)){
            printf("user win\n"); 
            break;
        } 
        printf("chked user\n");

        aiPlays();
    }while(m.chkVic(guiBoard, mov1, mov2) != 0);

}
/*
int 
main ()
{
    short temp = 0, win = 0;
    

    if (aiColor == BLACK) {
        guiBoard[10][10] = aiColor;
        printf("AI place first stone!!!\n");
    }

    do {
        printf("First stone : ");
        temp = scanf("%hd %hd", &mov1.x, &mov1.y);
        guiBoard[mov1.y][mov1.x] = userColor;

        printf("Second stone : ");
        temp = scanf("%hd %hd", &mov2.x, &mov2.y);
        guiBoard[mov2.y][mov2.x] = userColor;

        Mcts m = Mcts(guiBoard, aiColor);

        
        if (win = m.chkVic(guiBoard, mov1, mov2)) {

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

        guiBoard[result->movesLog[0].y][result->movesLog[0].x] = aiColor;
        guiBoard[result->movesLog[1].y][result->movesLog[1].x] = aiColor;

        if (win = m.chkVic(guiBoard, mov1, mov2)) {

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
*/

void 
freeAll (pNode currnode) 
{
    if (currnode->children->empty()){ 
        free(currnode->movesLog);
        delete currnode->children;
        delete currnode;
        return;
    }

    do {
        freeAll(currnode->children->back());
        freeNode(currnode->children->back()); //?
        currnode->children->pop_back();
    } while (!currnode->children->empty());
}