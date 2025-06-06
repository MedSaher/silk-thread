package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"os"
	"strconv"
	"strings"
)

func main() {
	listener, err := net.Listen("tcp", ":999")
	if err != nil {
		fmt.Println("Error listening:", err)
		return
	}
	defer listener.Close()

	conn, err := listener.Accept()
	if err != nil {
		fmt.Println("Error accepting connection:", err)
		return
	}
	defer conn.Close()

	reader := bufio.NewReader(conn)
	stdinReader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("shell> ")
		cmd, _ := stdinReader.ReadString('\n')
		cmd = strings.TrimSpace(cmd)

		if cmd == "exit" {
			fmt.Fprintln(conn, "exit")
			break
		}

		// Send command to client
		fmt.Fprintln(conn, cmd)

		// First read the length of the output
		lengthStr, err := reader.ReadString('\n')
		if err != nil {
			fmt.Println("Error reading length:", err)
			return
		}

		length, err := strconv.Atoi(strings.TrimSpace(lengthStr))
		if err != nil {
			fmt.Println("Invalid length received:", err)
			return
		}

		output := make([]byte, length)
		_, err = io.ReadFull(reader, output)
		if err != nil {
			fmt.Println("Error reading output:", err)
			return
		}

		fmt.Printf("client: %s\n", output)
	}
}
