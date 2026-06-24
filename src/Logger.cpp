    #include "../include/Logger.h"

    #include <fstream>
    #include <mutex>
    #include <ctime>

    static std::mutex logMutex;

    void Logger::log(const std::string& msg)
    {
        std::lock_guard<std::mutex> lock(logMutex);

        std::ofstream file(
            "logs/server.log",
            std::ios::app
        );

        time_t now = time(nullptr);

        file << ctime(&now)
            << " "
            << msg
            << "\n";
    }