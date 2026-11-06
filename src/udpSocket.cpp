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

bool UdpSocket::SendTo(const UdpMessage &message)
{
    sockaddr_in destination{};

    destination.sin_family = AF_INET;
    destination.sin_port = htons(message.port);

    if (inet_pton(AF_INET, message.ip.c_str(), &destination.sin_addr) != 1)
    {
        return false;
    }

    int result = sendto(
        m_socketHandle, 
        message.data.c_str(), 
        message.data.size(), 
        0, 
        reinterpret_cast<sockaddr*>(&destination), 
        sizeof(destination)
    );

    return result < 0 ? false : true; 
}

bool UdpSocket::Received(UdpMessage &message)
{
    char buffer[MTU_SIZE];

    sockaddr_in sender{};
    SocketLenght senderLenght = sizeof(sender);

    int received = recvfrom(
        m_socketHandle,
        buffer,
        sizeof(buffer),
        0,
        reinterpret_cast<sockaddr*>(&sender),
        &senderLenght
    );

    if (received < 0)
    {
        return false;
    }

    char ip[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &sender.sin_addr, ip, sizeof(ip)) == nullptr)
    {
        return false;
    }

    message.data.assign(buffer, received);
    message.ip = ip;
    message.port = ntohs(sender.sin_port);

    return true;
}
