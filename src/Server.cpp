#include "../include/Server.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <fstream>
#include <sstream>
#include "../include/Logger.h"

Server::Server(int p)
    : port(p),
      serverSocket(-1),
      pool(4),
      requestCount(0)
{
}
bool Server::start()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket < 0)
    {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    int opt = 1;
    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if(bind(
        serverSocket,
        (sockaddr*)&serverAddr,
        sizeof(serverAddr)
    ) < 0)
    {
        std::cerr << "Bind failed\n";
        return false;
    }

    if(listen(serverSocket, 10) < 0)
    {
        std::cerr << "Listen failed\n";
        return false;
    }

    return true;
}

std::string readFile(const std::string& path)
{
    std::ifstream file(path);

    if(!file.is_open())
    {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void Server::handleClient(int clientSocket)
{


    std::cout
    << "Thread ID: "
    << std::this_thread::get_id()
    << "\n";
    char buffer[4096] = {0};

    int bytesReceived =
        recv(clientSocket,
             buffer,
             sizeof(buffer),
             0);

    if(bytesReceived <= 0)
    {
        close(clientSocket);
        return;
    }

    std::string request(buffer);
    requestCount++;
    Logger::log(request);

    std::cout << "\n====== REQUEST ======\n";
    std::cout << request << "\n";

    std::string response;

    

 if(request.find("/style.css") != std::string::npos)
{
    std::string css =
        readFile("public/style.css");

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/css\r\n"
        "\r\n" +
        css;
}
else if(request.find("/api/status") != std::string::npos)
{
    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{"
        "\"status\":\"running\","
        "\"requests_served\":" +
        std::to_string(requestCount.load()) +
        ","
        "\"worker_threads\":4"
        "}";
}
else if(request.find("/hello") != std::string::npos)
{
    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello bro from multithreaded C++ server!";
}
else if(request.find("/ ") != std::string::npos)
{
    std::string html =
        readFile("public/index.html");

    response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n" +
        html;
}
    else
    {
        response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "404 - Route Not Found";
    }

    send(
        clientSocket,
        response.c_str(),
        response.size(),
        0
    );

    close(clientSocket);
}

void Server::run()
{
    while(true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientSize =
            sizeof(clientAddr);
        
        int clientSocket =
            accept(
                serverSocket,
                (sockaddr*)&clientAddr,
                &clientSize
            );

        if(clientSocket < 0)
        {
            continue;
        }

        pool.enqueue(
    [this, clientSocket]()
    {
        handleClient(clientSocket);
    });
    }
}