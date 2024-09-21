#ifndef SOCKETCOMM_SERVER_H
#define SOCKETCOMM_SERVER_H

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <functional>

namespace socketcomm
{

    class Server
    {
    public:
        using Callback = std::function<void(const std::string &)>;

        Server(const std::string &socket_path) noexcept;
        ~Server() noexcept;
        Server(const Server &) = delete;
        Server(Server &&) noexcept;
        Server &operator=(const Server &) = delete;
        Server &operator=(Server &&) noexcept;

        void start() noexcept;

        void stop() noexcept;

        void setCallback(Callback cb) noexcept;

    private:
        void run() noexcept;

        std::string socket_path_;
        int server_fd_;
        std::atomic<bool> running_;
        std::thread server_thread_;
        Callback callback_;
    };

} // namespace socketcomm

#endif // SOCKETCOMM_SERVER_H
