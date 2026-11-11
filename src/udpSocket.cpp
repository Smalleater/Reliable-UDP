#include "udpSocket.hpp"

#include <memory>
#include <cstring>

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
    m_socketHandle(INVALID_SOCKET_HANDLE),
    m_protocolId(0)
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

void UdpSocket::SetProtocolId(uint16_t protocolId)
{
    m_protocolId = protocolId;
}

bool UdpSocket::SetNonBlocking(bool enable)
{
#ifdef _WIN32
    u_long mode = enable;

    if (ioctlsocket(m_socketHandle, FIONBIO, &mode) != NO_ERROR)
    {
        return false;
    }
#else
    int flags = fcntl(m_socketHandle, F_GETFL);
    if (flags == -1)
    {
        return false;
    }

    flags = enable ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;

    if (fcntl(m_socketHandle, F_SETFL, flags) == -1)
    {
        return false;
    }
#endif

    return true;
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

    SocketDataSize packetSize = HEADER_SIZE + message.data.size();
    if (packetSize > MTU_SIZE)
    {
        return false;
    }

    std::vector<std::byte> packet(packetSize);

    SocketDataSize writeOffset = 0;

    std::memcpy(packet.data() + writeOffset, &m_protocolId, sizeof(uint16_t));
    writeOffset += sizeof(uint16_t);

    std::memcpy(packet.data() + writeOffset, &message.packetId, sizeof(PacketId));
    writeOffset += sizeof(message.packetId);

    std::memcpy(packet.data() + writeOffset, message.data.data(), message.data.size());
    writeOffset += message.data.size();

    SocketDataSize byteSent = sendto(
        m_socketHandle, 
        reinterpret_cast<const char*>(packet.data()), 
        packet.size(), 
        0, 
        reinterpret_cast<sockaddr*>(&destination), 
        sizeof(destination)
    );

    return byteSent == packetSize;
}

bool UdpSocket::Received(UdpMessage &message)
{
    std::vector<std::byte> packet(MTU_SIZE);

    sockaddr_in sender{};
    SocketLenght senderLenght = sizeof(sender);

    SocketDataSize byteReceived = recvfrom(
        m_socketHandle,
        reinterpret_cast<char*>(packet.data()),
        packet.size(),
        0,
        reinterpret_cast<sockaddr*>(&sender),
        &senderLenght
    );

    if (byteReceived < 0)
    {
#ifdef _WIN32
        int error = WSAGetLastError();
        if (error != WSAEWOULDBLOCK && error != WSAECONNRESET)
        {
            return false;
        }
#else
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            return false;
        }
#endif
        message.packetId = PacketId::None;
        message.data.clear();
        message.ip = "0";
        message.port = 0;

        return true;
    }
    else if (byteReceived < HEADER_SIZE || byteReceived > MTU_SIZE)
    {
        return false;
    }

    SocketDataSize readOffset = 0;

    uint16_t protocolId = 0;
    std::memcpy(&protocolId, packet.data() + readOffset, sizeof(uint16_t));
    readOffset += sizeof(uint16_t);

    if (protocolId != m_protocolId)
    {
        message.packetId = PacketId::None;
        message.data.clear();
        message.ip = "0";
        message.port = 0;

        return true;
    }

    char ip[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &sender.sin_addr, ip, sizeof(ip)) == nullptr)
    {
        return false;
    }

    std::memcpy(&message.packetId, packet.data() + readOffset, sizeof(PacketId));
    readOffset += sizeof(PacketId);

    message.data.assign(packet.begin() + HEADER_SIZE, packet.end());
    message.ip = ip;
    message.port = ntohs(sender.sin_port);

    return true;
}
