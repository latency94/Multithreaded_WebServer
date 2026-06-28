#include "../include/Server.h"
#include "../include/HttpRequest.h"  // ← ADD THIS LINE
#include <iostream>
#include <unistd.h>
// ... other includes ...
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
    
    // FIX: Loop until complete request received
    std::string request;
    char buf[1024];
    int n;
    while((n = recv(clientSocket, buf, sizeof(buf), 0)) > 0) {
        request.append(buf, n);
        if(request.find("\r\n\r\n") != std::string::npos) break;
    }
    
    if(request.empty()) {
        close(clientSocket);
        return;
    }
requestCount++;
    Logger::log(request);
    std::cout << "\n====== REQUEST ======\n";
    std::cout << request << "\n";
    
    // FIX: Actually parse the request
    HttpRequest req;
    req.parse(request);
    
    std::string response;
    
    if(req.path == "/style.css")
    {
        std::string css = readFile("public/style.css");
        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/css\r\n"
            "Content-Length: " + std::to_string(css.size()) + "\r\n"
            "\r\n" + css;
    }
    else if(req.path == "/api/status")
    {
        std::string body =
            "{\"status\":\"running\","
            "\"requests_served\":" +
            std::to_string(requestCount.load()) +
            ",\"worker_threads\":4}";
        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" + body;
    }
    else if(req.path == "/hello")
    {
        std::string body = "Hello bro from multithreaded C++ server!";
        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" + body;
    }
    else if(req.path == "/")
    {
        std::string html = readFile("public/index.html");
        response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(html.size()) + "\r\n"
            "\r\n" + html;
    }
    else
    {
        std::string body = "404 - Route Not Found";
        response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" + body;
    }

// FIX: Handle partial sends
    size_t totalSent = 0;
    const char* data = response.c_str();
    size_t remaining = response.size();
    
    while(totalSent < remaining) {
        int sent = send(
            clientSocket,
            data + totalSent,
            remaining - totalSent,
            0
        );
        if(sent < 0) {
            break; // error occurred
        }
        totalSent += sent;
    }
    
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
