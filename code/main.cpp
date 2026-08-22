#include <iostream>
#include <stdio.h>
#include <string>

#include <arpa/inet.h>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, const char* argv[]) {
    const char* ipaddr = argv[1]; // TODO: check for bounds
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Error creating socket!");
        exit(1);
    }
    std::string s = "5+6";

    // TODO: make dat damn destaddr

    struct sockaddr_in destaddr;

    destaddr.sin_family = AF_INET;
    destaddr.sin_port =
        htons(4006); // HACK: take port number from  command line argument.
    if (inet_pton(AF_INET, ipaddr, (void*)&destaddr.sin_addr) < 1) {
        std::cerr << "invalid ip address or address family" << ipaddr << '\n';
        exit(1);
    }

    int ret;
    if ((ret = sendto(socket_fd, s.c_str(), s.length(), 0,
                      (struct sockaddr*)&destaddr, sizeof(destaddr))) < 0) {
        perror("Error sending");
    }

    struct sockaddr_in srcaddr;
    socklen_t srcaddrlen;
    char buffer[2048];

    if ((ret = recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                        (struct sockaddr*)&srcaddr, &srcaddrlen) < 0)) {
        perror("Error receiving");
        exit(1);
    }

    std::cout << "received: " << buffer << '\n';
}
