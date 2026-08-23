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
    const char* ip_addr = argv[1]; // TODO: check for bounds
    // Try to make an IPv4 UDP socket.
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Error creating socket!");
        exit(1);
    }
    std::string s = "5+6";

    struct sockaddr_in dest_addr;

    // Set address family to IPv4, and port number to 4006.
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port =
        htons(4006); // HACK: take port number from  command line argument.
    if (inet_pton(AF_INET, ip_addr, (void*)&dest_addr.sin_addr) < 1) {
        std::cerr << "invalid ip address or address family" << ip_addr << '\n';
        exit(1);
    }

    // Send the string to the destination address.
    ssize_t ret = sendto(socket_fd, s.c_str(), s.length(), 0,
                         (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (ret < 0) {
        perror("Error sending");
    }

    struct sockaddr_in src_addr;
    socklen_t src_addr_len;
    char data_buffer[2048];

    if ((ret = recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,
                        (struct sockaddr*)&src_addr, &src_addr_len) < 0)) {
        perror("Error receiving");
        exit(1);
    }

    std::cout << "received: " << data_buffer << '\n';
    close(socket_fd);
}

ssize_t sendto_wrapper(std::string& data) {
    return 3;
}
