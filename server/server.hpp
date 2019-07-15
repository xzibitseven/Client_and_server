#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <fstream>
#include "threadpool.hpp"

class Server {
    public:
        explicit Server(const std::uint16_t& port, const std::string& logFileName = {"log.txt"});
        ~Server();
        void run();
        inline void stop() {m_work = false;}

    private:
        void handlerClient(std::int32_t fd);

    private:
        bool m_work;
        std::int32_t m_listener;
        struct sockaddr_in m_addr;
        std::mutex m_mtx;
        std::ofstream m_os;
        ThreadPool<std::function<void()>> m_threadPool;
};

constexpr std::uint64_t READ_BUFFER_SIZE{100};

#endif // SERVER_HPP
