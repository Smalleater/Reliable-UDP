#include <iostream>
#include <thread>

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

    if (!socket.Bind(0))
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

    std::string messageStr = "Hello from client";

    UdpMessage sendMessage;
    sendMessage.packetId = PacketId::Test;
    sendMessage.data.insert(sendMessage.data.end(), reinterpret_cast<std::byte*>(messageStr.data()), 
        reinterpret_cast<std::byte*>(messageStr.data() + messageStr.size()));
    sendMessage.ip = "127.0.0.1";
    sendMessage.port = 2026;

    UdpMessage responseMessage;

    while (true)
    {
        if (!socket.SendTo(sendMessage))
        {
            printf("Failed to send the message\n");
            return EXIT_FAILURE;
        }

        if(!socket.Received(responseMessage))
        {
            printf("Failed to received message");
            return EXIT_FAILURE;
        }

        if (responseMessage.data.size() == 0)
        {
            continue;
        }

        std::string receivedStr(reinterpret_cast<char*>(responseMessage.data.data()));

        printf("Received responce from server %s:%u\nPacketId: %u\ndata: %s\n", 
            responseMessage.ip.c_str(), responseMessage.port, responseMessage.packetId, receivedStr.c_str());

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return EXIT_SUCCESS;
}