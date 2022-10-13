#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080
using namespace std;

void read_file_and_send(int sever_fd, string filename)
{
    cout << "[*]Initiating file transfer\n";
    const char *file_str = filename.c_str();
    FILE *f = fopen(file_str, "rb");
    if (f == NULL)
    {
        perror("[-]Error opening the file\n");
        exit(EXIT_FAILURE);
    }
    char file_buffer[8192] = {0};
    while (!feof(f))
    {
        cout << "[*]sending a block...\n";
        memset(file_buffer, 0, 1024);
        int read_length = fread(file_buffer, 1, 1024, f);
        send(sever_fd, file_buffer, read_length, 0);
    }
    fclose(f);
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
    read_file_and_send(server_fd, "sample.mkv");
    close(server_connection);
    return 0;
}