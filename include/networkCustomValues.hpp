#ifndef NETWORK_CUSTOM_VALUES_HPP
#define NETWOKR_CUSTOM_VALUES_HPP

#include <cstdint>

// Set your custom protocol ID
constexpr uint16_t PROTOCOL_ID = 0xABCD;

enum class PacketId : uint8_t
{
    None = 0,
    // Add your custom packet IDs here (e.g., Connect, Ping, PlayerMove, etc.)
    Test,
};

#endif