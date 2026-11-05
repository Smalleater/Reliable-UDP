#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

#include <cstdint>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

using SocketHandle = SOCKET;
using SocketLenght = int;

constexpr SocketHandle INVALID_SOCKET_HANDLE = INVALID_SOCKET;

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <cerrno>

using SocketHandle = int;
using SocketLenght = socklen_t;

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

class UdpSocket
{
public:
    UdpSocket();
    ~UdpSocket();

    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;

    bool Init();
    void Term();

    bool Bind(uint16_t port);

private:
    SocketHandle m_socketHandle;
};

#endif #UDP_SOCKET_HPP