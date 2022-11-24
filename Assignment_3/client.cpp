#include <iostream>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUF_SIZE 524288
using namespace std;

void read_file_and_send(int sever_fd, string filename)
{
    cout << "[*]Initiating file transfer\n";
    int f = open(filename.c_str(), O_RDONLY, 0);
    char file_buffer[BUF_SIZE] = {0};
    int read_length;
    while ((read_length = read(f, file_buffer, BUF_SIZE)) > 0)
    {
        cout << "[*]sending a block...\n";
        send(sever_fd, file_buffer, read_length, 0);
    }
    close(f);
    return;
}

int main()
{
    int server_fd, server_connection;
    sockaddr_in server_addr;
    int addr_len = sizeof(server_addr);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("[-]Socket failed to create\n");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    server_connection = connect(server_fd, (const sockaddr *)&server_addr, (socklen_t)addr_len);
    if (server_connection < 0)
    {
        perror("[-]Error while connecting to server\n");
        exit(EXIT_FAILURE);
    }
    read_file_and_send(server_fd, "sample.txt");
    close(server_connection);
    return 0;
}