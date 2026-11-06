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

    UdpMessage receivedMessage;

    UdpMessage responseMessage;
    responseMessage.data = "Message received";

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

        printf("Message received from %s:%u\ndata: %s\n", receivedMessage.ip.c_str(), receivedMessage.port, receivedMessage.data.c_str());

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