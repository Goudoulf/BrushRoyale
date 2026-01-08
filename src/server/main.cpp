#include <enet/enet.h>
#include <iostream>
#include <random> // For random number generation
#include "../common/protocol.hpp"

// A helper struct to keep track of players on the server side
struct ServerPlayer {
    uint32_t id;
    uint8_t r, g, b;
};

// Helper to generate random numbers
int randomRange(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);
    return distr(gen);
}

int main() {
    std::cout << "--- BrushRoyale Server ---" << std::endl;

    if (enet_initialize() != 0) {
        std::cerr << "[Error] Failed to initialize ENet." << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    address.host = ENET_HOST_ANY; 
    address.port = 7777;

    // 32 clients, 2 channels, unlimited bandwidth
    ENetHost* server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == nullptr) {
        std::cerr << "[Error] Failed to create ENet server host." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[Success] Server started on port 7777. Waiting for events..." << std::endl;

    ENetEvent event;
	uint32_t nextPlayerId = 1;

    while (enet_host_service(server, &event, 1000) >= 0) {
        switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT: {
                std::cout << "[Event] New client connected." << std::endl;

                // 1. Create Player Data
                // Allocate memory for this player's info so we can retrieve it later
                ServerPlayer* newPlayer = new ServerPlayer();
                newPlayer->id = nextPlayerId++;
                newPlayer->r = (uint8_t)randomRange(50, 255); // Avoid too dark colors
                newPlayer->g = (uint8_t)randomRange(50, 255);
                newPlayer->b = (uint8_t)randomRange(50, 255);

                // Store it in the peer so we know who this connection belongs to
                event.peer->data = newPlayer;

                // 2. Prepare the Welcome Packet
                PacketWelcome welcome;
                welcome.type = PACKET_WELCOME; // Ensure this matches your Enum
                welcome.myId = newPlayer->id;
                welcome.r = newPlayer->r;
                welcome.g = newPlayer->g;
                welcome.b = newPlayer->b;

                // 3. Send the Packet
                ENetPacket* packet = enet_packet_create(&welcome, sizeof(PacketWelcome), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(event.peer, 0, packet); // Send on Channel 0 (Reliable channel)

                std::cout << " -> Assigned ID: " << newPlayer->id 
                          << " Color: (" << (int)newPlayer->r << "," 
                          << (int)newPlayer->g << "," << (int)newPlayer->b << ")" << std::endl;
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT: {
                if (event.peer->data != NULL) {
                    ServerPlayer* player = (ServerPlayer*)event.peer->data;
                    std::cout << "[Event] Player " << player->id << " disconnected." << std::endl;
                    delete player;
                    event.peer->data = NULL;
                }
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << "[Event] Packet received (" 
                          << event.packet->dataLength << " bytes)" << std::endl;
                
                // Clean up the packet strictly after use
                enet_packet_destroy(event.packet);
                break;
                
            case ENET_EVENT_TYPE_NONE:
                // Timeout reached (1000ms) with no events. 
                // This is normal, the loop just runs again.
                break;
        }
    }

    // 5. Cleanup
    enet_host_destroy(server);
    return EXIT_SUCCESS;
}
