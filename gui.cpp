#include "mcts.h"
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

int cell_size = 30;
int board[19][19] = {};


int 
runDahunGUI()
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
					printf("here %d %d\n", iy, ix);
				}
				// Mouse Right = White Stone
				if (e.mouseButton.button == Mouse::Right)
				{
					board[iy][ix] = WHITE;
					printf("here %d %d\n", iy, ix);
				}
			}
		}
		window.clear(Color(255, 207, 97));
		auto draw_board = [&]() {
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

			for (int y = 0; y < 3; y++)
				for (int x = 0; x < 3; x++)
				{
					circle.setPosition(half_cell + (3 + 6 * x) * cell_size - start_point_r, half_cell + (3 + 6 * y) * cell_size - start_point_r);
					window.draw(circle);
				}
		}
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