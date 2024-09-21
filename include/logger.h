#ifndef SOCKETCOMM_LOGGER_H
#define SOCKETCOMM_LOGGER_H

#include <iostream>
#include <string>

// INFOログマクロ
#define LOG_INFO(message) socketcomm::Logger::log("INFO", message, __PRETTY_FUNCTION__, __FILE__, __LINE__)

// ERRORログマクロ
#define LOG_ERROR(message) socketcomm::Logger::log("ERROR", message, __PRETTY_FUNCTION__, __FILE__, __LINE__)

// VERBOSEログマクロ (デバッグビルドでのみログを出力)
#ifndef NDEBUG
#define LOG_VERBOSE(message) socketcomm::Logger::log("VERBOSE", message, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define LOG_VERBOSE(message) // リリースビルドではVERBOSEログを出力しない
#endif

namespace socketcomm
{

    class Logger
    {
    public:
        static void log(const std::string &level, const std::string &message, const char *func, const char *file, int line)
        {
            std::cout << "[socketcomm][" << func << "][" << level << "] " << file << ":" << line << " - " << message << std::endl;
        }
    };

} // namespace socketcomm

#endif // SOCKETCOMM_LOGGER_H
