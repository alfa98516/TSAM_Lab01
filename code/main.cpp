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
    if (argc < 4) {
        std::cerr << "Usage:" << argv[0] << " <IPv4 address>" << " <low port>"
                  << " <high port>" << '\n';
        return 1;
    }

    const std::string payload = "TSAM_SCAN";

    const char* ip_addr = argv[1]; // TODO: check for bounds
    const int low_port = std::stoi(argv[2]);
    const int high_port = std::stoi(argv[1]);

    timeval timeout{};
    timeout.tv_sec = 3;

    for (int curr_port = low_port; curr_port < high_port; curr_port++) {
        // Create socket.
        int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0) {
            perror("Error creating socket!");
            close(socket_fd);
            return 1;
        }

        struct sockaddr_in dest_addr{};

        // Set address family to IPv4, and convert port number.
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(curr_port);
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

        if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                       sizeof(timeout)) < 0) {
            perror("setsockopt");
            close(socket_fd);
            return 1;
        }

        // Send the string to the destination address.
        if (sendto(socket_fd, payload.c_str(), payload.length(), 0,
                   (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("Error sending");
            close(socket_fd);
            return 1;
        }

        char data_buffer[2048];
        struct sockaddr_in src_addr{};
        socklen_t src_addr_len = sizeof(src_addr);
        ssize_t n_bytes =
            recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,
                     (struct sockaddr*)&src_addr, &src_addr_len);
        if (n_bytes < 0) {
            perror("Error receiving");
            close(socket_fd);
            return 1;
        }

        std::cout.write(data_buffer, n_bytes);
        close(socket_fd);
    }
}

ssize_t sendto_wrapper(std::string& data) {
    return 3;
}
