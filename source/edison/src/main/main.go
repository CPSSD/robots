package main

import (
	"mapping/maps"
	"server"
	"flag"
)

var bitmapScale = flag.Int("scale", 10, "Scale for the bitmap")

func main() {
	flag.Parse()
	maps.MapInit(*bitmapScale)
	server.StartServer()
}
