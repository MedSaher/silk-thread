package main

import (
	"fmt"
	"net"
)

func main() {
	listener, err := net.Listen("tcp", ":999")
	if err != nil {
		fmt.Println(err)
		return
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println(err)
			return
		}
		fmt.Println(conn.LocalAddr().String())
	}
}
