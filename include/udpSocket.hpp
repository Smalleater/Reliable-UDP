#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "networkCustomValues.hpp"

constexpr uint16_t MTU_SIZE = 1200;
constexpr uint8_t HEADER_SIZE = sizeof(uint16_t) + sizeof(PacketId);

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

using SocketHandle = SOCKET;
using SocketLenght = int;
using SocketDataSize = int;

constexpr SocketHandle INVALID_SOCKET_HANDLE = INVALID_SOCKET;

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <cerrno>

using SocketHandle = int;
using SocketLenght = socklen_t;
using SocketDataSize = ssize_t;

constexpr SocketHandle INVALID_SOCKET_HANDLE = -1;

#endif

inline void closeSocket(SocketHandle socketHandle)
{
#ifdef _WIN32
    closesocket(socketHandle);
#else
    close(socketHandle);
#endif
}

class NetworkRuntime
{
public:
    NetworkRuntime() = default;
    ~NetworkRuntime();

    NetworkRuntime(const NetworkRuntime&) = delete;
    NetworkRuntime& operator=(const NetworkRuntime&) = delete;

    bool Init();
    void Term();

private:
};

struct UdpMessage
{
    PacketId packetId;
    std::vector<std::byte> data;
    std::string ip;
    uint16_t port;
};

class UdpSocket
{
public:
    UdpSocket();
    ~UdpSocket();

    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    bool Init();
    void Term();

    void SetProtocolId(uint16_t protocolId);
    bool SetNonBlocking(bool enable);

    bool Bind(uint16_t port);

    bool SendTo(const UdpMessage& message);
    bool Received(UdpMessage& message);

private:
    SocketHandle m_socketHandle;
    uint16_t m_protocolId;
};

#endif