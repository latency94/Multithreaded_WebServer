#include "../include/HttpResponse.h"

std::string HttpResponse::buildResponse(
    const std::string& body)
{
    return
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    + body;
}