#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << "<IPv4 adress> <port> <payload>\n";
        return 1;
    }
    const char* ip_addr = argv[1];
    const int port = std::stoi(argv[2]);
    const char* payload = argv[3];

    if (port < 0 || port > 65535) {
        std::cerr << "Port numbers range between 0 and 65535\n";
        return 1;
    }

    timeval timeout{};
    timeout.tv_usec = 50000;

    struct sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;

    int inet_pton_result = inet_pton(AF_INET, ip_addr, &dest_addr.sin_addr);

    if (inet_pton_result == 0) {
        std::cerr << "Invalid IPv4 address: " << ip_addr << '\n';
        return 1;
    }
    if (inet_pton_result < 0) {
        perror("inet_pton");
        return 1;
    }

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Error creating socket!");
        close(socket_fd);
        return 1;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof(timeout)) < 0) {
        close(socket_fd);
        return 1;
    }

    struct sockaddr_in src_addr{};
    socklen_t src_addr_len = sizeof(src_addr);
    for (int i = 0; i < 5; i++) {
        if (sendto(socket_fd, payload, strlen(payload), ))
    }
}
