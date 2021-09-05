// #include "mcts.h"
#include "minimax.h"
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <iostream>
#include <pthread.h>


using namespace std;
using namespace sf;

int cell_size = 30;
// int board[19][19] = {};


int freeCount;
short** guiBoard;
short aiColor, userColor;
// Mcts m;
Mini m;
pthread_t freer, GUIRunner;
pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wakegui = PTHREAD_COND_INITIALIZER;
pthread_cond_t wakeai = PTHREAD_COND_INITIALIZER;
int receiveFromGui = 0;
Move mov1, mov2;


int 
try_wait()
{
	int toReturn = 0;
	pthread_mutex_lock(&mylock);
	if(receiveFromGui > 0){
		receiveFromGui--;
		toReturn = 1;
	}
	pthread_mutex_unlock(&mylock);
	return toReturn;
}

void
post()
{
	pthread_mutex_lock(&mylock);
	receiveFromGui++;
	pthread_cond_wait(&wakeai, &mylock);
	pthread_mutex_unlock(&mylock);
	
}

void 
draw()
{
	pthread_mutex_lock(&mylock);
	receiveFromGui++;
	pthread_mutex_unlock(&mylock);
}

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
    //TODO: run mcts that handlee first move
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
userPlayFirst ()
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
GUIuserPlayFirst()
{
	// pthread_mutex_lock(&lock);
	// pthread_cond_signal(&wakegui);
	// pthread_cond_wait(&wakeai, &lock);
	// pthread_mutex_unlock(&lock);
	post();
}

int
GUIreceiveUserInput()
{
	// pthread_mutex_lock(&lock);
	// pthread_cond_signal(&wakegui);
	// pthread_cond_wait(&wakeai, &lock);

	// pthread_cond_signal(&wakegui);
	// pthread_cond_wait(&wakeai, &lock);
	
	// pthread_mutex_unlock(&lock);
	post(); post();

	if (m.chkVic(guiBoard, mov1, mov2))
        return 1;
    else return 0;
}



void 
runGUI()
{
	initGUI();
	pthread_create(&GUIRunner, NULL, runDahunGUI, NULL);
    
	
    if(userColor == BLACK){
		GUIuserPlayFirst();
        // userPlayFirst();
        aiPlays();
    }
    else{ 
        aiPlayFirst();
        // printBoard();
    }

    do{
        // if(receiveUserInput()){
		if(GUIreceiveUserInput()){
            puts("user wins");
            break;
        }
    }while(aiPlays() == 0);
    // printBoard();
    puts("end of game");
	draw();
	system("pause");
}



void*
runDahunGUI(void*)
{
	int toggleMov = 0;
	/* ----- void init() ----- */
	ContextSettings s;
	s.antialiasingLevel = 8;
	RenderWindow window(VideoMode(cell_size * 19, cell_size * 19), "SNS", Style::Default, s);
	CircleShape blackStone(1.0 * cell_size / 2), whiteStone(1.0 * cell_size / 2); // bs : Black Stone , ws : White Stone

	blackStone.setFillColor(Color::Black);
	blackStone.setOutlineColor(Color::Black);
	whiteStone.setFillColor(Color::White);
	whiteStone.setOutlineColor(Color::Black);
	whiteStone.setOutlineThickness(-2);

	auto draw_board = [&]() {
		window.clear(Color(255, 207, 97));
		float half_cell = 1.0 * cell_size / 2;
		for (int y = 0; y < 19; y++) //horizontal lines
		{
			Vertex hline[] = {
				Vertex(Vector2f(half_cell, half_cell + y * cell_size)),
				Vertex(Vector2f(cell_size * 19 - half_cell, half_cell + y * cell_size))
			};
			hline[0].color = Color::Black;
			hline[1].color = Color::Black;
			window.draw(hline, 2, Lines);
		}
		for (int x = 0; x < 19; x++) // vertical lines
		{
			Vertex vline[] = {
				Vertex(Vector2f(half_cell + x * cell_size, half_cell)),
				Vertex(Vector2f(half_cell + x * cell_size, cell_size * 19 - half_cell))
			};
			vline[0].color = Color::Black;
			vline[1].color = Color::Black;
			window.draw(vline, 2, Lines);
		}
		/* ----- draw 9 place holding dots ----- */
		float start_point_r = half_cell / 5;
		CircleShape circle(start_point_r);
		circle.setFillColor(Color::Black);

		for (int y = 0; y < 3; y++){
			for (int x = 0; x < 3; x++)
			{
				circle.setPosition(half_cell + (3 + 6 * x) * cell_size - start_point_r, half_cell + (3 + 6 * y) * cell_size - start_point_r);
				window.draw(circle);
			}
		}
	};

	auto draw_stones = [&]() {
		draw_board();
		for (int y = 0; y < 19; y++){
			for (int x = 0; x < 19; x++)
			{
				if (guiBoard[y][x] == BLACK)
				{
					blackStone.setPosition(x * cell_size, y * cell_size);
					window.draw(blackStone);
				}
				else if (guiBoard[y][x] == WHITE)
				{
					whiteStone.setPosition(x * cell_size, y * cell_size);
					window.draw(whiteStone);
				}
			}
		}
		window.display();
	};

	draw_board();
	window.display();

	while(1){
		// pthread_mutex_lock(&lock);
		// pthread_cond_wait(&wakegui, &lock);
		if(try_wait()){
			draw_stones();
			while(window.isOpen())
			{
				Event e;
				if (window.pollEvent(e))
				{
					if (e.type == Event::Closed)
						window.close();
					if (e.type == Event::MouseButtonPressed)
					{
						int ix = e.mouseButton.x / cell_size;
						int iy = e.mouseButton.y / cell_size;

						if (e.mouseButton.button == Mouse::Left)
						{
							if(toggleMov){
								mov1.x = ix;
								mov1.y = iy;
								if(placeStone(mov1, userColor) == -1) continue;
								printf("user input: %d %d\n", iy, ix);
								toggleMov--;
							}
							else{
								mov2.x = ix;
								mov2.y = iy;
								if(placeStone(mov2, userColor) == -1) continue;
								printf("user input: %d %d\n", iy, ix);
								toggleMov++;
							}
						}
						draw_stones();
						break;
					}
				}
				draw_stones();
			}
			pthread_cond_signal(&wakeai);
		}
		// puts("signaling ai");
		// pthread_cond_signal(&wakeai);
		// pthread_mutex_unlock(&lock);
	}
}