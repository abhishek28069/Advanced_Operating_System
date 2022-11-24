#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8080
#define BUF_SIZE 524288
using namespace std;

void write_file_from_client(int socket_of_request, string dest)
{
    char read_buffer[BUF_SIZE] = {0};
    int d = open(dest.c_str(), O_WRONLY | O_CREAT, 0644);
    int size_of_message;
    while (1)
    {
        size_of_message = read(socket_of_request, read_buffer, BUF_SIZE);
        if (size_of_message <= 0)
        {
            cout << "[*]Finished writing messaage\n";
            break;
        }
        cout << "[*]Block of " << size_of_message << " recieved from clent\n";
        write(d, read_buffer, size_of_message);
    }
    close(d);
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
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    server_connection = bind(server_fd, (const sockaddr *)&server_addr, (socklen_t)addr_len);
    if (server_connection < 0)
    {
        perror("[-]Created Socket failed to bind\n");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        perror("[-]Server failed to listen\n");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        cout << "[*]Server waiting for a request...\n";
        int socket_of_request;
        socket_of_request = accept(server_fd, (sockaddr *)&server_addr, (socklen_t *)&addr_len);
        if (socket_of_request < 0)
        {
            perror("[-]Error while accepting request\n");
            exit(EXIT_FAILURE);
        }
        write_file_from_client(socket_of_request, "dest.txt");
    }
    return 0;
}