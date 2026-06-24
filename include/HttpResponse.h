#pragma once

#include <string>

class HttpResponse {
public:

    static std::string buildResponse(
        const std::string& body
    );
};