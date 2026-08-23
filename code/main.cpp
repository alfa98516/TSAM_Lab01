#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

int main(int argc, const char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage:" << argv[0] << " <IPv4 address>" << " <low port>"
                  << " <high port>" << '\n';
        return 1;
    }

    const std::string payload = "TSAM_SCAN";

    const char* ip_addr = argv[1]; // TODO: check for bounds
    const int low_port = std::stoi(argv[2]);
    const int high_port = std::stoi(argv[3]);

    timeval timeout{};
    timeout.tv_usec = 5000;

    std::vector<int> open_ports;

    struct sockaddr_in dest_addr{};

    // Set address family to IPv4, and convert port number.
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

    for (int curr_port = low_port; curr_port < high_port; curr_port++) {
        // Print current progress on the same line.
        double progress =
            100.0 * (curr_port - low_port) / (high_port - low_port);
        std::cout << "\rScan progress:" << progress << "%" << std::flush;

        dest_addr.sin_port = htons(curr_port);
        // Create socket.
        int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0) {
            perror("Error creating socket!");
            close(socket_fd);
            return 1;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                       sizeof(timeout)) < 0) {
            // perror("setsockopt");
            close(socket_fd);
            continue;
        }

        char data_buffer[2048];
        struct sockaddr_in src_addr{};
        socklen_t src_addr_len = sizeof(src_addr);
        ssize_t n_bytes;
        // perror("Error receiving");

        for (int i = 0; i < 5; i++) {
            if (sendto(socket_fd, payload.c_str(), payload.length(), 0,
                       (struct sockaddr*)&dest_addr, sizeof(dest_addr)) >= 0) {
                n_bytes =
                    recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,
                             (struct sockaddr*)&src_addr, &src_addr_len);
                if (n_bytes < 0) {
                    continue;
                } else {
                    // std::cout.write(data_buffer, n_bytes);
                    // std::cout << '\n';
                    open_ports.push_back(curr_port);
                    break;
                }
            }
        }

        close(socket_fd);
    }
    std::cout << '\n' << "Open ports:" << '\n';
    for (auto open_port : open_ports) {
        std::cout << open_port << '\n';
    }
}
