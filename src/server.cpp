#include <iostream>

#include "udpSocket.hpp"

int main()
{
    NetworkRuntime network;
    if (!network.Init())
    {
        printf("Failed to Init the network\n");
        return EXIT_FAILURE;
    }

    UdpSocket socket;

    if (!socket.Init())
    {
        printf("Failed to Init the UDP socket\n");
        return EXIT_FAILURE;
    }

    if (!socket.Bind(2026))
    {
        printf("Failed to Bind the UDP socket\n");
        return EXIT_FAILURE;
    }

    if (!socket.SetNonBlocking(true))
    {
        printf("Failed to set the socket to non blocking mode");
        return EXIT_FAILURE;
    }

    socket.SetProtocolId(PROTOCOL_ID);

    UdpMessage receivedMessage;

    std::string responseStr = "Message received";

    UdpMessage responseMessage;
    responseMessage.packetId = PacketId::None;
    responseMessage.data.insert(responseMessage.data.end(), reinterpret_cast<std::byte*>(responseStr.data()), 
        reinterpret_cast<std::byte*>(responseStr.data() + responseStr.size()));

    while (true)
    {
        if(!socket.Received(receivedMessage))
        {
            printf("Failed to received message");
            return EXIT_FAILURE;
        }

        if (receivedMessage.data.size() == 0)
        {
            continue;
        }

        std::string receivedStr(reinterpret_cast<char*>(receivedMessage.data.data()));

        printf("Message received from %s:%u\nPacketId: %u\ndata: %s\n", 
            receivedMessage.ip.c_str(), receivedMessage.port, receivedMessage.packetId, receivedStr.c_str());

        responseMessage.ip = receivedMessage.ip;
        responseMessage.port = receivedMessage.port;

        if (!socket.SendTo(responseMessage))
        {
            printf("Failed to send the response message\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}