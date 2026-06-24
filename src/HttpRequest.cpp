#include "../include/HttpRequest.h"

#include <sstream>

void HttpRequest::parse(const std::string& raw)
{
    std::stringstream ss(raw);

    ss >> method;
    ss >> path;
    ss >> version;

    std::string line;

    std::getline(ss, line);

    while(std::getline(ss, line))
    {
        if(line == "\r" || line.empty())
        {
            break;
        }

        size_t pos = line.find(':');

        if(pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if(!value.empty() && value[0] == ' ')
            {
                value.erase(0, 1);
            }

            if(!value.empty() && value.back() == '\r')
            {
                value.pop_back();
            }

            headers[key] = value;
        }
    }
}