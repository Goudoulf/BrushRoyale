#include <enet/enet.h>
#include <iostream>

int main() {
    std::cout << "--- BrushRoyale Server Test ---" << std::endl;

    // 1. Initialize ENet Library
    if (enet_initialize() != 0) {
        std::cerr << "[Error] An error occurred while initializing ENet." << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize); // Ensure it cleans up when we quit
    std::cout << "[Success] ENet library initialized." << std::endl;

    // 2. Configure the Address
    ENetAddress address;
    address.host = ENET_HOST_ANY; // Listen on 0.0.0.0 (All interfaces)
    address.port = 7777;          // The port number

    // 3. Create the Server Host
    // Arguments: &address, max_clients (32), channels (2), down_bandwidth (0=unlimited), up_bandwidth (0=unlimited)
    ENetHost* server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == nullptr) {
        std::cerr << "[Error] An error occurred while trying to create an ENet server host." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[Success] Server started on port 7777!" << std::endl;
    std::cout << "          (If you see this, the libraries are linked correctly)" << std::endl;

    // 4. Cleanup (Just for this test)
    enet_host_destroy(server);

    return EXIT_SUCCESS;
}
