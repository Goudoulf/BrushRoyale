#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <optional>
#include <vector>
#include <iostream>

#include <enet/enet.h>

#include "../common/protocol.hpp"


int main()
{
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet." << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost* client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == nullptr) {
        std::cerr << "Failed to create ENet client." << std::endl;
        return EXIT_FAILURE;
    }

    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1"); 
    address.port = 7777;

    std::cout << "Connecting to server..." << std::endl;
    ENetPeer* peer = enet_host_connect(client, &address, 2, 0);

    if (peer == nullptr) {
        std::cerr << "No available peers for initiating an ENet connection." << std::endl;
        return EXIT_FAILURE;
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection to 127.0.0.1:7777 succeeded." << std::endl;
    } else {
        enet_peer_reset(peer);
        std::cerr << "Connection failed. Is the server running?" << std::endl;
        return EXIT_FAILURE;
    }

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
		while (enet_host_service(client, &event, 0) > 0) {
            switch (event.type) {
				case ENET_EVENT_TYPE_RECEIVE: {
					uint8_t packetType = event.packet->data[0];

					if (packetType == PACKET_WELCOME) {
						if (event.packet->dataLength < sizeof(PacketWelcome)) {
							std::cerr << "[Error] Malformed Welcome packet received." << std::endl;
						} else {
							PacketWelcome* welcome = (PacketWelcome*)event.packet->data;

							std::cout << "-----------------------------------" << std::endl;
							std::cout << "[Server] Welcome! You are connected." << std::endl;
							std::cout << " -> Your ID: " << welcome->myId << std::endl;
							std::cout << " -> Your Color: R=" << (int)welcome->r 
									  << " G=" << (int)welcome->g 
									  << " B=" << (int)welcome->b << std::endl;
							std::cout << "-----------------------------------" << std::endl;
							
							//Todo Save the data for client
						}
					} else {
						std::cout << "[Network] Unknown packet received (Type: " << (int)packetType << ")" << std::endl;
					}

					enet_packet_destroy(event.packet);
					break;
				}

                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Server disconnected." << std::endl;
                    break;
            }
        }


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

    enet_host_destroy(client);
    return EXIT_SUCCESS;
}
