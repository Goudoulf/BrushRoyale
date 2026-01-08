#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <optional>
#include <vector>
#include <iostream>

int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    uint32_t width = desktop.size.x;
    uint32_t height = desktop.size.y;
    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "Super Smash Bros Brush");
    window.setPosition({int(width / 4), int(height / 4)});
    window.setFramerateLimit(60);
    std::vector<sf::Vector2i> mouseDeltaList;

    sf::CircleShape circle;
    circle.setRadius(50.f);
    circle.setFillColor(sf::Color::Red);
    circle.setOrigin({50.f, 50.f});
    circle.setPosition({-100.f, 0.f});

    while (window.isOpen())
    {
        sf::Vector2i position = sf::Mouse::getPosition(window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            circle.setPosition({static_cast<float>(position.x), static_cast<float>(position.y)});
        }

        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // if (const auto* mouseMoved = event->getIf<sf::Event::MouseMovedRaw>())
            //     mouseDeltaList.push_back(mouseMoved->delta);

            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        // mouseDeltaList.clear();
        window.draw(circle);
        window.display();
    }
}
