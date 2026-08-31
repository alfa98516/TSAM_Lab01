#include <arpa/inet.h>
#include <cerrno>
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
    const std::string payload = argv[3];

    if (port < 0 || port > 65535) {
        std::cerr << "Port numbers range between 0 and 65535\n";
        return 1;
    }

    timeval timeout{};
    timeout.tv_sec = 1;

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

    char data_buffer[2048];
    struct sockaddr_in src_addr{};
    socklen_t src_addr_len = sizeof(src_addr);
    for (int i = 0; i < 5; i++) {

        dest_addr.sin_port = htons(port);
        if (sendto(socket_fd, payload.c_str(), payload.length(), 0,
                   (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
            continue;
        }
    }

    while (true) {

        ssize_t nbytes_recieved =
            recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,
                     (struct sockaddr*)&src_addr, &src_addr_len);
        if (nbytes_recieved < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "No response from port " << port << '\n';
                break;
            }
            continue;
        } else {
            std::cout << "Port " << ntohs(src_addr.sin_port)
                      << " responded with:\n";
            std::cout.write(data_buffer, nbytes_recieved);
            std::cout << '\n';
            break;
        }
    }
}
