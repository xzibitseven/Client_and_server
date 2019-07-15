#include <iostream>
#include "client.hpp"

int main(int argc, char* argv[]) {
    try {
        if(argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <name> <port> <period>\n";
            return 0;
        }

        const std::int64_t port = std::stol(argv[2]);
        if(port < 1 || port > 65535)
            throw std::runtime_error("Port number out of range\n");

        const std::int64_t period = std::stol(argv[3]);
        if(period < 0)
            throw std::runtime_error("Period number out of range\n");

        Client client(argv[1], static_cast<std::uint16_t>(port), static_cast<std::uint64_t>(period));
        client.run();

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
    } catch( ... ) {
        std::cerr << "Unknown exception!!!\n";
    }

    return 0;
}
