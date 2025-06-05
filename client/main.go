package main

import (
	"bufio"
	"fmt"
	"net"
)

func main() {
	conn, err := net.Dial("tcp", ":999")
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Fprint(conn, "hello world")
	status, err := bufio.NewReader(conn).ReadString('\n')
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println(status)
}
