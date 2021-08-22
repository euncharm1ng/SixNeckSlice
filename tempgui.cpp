
#include<cstdio>
#include "mcts.h"
#include <iostream>
#include <pthread.h>

int freeCount;
short** guiBoard;
short aiColor, userColor;
Mcts m;
pthread_t freer;

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
    setRedStone();

    m = Mcts(guiBoard, aiColor);
}

void 
setRedStone()
{
    int redCnt = 0;
    Move redStone;
    printf("input the number of red stone to place: ");
    scanf("%d", &redCnt);
    for(int i =0; i< redCnt; i++){
        printf("%dth red stone: ", i);
        scanf("%hd %hd", &redStone.x, &redStone.y);
        if(!chkBoundary(redStone)) 
            i--;
        else 
            placeStone(redStone, OBSTACLE);
    }
}

void
aiPlayFirst ()
{
    //TODO: run mcts that handlee first move
    guiBoard[10][10] = aiColor;
}


int 
chkBoundary (Move inputMove)
{
    if(inputMove.x > -1 && inputMove.x < 19 && inputMove.y > -1 && inputMove.y < 19){
        if(guiBoard[inputMove.y][inputMove.x] == 0) return 1;
    }
    printf("invalid input: %d, %d; please retry\n", inputMove.x, inputMove.y);
    return 0;
}

void 
userPlayFirst ()
{
    Move mov1;
    do{
        printf("input the first move: ");
        scanf("%hd %hd", &mov1.x, &mov1.y);
    }while(!chkBoundary(mov1));
    placeStone(mov1, userColor);
}


int
placeStone (Move inputMove, short inputColor)
{
    if(guiBoard[inputMove.y][inputMove.x] != 0) return -1;
    guiBoard[inputMove.y][inputMove.x] = inputColor;
    return 1;
}

int 
receiveUserInput ()
{
    Move mov1, mov2;
    printBoard();
    do{
        printf("input the first move: ");
        scanf("%hd %hd", &mov1.x, &mov1.y);
        printf("input the second move: ");
        scanf("%hd %hd", &mov2.x, &mov2.y);
    }while(!chkBoundary(mov1) && !chkBoundary(mov2));
    printf("placing stones: \n");
    placeStone(mov1, userColor);
    placeStone(mov2, userColor);

    if (m.chkVic(guiBoard, mov1, mov2))
        return 1;
    else return 0;
}

void 
printBoard()
{
    puts("y\\x 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8");
    for (int i = 0; i < 19; i++) {
        printf("  %d ", i%10);
        for (int j = 0; j < 19; j++) {
            if (guiBoard[i][j] == BLACK) printf(CYAN "o " NORM);
            else if (guiBoard[i][j] == WHITE) printf("o ");
            else if (guiBoard[i][j] == OBSTACLE) printf(RED "o " NORM);
            else printf(YELLOW "+ " NORM);
        }
        printf("\n");
    }
}

int 
aiPlays() 
{
    pNode result = m.runGame();
    placeStone(result->movesLog[0], aiColor);
    placeStone(result->movesLog[1], aiColor);
    int chkWin = m.chkVic(guiBoard, result->movesLog[0], result->movesLog[1]);

    pthread_create(&freer, NULL, freeAll, m.root);

    m.setRoot(aiColor);
    return chkWin;    
}



void 
runGUI()
{
    initGUI();
    
    if(userColor == BLACK){ 
        userPlayFirst();
        aiPlays();
    }
    else{ 
        aiPlayFirst();
        printBoard();
    }

    do{
        if(receiveUserInput()){
            puts("user wins");
            break;
        }
    }while(aiPlays() == 0);
    printBoard();
    puts("end of game");
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

