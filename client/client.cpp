#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "client.hpp"

Client::Client(const std::string& name, const std::uint16_t& port, const std::uint64_t& period):
    m_work(true),
    m_name(name),
    m_socket(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)),
    m_period(period) {

    if(m_socket < 0)
        throw std::runtime_error("Error create socket\n");

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = inet_addr("127.0.0.7"); //htonl(INADDR_LOOPBACK);

    if(connect(m_socket, reinterpret_cast<struct sockaddr*>(&m_addr), sizeof(m_addr)) < 0) {
        close(m_socket);
        std::cerr << "Error connect\n";
    }
}

bool Client::sendData(const std::string& buffer) {

    std::uint64_t writeBytes{0};
    const std::uint64_t sizeBuffer = buffer.size();
    std::int64_t bytes{0};

       while(writeBytes < sizeBuffer) {
           bytes = write(m_socket, &buffer[0]+writeBytes, buffer.size() - writeBytes);
           if(bytes <= 0) {
               if(errno == EINTR)
                   continue;
               close(m_socket);
               return false;
           }
           writeBytes += static_cast<std::uint64_t>(bytes);
       }
       return true;
}

void Client::run() {

    auto timeInHhMmSdMmm = []() -> std::string {
            namespace sc = std::chrono;
            auto now = sc::system_clock::now();

            // get number of milliseconds for the current second
            // (remainder after division into seconds)
            auto ms = sc::duration_cast<sc::milliseconds>(now.time_since_epoch()) % 1000;

            // convert to std::time_t in order to convert to std::tm (broken time)
            auto timer = sc::system_clock::to_time_t(now);

            // convert to broken time
            std::tm bt = *std::localtime(&timer);

            std::ostringstream oss;
            oss << std::put_time(&bt, "%F %T"); // HH:MM:SS
            oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return oss.str();
    };

    while(m_work) {
        if((m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            throw std::runtime_error("Error create socket\n");

        if(connect(m_socket, reinterpret_cast<struct sockaddr*>(&m_addr), sizeof(m_addr)) == 0) {
            std::string buffer{'[' + timeInHhMmSdMmm() + "] " + m_name};
            if(!sendData(buffer))
                std::cerr << "Error send data\n";
        }
        close(m_socket);
        std::this_thread::sleep_for(std::chrono::seconds(m_period));
    }
}

