#include <enet/enet.h>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "../common/protocol.hpp"

int main() {
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

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "BrushRoyale Client");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        
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

        sf::Event sfEvent;
        while (window.pollEvent(sfEvent)) {
            if (sfEvent.type == sf::Event::Closed) {
                enet_peer_disconnect(peer, 0);
                enet_host_flush(client); 
                window.close();
            }
        }

        window.clear(sf::Color(80, 30, 30)); // Dark Grey background
        
        
        window.display();
    }

    enet_host_destroy(client);
    return EXIT_SUCCESS;
}
