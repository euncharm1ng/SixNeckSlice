#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "Six Neck Slice");
    sf::CircleShape shape(20);
    sf::CircleShape shape2(4);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        shape.setPosition(200, 20);
        shape2.setPosition(400, 400);
        window.draw(shape);
        window.draw(shape2);
        window.display();
    }

    return 0;
}
