
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <cstdlib>
#include <string>

int main(int argc, const char* argv[]) {
    

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <IPv4 address>" << std::endl;
        return EXIT_FAILURE;
    }
    const char *ipaddr = argv[1];

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creating socket");
        exit(1);
    }

    std::string s = "3*7";

    struct sockaddr_in destaddr;
    destaddr.sin_family = AF_INET;
    destaddr.sin_port = htons(4001); // TODO: take port number from command line arguments
    if ((inet_pton(AF_INET, ipaddr, &destaddr.sin_addr)) < 1) {
        std::cerr << "invalid ip address or address family: " << ipaddr << std::endl;
        exit(1);
    }

    int ret;
    if ((ret = (sendto(sockfd, &s, sizeof(s), 0 /*flags*/, (struct sockaddr*)&destaddr, sizeof(destaddr)))) < 0) {
        perror("Error sending");
        exit(1);
    }

    struct sockaddr_in srcaddr;
    socklen_t srcaddrlen;
    char buffer[2048];

    if ((ret = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&srcaddr, &srcaddrlen)) < 0) {
        perror("Error recieving");
        exit(1);
    }
    std::cout << "recieved: " << buffer << std::endl;
}
