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

    UdpMessage sendMessage;
    sendMessage.data = "Hello from client";
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

        printf("Received responce from server %s:%u\ndata: %s\n", responseMessage.ip.c_str(), responseMessage.port, responseMessage.data.c_str());

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return EXIT_SUCCESS;
}