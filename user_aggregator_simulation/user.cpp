#include<bits/stdc++.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

void exit_the_process(int sockfd) {
    close(sockfd);
    cout << "Exiting the process..." << endl;
    exit(0);
}

int main() {
    int sockfd;
    sockaddr_in server_addr{};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8111);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // localhost

    if (connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }
    int pk = 2138679817;
    string message = "Hello ";
    message += std::to_string(pk);
    send(sockfd, message.c_str(), message.size(), 0);
    
    char buffer[1024] = {0};
    int bytes_read = read(sockfd, buffer, sizeof(buffer));
    std::cout << "Server reply: " << std::string(buffer, bytes_read) << "\n";

    if(strcmp(buffer, "OK") == 0){
        cout << "OK received" << endl;
    } else {
        cout << "Invalid response" << endl;
        exit_the_process(sockfd);
        return 1;
    }

    close(sockfd);
    return 0;
}
