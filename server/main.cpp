#include <iostream>
#include <thread>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "header.hpp"

void receive_loop(int client_socket)
{
    char buffer[1024];
    while (true)
    {
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
        {
            std::cerr << "\nClient disconnected.\n";
            close(client_socket);
            break;
        }
        buffer[bytes] = '\0';
        {
            std::cout << "\033[31mClient: \033[37m" << buffer << "\033[32mYou > \033[37m" << std::flush;
        }
    }
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    std::cout << "Server listening on port 8888...\n";

    sockaddr_in client_addr{};
    socklen_t addrlen = sizeof(client_addr);
    int client_socket = accept(server_fd, (sockaddr *)&client_addr, &addrlen);
    std::cout << "Client connected.\n";

    // set commands
    std::map<std::string, bool> *cmds = SetCommands();

    std::cout << "\033[32mYou > \033[37m";
    std::thread receiver(receive_loop, client_socket);

    while (true)
    {
        std::string message;
        std::getline(std::cin, message);
        if (message == "exit")
            break;
        if (message == "clear")
        {
            std::cout << "\033[2J\033[H\033[32mYou > \033[37m"; // ANSI escape sequence to clear screen and move cursor to top
            continue;                     // No need to execute anything else
        }
        if (is_valid_command(cmds, message))
        {
            send(client_socket, message.c_str(), message.length(), 0);
        }
        else
        {
            std::cout << "Invalid Command\n\033[32mYou > \033[37m";
        }
    }

    close(client_socket);
    receiver.detach(); // Or receiver.join()
    close(server_fd);
    return 0;
}
