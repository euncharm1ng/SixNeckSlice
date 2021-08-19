
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

int cell_size = 70;
int board[19][19] = {};
#define BLACK (1)
#define WHITE (2)

int main(void)
{
	ContextSettings s;
	s.antialiasingLevel = 8;
	RenderWindow window(VideoMode(cell_size * 19, cell_size * 19), "SNS", Style::Default, s);

	CircleShape bs(1.0 * cell_size / 2), ws(1.0 * cell_size / 2); // bs : Black Stone , ws : White Stone
	bs.setFillColor(Color::Black);
	bs.setOutlineColor(Color::Black);
	ws.setFillColor(Color::White);
	ws.setOutlineColor(Color::Black);
	ws.setOutlineThickness(-2);

	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();
			if (e.type == Event::MouseButtonPressed)
			{
				int ix = e.mouseButton.x / cell_size;
				int iy = e.mouseButton.y / cell_size;

				// Mouse Left = Black Stone
				if (e.mouseButton.button == Mouse::Left)
				{
					board[iy][ix] = BLACK;
				}
				// Mouse Right = White Stone
				if (e.mouseButton.button == Mouse::Right)
				{
					board[iy][ix] = WHITE;
				}
			}
		}
		window.clear(Color(255, 207, 97));
		auto draw_board = [&]() {
			float half_cell = 1.0 * cell_size / 2;
			for (int y = 0; y < 19; y++)
			{
				Vertex hline[] = {
					Vertex(Vector2f(half_cell, half_cell + y * cell_size)),
					Vertex(Vector2f(cell_size * 19 - half_cell, half_cell + y * cell_size))
				};
				hline[0].color = Color::Black;
				hline[1].color = Color::Black;
				window.draw(hline, 2, Lines);
			}
			for (int x = 0; x < 19; x++)
			{
				Vertex vline[] = {
					Vertex(Vector2f(half_cell + x * cell_size, half_cell)),
					Vertex(Vector2f(half_cell + x * cell_size, cell_size * 19 - half_cell))
				};
				vline[0].color = Color::Black;
				vline[1].color = Color::Black;
				window.draw(vline, 2, Lines);
			}
			// 바둑판에서 9개의 점 찍기!
			float start_point_r = half_cell / 5;
			CircleShape circle(start_point_r);
			circle.setFillColor(Color::Black);

			for (int y = 0; y < 3; y++)
				for (int x = 0; x < 3; x++)
				{
					circle.setPosition(half_cell + (3 + 6 * x) * cell_size - start_point_r, half_cell + (3 + 6 * y) * cell_size - start_point_r);
					window.draw(circle);
				}
		};
		draw_board();
		auto draw_stones = [&]() {
			for (int y = 0; y < 19; y++)
				for (int x = 0; x < 19; x++)
				{
					if (board[y][x] == BLACK)
					{
						bs.setPosition(x * cell_size, y * cell_size);
						window.draw(bs);
					}
					if (board[y][x] == WHITE)
					{
						ws.setPosition(x * cell_size, y * cell_size);
						window.draw(ws);
					}
				}
		};
		draw_stones();
		window.display();
	}
	return 0;
}
  /*
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
    scanf("%hd", &userColor);
    aiColor = (userColor == BLACK) ? WHITE : BLACK;
    printf("Then AiColor is %hd\n", aiColor);
    
    if (aiColor == BLACK) {
        board[10][10] = aiColor;
        printf("AI place first stone!!!\n");
    }

    do {
        printf("First stone : ");
        temp = scanf("%hd %hd", &mov1.x, &mov1.y);
        board[mov1.y][mov1.x] = userColor;

        printf("Second stone : ");
        temp = scanf("%hd %hd", &mov2.x, &mov2.y);
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

    if (currnode->children->empty()) return;

    do {
        freeAll(currnode->children->back());
        freeNode(currnode->children->back());
        currnode->children->pop_back();
    } while (!currnode->children->empty());

    return;
    
    while(!currnode->children->empty()){
        freeAll(currnode->children->back());
        // currnode->children->pop_back();
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
