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

/**
 * @brief Main function that scans a given IPv4 address for open UDP ports in a
 * specified range.
 * @param argc Number of args.
 * @param argv Argument vector.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, const char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage:" << argv[0] << " <IPv4 address>" << " <low port>"
                  << " <high port>" << '\n';
        return 1;
    }

    const std::string payload = "TSAM_SCAN";

    const char* ip_addr = argv[1];
    const int low_port = std::stoi(argv[2]);
    const int high_port = std::stoi(argv[3]);

    timeval timeout{};
    // Time to wait in microseconds for each scan before timeout.
    timeout.tv_usec = 5000;

    std::vector<int> open_ports; // Array of found open ports.

    struct sockaddr_in dest_addr{};

    dest_addr.sin_family = AF_INET; // Set address family to IPv4.
    // Validate IPv4 address and convert it to binary form.
    int inet_pton_result = inet_pton(AF_INET, ip_addr, &dest_addr.sin_addr);

    if (inet_pton_result == 0) {
        std::cerr << "Invalid IPv4 address: " << ip_addr << '\n';
        return 1;
    }

    if (inet_pton_result < 0) {
        perror("inet_pton");
        return 1;
    }

    // Scan given IP address for open UDP ports in the given range.
    for (int curr_port = low_port; curr_port <= high_port; curr_port++) {
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

        // Set socket options.
        if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                       sizeof(timeout)) < 0) {
            close(socket_fd);
            continue;
        }

        // Buffer to hold received data, source address, and its length.
        char data_buffer[2048];
        struct sockaddr_in src_addr{};
        socklen_t src_addr_len = sizeof(src_addr);

        /* Send UPD datagrams to current port repeatedly until we receive a
         * response (max 5 tries). */
        for (int i = 0; i < 5; i++) {
            if (sendto(socket_fd, payload.c_str(), payload.length(), 0,
                       (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
                continue;
            }
            ssize_t n_bytes_received =
                recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,
                         (struct sockaddr*)&src_addr, &src_addr_len);
            if (n_bytes_received < 0) {
                continue;
            } else {
                open_ports.push_back(curr_port); // Open port found.
                break;
            }
        }

        close(socket_fd);
    }

    std::cout << '\n' << "Open ports:" << '\n';
    for (auto open_port : open_ports) {
        std::cout << open_port << '\n';
    }
}
