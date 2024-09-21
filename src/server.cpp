#include "server.h"
#include "logger.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace socketcomm
{

    Server::Server(const std::string &socket_path) noexcept
        : socket_path_(socket_path), server_fd_(-1), running_(false), callback_(nullptr)
    {
        LOG_VERBOSE("Initializing server...");

        server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_fd_ == -1)
        {
            LOG_ERROR("Socket creation failed: " + std::string(strerror(errno)));
        }
        else
        {
            LOG_VERBOSE("Socket created successfully.");

            sockaddr_un addr;
            std::memset(&addr, 0, sizeof(addr));
            addr.sun_family = AF_UNIX;
            std::strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1U);

            // 古いソケットファイルを削除
            unlink(socket_path_.c_str());
            if (bind(server_fd_, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
            {
                LOG_ERROR("Bind failed: " + std::string(strerror(errno)));
                close(server_fd_);
                server_fd_ = -1;
            }
            else if (listen(server_fd_, 5) == -1)
            {
                LOG_ERROR("Listen failed: " + std::string(strerror(errno)));
                close(server_fd_);
                server_fd_ = -1;
            }
            else
            {
                LOG_VERBOSE("Socket bind and listen successful.");
            }
        }
    }

    Server::~Server() noexcept
    {
        LOG_VERBOSE("Server is shutting down.");
        stop();
        if (server_fd_ != -1)
        {
            close(server_fd_);
            unlink(socket_path_.c_str());
        }
        LOG_VERBOSE("Server shutdown complete.");
    }

    // ムーブコンストラクタ
    Server::Server(Server &&other) noexcept
        : socket_path_(std::move(other.socket_path_)),
          server_fd_(other.server_fd_),
          running_(other.running_.load()),
          callback_(std::move(other.callback_))
    {
        LOG_VERBOSE("Move constructor called.");

        other.server_fd_ = -1;
        other.running_.store(false);

        if (other.server_thread_.joinable())
        {
            server_thread_ = std::move(other.server_thread_);
        }
    }

    // ムーブ代入演算子
    Server &Server::operator=(Server &&other) noexcept
    {
        if (this != &other)
        {
            LOG_VERBOSE("Move assignment operator called.");
            // 現在のサーバーが動作中なら停止
            stop();

            socket_path_ = std::move(other.socket_path_);
            server_fd_ = other.server_fd_;
            running_.store(other.running_.load());
            callback_ = std::move(other.callback_);

            other.server_fd_ = -1;
            other.running_.store(false);

            if (other.server_thread_.joinable())
            {
                server_thread_ = std::move(other.server_thread_);
            }
        }
        return *this;
    }

    void Server::start() noexcept
    {
        if (!running_.load())
        {
            running_.store(true);
            LOG_INFO("Server is starting.");
            LOG_VERBOSE("Starting server thread.");
            server_thread_ = std::thread(&Server::run, this);
        }
        else
        {
            LOG_VERBOSE("Server is already running.");
        }
    }

    void Server::stop() noexcept
    {
        if (running_.load())
        {
            LOG_VERBOSE("Stopping server.");
            running_.store(false);

            if (server_fd_ != -1)
            {
                // 読み書きを停止
                shutdown(server_fd_, SHUT_RDWR);
                close(server_fd_);

                // 再度停止しないように無効化
                server_fd_ = -1;
            }

            if (server_thread_.joinable())
            {
                server_thread_.join();
            }
            LOG_INFO("Server has stopped.");
        }
        else
        {
            LOG_VERBOSE("Server is not running.");
        }
    }

    void Server::setCallback(Callback cb) noexcept
    {
        LOG_VERBOSE("Setting callback function.");
        callback_ = std::move(cb);
    }

    void Server::run() noexcept
    {
        while (running_.load())
        {
            int client_fd = accept(server_fd_, nullptr, nullptr);
            if (client_fd == -1)
            {
                if (!running_.load())
                {
                    // サーバーが停止され、正常に終了するシナリオ
                    LOG_INFO("Server is stopping, shutting down accept loop.");
                    break;
                }
                else if (errno == EINTR)
                {
                    // シグナルによってaccept()が中断された異常ケース
                    LOG_ERROR("Accept interrupted by signal.");
                    continue; // 中断された場合はループを継続
                }
                else
                {
                    // 他のエラーの場合
                    LOG_ERROR("Accept failed: " + std::string(strerror(errno)));
                    break;
                }
            }

            LOG_VERBOSE("Client connected, reading data.");
            // バッファをゼロクリア
            char buf[100] = {0};
            ssize_t n = read(client_fd, buf, sizeof(buf) - 1U);
            if (n > 0)
            {
                buf[n] = '\0';
                LOG_VERBOSE("Data received: " + std::string(buf));
                if (callback_)
                {
                    callback_(std::string(buf));
                }
            }
            else if (n == -1)
            {
                LOG_ERROR("Read error: " + std::string(strerror(errno)));
            }
            else
            {
                LOG_ERROR("Client disconnected or sent empty message.");
            }
            close(client_fd);
        }

        LOG_INFO("Server thread has exited.");
    }

} // namespace socketcomm
