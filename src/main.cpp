#include "../include/Server.h"
#include <iostream>

int main()
{
    Server server(8080);

    if (!server.start())
    {
        std::cout << "Failed to start server\n";
        return 1;
    }

    std::cout << "Server running on http://localhost:8080\n";

    server.run();

    return 0;
}