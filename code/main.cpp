#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:" << argv[0] << "<IPv4 address>" << "<port>"
                  << '\n';
        return 1;
    }

    const char* ip_addr = argv[1]; // TODO: check for bounds
    int port = std::atoi(argv[2]);
    // Try to make an IPv4 UDP socket.
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Error creating socket!");
        close(socket_fd);
        return 1;
    }
    std::string s = "5+6";

    // TODO: make dat damn dest_addr

    struct sockaddr_in dest_addr{};

    // Set address family to IPv4, and port number to 4006.
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    int inet_pton_result = inet_pton(AF_INET, ip_addr, &dest_addr.sin_addr);

    if (inet_pton_result == 0) {
        std::cerr << "Invalid IPv4 address: " << ip_addr << '\n';
        close(socket_fd);
        return 1;
    }

    if (inet_pton_result < 0) {
        perror("inet_pton");
        close(socket_fd);
        return 1;
    }

    // Send the string to the destination address.
    if (sendto(socket_fd, s.c_str(), s.length(), 0,
               (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Error sending");
        close(socket_fd);
        return 1;
    }

    struct sockaddr_in src_addr{};
    socklen_t src_addr_len;
    char data_buffer[2048];

    if (recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,
                 (struct sockaddr*)&src_addr, &src_addr_len) < 0) {
        perror("Error receiving");
        close(socket_fd);
        return 1;
    }

    std::cout << "received: " << data_buffer << '\n';
    close(socket_fd);
}

ssize_t sendto_wrapper(std::string& data) {
    return 3;
}
