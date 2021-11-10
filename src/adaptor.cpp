#include <iostream>
#include <pthread.h>
#include <string.h>
#include "../include/minimax.h"
#include "../include/connsix.h"
#include "../include/adaptor.hpp"

using namespace std;

int freeCount;
short** adaptor_board;
short home_color, away_color;
char *home_buffer = (char*)malloc(sizeof(char) * 8);

Mini home_ai;
pthread_t freer;
Move mov1, mov2;


void
ctoi(char *stone, int *i, int *j)
{
    char a;
    sscanf(stone, "%c%2d", &a, i);
    if(a >= 'A' && a <= 'H') *j = a - 'A';
    else *j = a - 'A' - 1;
    *i = 19 - *i;
}

char*
itoc(char *stone, int i, int j)
{
    char a = 'A' + j;
    if(j > 7) a++;
    stone[0] = a;
    sprintf(stone, "%c%02d\0", a, 19-i);
    return stone;
}

int
place_move (int x, int y, short inputColor)
{
    if(adaptor_board[y][x] != 0) return -1;
    adaptor_board[y][x] = inputColor;
    return 1;
}

void 
print_board()
{
    puts("y\\x 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8");
    for (int i = 0; i < 19; i++) {
        printf("  %d ", i%10);
        for (int j = 0; j < 19; j++) {
            if (adaptor_board[i][j] == BLACK) printf(CYAN "o " NORM);
            else if (adaptor_board[i][j] == WHITE) printf("o ");
            else if (adaptor_board[i][j] == OBSTACLE) printf(RED "o " NORM);
            else printf(YELLOW "+ " NORM);
        }
        printf("\n");
    }
}

char* 
ai_draw() 
{
    char* temp = (char*)malloc(sizeof(char) * 4);
    pNode result = home_ai.runGame(mov1, mov2);
    place_move(result->movesLog[0].x, result->movesLog[0].y, home_color);
    place_move(result->movesLog[1].x, result->movesLog[1].y, home_color);

    //int chkWin = home_ai.chkVic(guiBoard, result->movesLog[0], result->movesLog[1]);
	printf("x1: %2d y1: %2d\nx2: %2d y2: %2d\n\n", result->movesLog[0].x, result->movesLog[0].y, result->movesLog[1].x, result->movesLog[1].y);
    pthread_create(&freer, NULL, freeAll, home_ai.root);
	pthread_detach(freer);

    home_ai.setRoot(home_color);

    home_buffer[0] = 0;
    strcat(home_buffer, itoc(temp, result->movesLog[0].y, result->movesLog[0].x));
    strcat(home_buffer, ":");
    strcat(home_buffer, itoc(temp, result->movesLog[1].y, result->movesLog[1].x));

    free(temp);
	
    //return chkWin;    
    return home_buffer;
}

int
draw_away_move(char *away_draw)
{
    int i, j;
    if(strcmp(away_draw, "WIN") == 0 || strcmp(away_draw, "LOSE") == 0 || strcmp(away_draw, "TIE") == 0) return 0;
    ctoi(away_draw, &i, &j);
    //adaptor_board[i][j] = away_color;
    place_move(j, i, away_color);
    ctoi(&away_draw[4], &i, &j);
    place_move(j, i, away_color);
    //adaptor_board[i][j] = away_color;
    return 1;
}


int
set_red(char *redstones)
{
    if(redstones == NULL)
        { perror("red stone error\n"); exit(1); }
    int i, j;
    char *stone = strtok(redstones, ":");
    while(stone != NULL){
        ctoi(stone, &i, &j);
        adaptor_board[i][j] = OBSTACLE;
        stone = strtok(NULL, ":");
    }
}
    
void
init_api()
{
    char ip[20] = "127.0.0.1", color[10] ;
	int port = 0 ;
	
	//printf("Enter ip: ") ;
	//scanf("%s", ip) ;
	printf("Enter port number: ") ;
	scanf("%d", &port) ;
	printf("Enter player color: ") ;
	scanf("%s", color) ;

    adaptor_board = (short**)malloc(sizeof(short*) * BOARDSIZE);
    for(int i = 0; i < BOARDSIZE; i++){
        adaptor_board[i] = (short*)malloc(sizeof(short) * BOARDSIZE);
        memset(adaptor_board[i], 0, sizeof(short) * BOARDSIZE);
    }
    if(strcmp(color, "black") == 0)
        { home_color = BLACK; away_color = WHITE; }
    else
        { home_color = WHITE; away_color = BLACK; }

    set_red(lets_connect(ip, port, color));
    print_board();

	home_ai = Mini(adaptor_board, home_color);
	mov1.x = mov1.y = mov2.x = mov2.y = 0;
}


void 
run_game()
{
    char *away_draw;
    init_api();

    if(home_color == BLACK)
        { adaptor_board[9][9] = home_color; away_draw = draw_and_read("K10"); }
    else 
        away_draw = draw_and_read("");

	if (away_draw == NULL) 
        { perror("draw and read error\n"); exit(1); }
    else if (strcmp(away_draw, "K10") == 0)
        adaptor_board[9][9] = away_color;
    else
        draw_away_move(away_draw);

	printf("Read %s from server.\n", away_draw) ;
    print_board();

    do{
        away_draw = draw_and_read(ai_draw());
    }while(draw_away_move(away_draw));

    puts("end of game");
}