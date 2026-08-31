#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

/**
 * @brief Main function that scans a given IPv4 address for open UDP ports in
 * specified range.
 * @param argc Number of args.
 * @param argv Argument vector.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, const char* argv[]) {

    // Make sure we have exactly four arguments
    if (argc != 4) {
        std::cerr << "Usage:" << argv[0]
                  << " <IPv4 address> <low port> <high port>" << '\n';
        return 1;
    }

    const std::string payload = "hi";

    const char* ip_addr = argv[1];

    // Try to make low port and high port integers, catch if not possible
    const char* argv2 = argv[2];
    const int low_port = [argv2]() {
        try {
            return std::stoi(argv2);
        } catch (std::invalid_argument) {
            std::cerr << "Argument 2 must be of type integer";
        }
        return -1;
    }();

    const char* argv3 = argv[3];
    const int high_port = [argv3]() {
        try {
            return std::stoi(argv3);
        } catch (std::invalid_argument) {
            std::cerr << "Argument 3 must be of type integer";
        }
        return -1;
    }();


    if ((low_port < 0 || low_port > 65535) ||
        (high_port > 65535 || high_port < 0)) {
        std::cerr << "Port numbers range between 0 and 65535\n";
        return 1;
    }

    if (low_port > high_port) {
        std::cerr
            << "First specified port number must be lower than the second\n";
        std::cerr << "Did you mean: " << high_port << " " << low_port << "?\n";
        return 1;
    }

    timeval timeout{};

    // Time to wait in microseconds for each scan before timeout.
    timeout.tv_sec = 1;

    const int port_count = high_port - low_port + 1; // set of found open ports.
    bool open_ports[port_count];
    memset(open_ports, 0, sizeof(open_ports));

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

    // Set socket options.

    // Buffer to hold received data, source address, and its length.
    char data_buffer[2048];
    struct sockaddr_in src_addr{};
    socklen_t src_addr_len = sizeof(src_addr);
    // Scan given IP address for open UDP ports in the given range.
    for (int curr_port = low_port; curr_port <= high_port; curr_port++) {
        // Print current progress on the same line.
        double progress = 100.0 * (curr_port - low_port + 1) / port_count;
        std::cout << "\rScan Progress:" << (int)progress << "%" << std::flush;

        dest_addr.sin_port = htons(curr_port);

        /* Send UPD datagrams to current port repeatedly until we receive a
         * response (max 5 tries). */
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (sendto(socket_fd, payload.c_str(), payload.length(), 0,
                       (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
                continue;
            }
        }
        // Get the responses.
    }

    while (true) {
        ssize_t n_bytes_received =
            recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,
                     (struct sockaddr*)&src_addr, &src_addr_len);
        // Got an error.
        if (n_bytes_received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break; // A timeout.
            }
            perror("recvfrom"); // A non-timeout error.
            continue;
        } else {
            // Got a successful response.
            open_ports[ntohs(src_addr.sin_port) - low_port] = true;
            continue;
        }
    }
    close(socket_fd);
    std::cout << "\nOpen ports:\n";
    for (int i = 0; i < port_count; i++) {
        if (open_ports[i]) {
            std::cout << i + low_port << '\n';
        }
    }
}
