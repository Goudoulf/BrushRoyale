#pragma once
#include <cstdint>

const int MAX_POINTS_PER_PACKET = 20;

enum PacketType : uint8_t {
    PACKET_WELCOME = 0,
    PACKET_NEW_PLAYER = 1,
    PACKET_LEAVE = 2,
    PACKET_CURSOR = 3,
    PACKET_STROKE = 4
};

struct Point {
    int16_t x;
    int16_t y;
};

#pragma pack(push, 1)

struct PacketWelcome {
    uint8_t type = PACKET_WELCOME;
    uint32_t myId; 
    uint8_t r, g, b;
};

struct PacketNewPlayer {
    uint8_t type = PACKET_NEW_PLAYER;
    uint32_t playerId;
    uint8_t r, g, b;
};

struct PacketLeave {
    uint8_t type = PACKET_LEAVE;
    uint32_t playerId;
    uint8_t r, g, b;
};

struct PacketCursor {
    uint8_t type = PACKET_CURSOR;
    uint32_t playerId; 
    int16_t x;
    int16_t y;
};

struct PacketStroke {
    uint8_t type = PACKET_STROKE;
    uint32_t playerId; 
    
    uint8_t r, g, b;
    uint8_t radius;
    
    uint8_t count;     
    Point points[MAX_POINTS_PER_PACKET]; 
};

#pragma pack(pop)
