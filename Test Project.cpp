#include <iostream>
#include "Point System.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Test");

    pointManager manager;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        manager.step(&window);

        window.display();
    }

    return 0;
}