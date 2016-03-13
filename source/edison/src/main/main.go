package main

import (
	"mapping/maps"
	"server"
)

func main() {
	maps.MapInit()
	server.StartServer()
}
