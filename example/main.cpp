#include <iostream>
#include "server.h" // libsocketcomm.so

void customDataHandler(const std::string &data)
{
    std::cout << "Custom data handler received: " << data;
}

int main()
{
    try
    {
        socketcomm::Server server("/tmp/uds_socket");

        server.setCallback(customDataHandler);
        server.start();

        std::cout << "Server is running. Press Enter to stop..." << std::endl;

        std::string dummy;
        std::getline(std::cin, dummy);
        std::cout << "Stop server...." << std::endl;

        server.stop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
