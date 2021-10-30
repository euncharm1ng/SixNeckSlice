#include "minimax.h"
#include <iostream>
#include <pthread.h>

using namespace std;

int cell_size = 30;

int freeCount;
short** guiBoard;
short aiColor, userColor;

Mini m;
pthread_t freer;
Move mov1, mov2;



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

    // m = Mcts(guiBoard, aiColor);
	m = Mini(guiBoard, aiColor);
	mov1.x = mov1.y = mov2.x = mov2.y = 0;
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

int 
chkBoundary (Move inputMove)
{
    if(inputMove.x > -1 && inputMove.x < 19 && inputMove.y > -1 && inputMove.y < 19){
        if(guiBoard[inputMove.y][inputMove.x] == 0) return 1;
    }
    printf("invalid input: %d, %d; please retry\n", inputMove.x, inputMove.y);
    return 0;
}

int
placeStone (Move inputMove, short inputColor)
{
    if(guiBoard[inputMove.y][inputMove.x] != 0) return -1;
    guiBoard[inputMove.y][inputMove.x] = inputColor;
    return 1;
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

void
aiPlayFirst ()
{
    guiBoard[9][9] = aiColor;
}

int 
aiPlays() 
{
    pNode result = m.runGame(mov1, mov2);
    placeStone(result->movesLog[0], aiColor);
    placeStone(result->movesLog[1], aiColor);

    int chkWin = m.chkVic(guiBoard, result->movesLog[0], result->movesLog[1]);
	printf("x1: %2d y1: %2d\nx2: %2d y2: %2d\n\n", result->movesLog[0].x, result->movesLog[0].y, result->movesLog[1].x, result->movesLog[1].y);
    pthread_create(&freer, NULL, freeAll, m.root);
	pthread_detach(freer);

    m.setRoot(aiColor);
	
    return chkWin;    
}

void 
userPlayFirst()
{
    do{
        printf("input the first move: ");
        scanf("%hd %hd", &mov1.x, &mov1.y);
    }while(!chkBoundary(mov1));
    placeStone(mov1, userColor);
}

int 
receiveUserInput ()
{
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
runGUI()
{
	initGUI();

	//set connection
	//first draw as BLACK OR WHITE
	
    if(userColor == BLACK){
        // userPlayFirst();
        aiPlays();
    }
    else{ 
        aiPlayFirst();
        // printBoard();
    }

    do{
		//minimax -> server -> minimax
        if(receiveUserInput()){
            puts("user wins");
            break;
        }
    }while(aiPlays() == 0);
    // printBoard();
    puts("end of game");
	system("pause");
}
