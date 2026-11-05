#include "udpSocket.hpp"

NetworkRuntime::~NetworkRuntime()
{
    Term();
}

bool NetworkRuntime::Init()
{
#ifdef _WIN32
    WSADATA data;
    
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    {
        return false;
    }
#endif

    return true;
}

void NetworkRuntime::Term()
{
#ifdef _WIN32
    WSACleanup();
#endif    
}

UdpSocket::UdpSocket() : 
    m_socketHandle(INVALID_SOCKET_HANDLE)
{

}

UdpSocket::~UdpSocket()
{
    Term();
}

bool UdpSocket::Init()
{
    m_socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (m_socketHandle == INVALID_SOCKET_HANDLE)
    {
        return false;
    }

    return true;
}

void UdpSocket::Term()
{
    if (m_socketHandle != INVALID_SOCKET_HANDLE)
    {
        closeSocket(m_socketHandle);
    }
}

bool UdpSocket::Bind(uint16_t port)
{
    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_socketHandle, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0)
    {
        return false;
    }

    return true;
}
