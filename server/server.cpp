#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <regex>
#include <iostream>
#include "server.hpp"

Server::Server(const std::uint16_t& port, const std::string& logFileName):
    m_work(true),
    m_listener(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)),
    m_os(std::ofstream(logFileName, std::ios::out)) {

    if(m_listener < 0)
        throw std::runtime_error("Error create socket\n");

    if(!m_os.is_open())
        throw std::runtime_error("Ofstream failed to open " + logFileName);

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr.s_addr = inet_addr("127.0.0.7"); //htonl(INADDR_ANY);

    if(bind(m_listener, reinterpret_cast<struct sockaddr *>(&m_addr), sizeof(m_addr)) < 0)
        throw std::runtime_error("Error bind\n");

    if(listen(m_listener, 1000) < 0)
        throw std::runtime_error("Error listen\n");

    if(fcntl(m_listener, F_SETFL, O_NONBLOCK) < 0)
        std::cerr << "Error fcntl\n";
}

Server::~Server() {
    m_os.flush();
    m_os.close();
    close(m_listener);
}

void Server::handlerClient(std::int32_t fd) {

    std::string buffer(READ_BUFFER_SIZE, '\0');
    std::uint64_t read_buffer_used{0};
    std::int64_t bytes_read{0};

    for(;;) {
        bytes_read = read(fd, &buffer[0] + read_buffer_used, READ_BUFFER_SIZE - read_buffer_used);
        if(bytes_read == 0) {
            close(fd);
            return;
        }
        if(bytes_read < 0) {
            if(errno == EINTR)
                continue;
            close(fd);
            return;
        }
        break; // read() succeeded
    }

    std::uint64_t check = read_buffer_used;
    std::uint64_t check_end = read_buffer_used + static_cast<std::uint64_t>(bytes_read);
    read_buffer_used = check_end;

    while(check < check_end) {
        if(buffer[check] != '\n'){
                check++;
                continue;
        }
        std::uint64_t length = check;
        buffer[length] = '\0';
        if((length > 0) && (buffer[length - 1] == '\r')) {
            buffer[length - 1] = '\0';
            length--;
        }

        auto checkData = [&buffer]()->bool {
            std::regex reg(R"(\[20[0-9]{2}-[01][0-9]-[0123][0-9]\s[012][0-9]:[0-6][0-9]:[0-6][0-9].[0-9]{3}\])");
            std::string::size_type pos = buffer.find(']');
            if(pos == std::string::npos)
                return false;
            ++pos;
            if(!std::regex_match(buffer.substr(0,pos), reg))
                return false;
            ++pos;
            std::string::size_type npos = buffer.find('\0',pos);
            if((npos == std::string::npos) || (pos == npos))
                return false;

            return true;
        };

        if(!checkData()) {
            std::cerr << "Client sent invalid data, closing connection.\n";
            close(fd);
            return;
        }
        read_buffer_used -= check + 1;
        check_end -= check;
        check = 0;
    }
    if(read_buffer_used == READ_BUFFER_SIZE) {
        std::cerr << "Client sent a very long string, closing connection.\n";
        close(fd);
    }
    close(fd);

    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_os << buffer.substr(0,buffer.find('\0')) << '\n';
        m_os.flush();
    }
}

void Server::run() {

    fd_set fdsMaster;
    FD_ZERO(&fdsMaster);
    FD_SET(m_listener, &fdsMaster);
    std::int32_t fdMax{m_listener};

    while(m_work) {

        fd_set fdsRead{fdsMaster};
        std::int32_t tfdMax{fdMax};
        std::int32_t res = pselect(fdMax+1, &fdsRead, nullptr, nullptr, nullptr, nullptr);
        if(res < 0)
             throw std::runtime_error("Error select\n");

        if(res == 0)
            continue;

        for(std::int32_t i=0; i<=tfdMax; ++i) {
            if(FD_ISSET(i, &fdsRead)) {
                if(i == m_listener) {
                    std::int32_t sock = accept(m_listener, nullptr, nullptr);
                    if(sock < 0)
                        throw std::runtime_error("Error accept\n");
                    else {
                        if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
                            std::cerr << "Error fcntl\n";

                        FD_SET(sock, &fdsMaster);
                        if (sock > fdMax)
                            fdMax = sock;
                    }
                }else {
                     m_threadPool.submit(std::bind(&Server::handlerClient,this,i));
                     FD_CLR(i, &fdsMaster);
                }
            }
        }
    }
}
