#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <string>

class Client {
    public:
        explicit Client(const std::string& name, const std::uint16_t& port, const std::uint64_t& period);
        void run();
        inline void stop() { m_work = false; }

    private:
        bool sendData(const std::string& buffer);

    private:
        bool m_work;
        const std::string m_name;
        struct sockaddr_in m_addr;
        std::int32_t m_socket;
        const std::uint64_t m_period; /* secоnd */
};

constexpr std::uint64_t MAXSLEEP{128};

#endif // CLIENT_HPP
