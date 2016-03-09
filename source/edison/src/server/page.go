package main

import (
	"fmt"
	"os"
	"io/ioutil"
	"net/http"
	"strings"
	"strconv"
	"time"
	"RobotDriverProtocol"
)

type MoveResponse struct {
	Angle int
	Distance int
	UniqueID int
}

type MoveCommand struct {
	Angle int
	Distance int
}

type Page struct {
	Title string
	Body []byte
}

func load(filename string) *Page {
	body, _ := ioutil.ReadFile(filename)
	return &Page{Title: filename, Body: body}
}

func driveHandler(w http.ResponseWriter, r *http.Request) {
	var page *Page
	if r.URL.Path == "/drive/"{
		page = load("public/index.php")
	} else {
		page = load(("public/" + r.URL.Path[len("/drive/"):]))
	}
	fmt.Fprintf(w, "%s", page.Body)
}

// Angle first, then distance: ie.. /drive/submit/<angle>/<distance>
func driveCommand(w http.ResponseWriter, r *http.Request) {
	data := strings.Split(r.URL.Path[len("/drive/response/"):], "/")
	angle, _ := strconv.Atoi(data[0])
	distance, _ := strconv.Atoi(data[1])
	cmd := MoveCommand{angle, distance}
	sendMoveCommand(cmd)
	fmt.Fprintf(w, "Sending move command: %d/%d, (angle/distance)", angle, distance)
}

func moveResponse(angle int, magnitude int) {
	// To be added. Will try pinging a page containing json from the website, 
	// and if it has an update i'll pull it and display on the page
}

func killHandler(w http.ResponseWriter, r*http.Request) {
	os.Exit(0)
}

func sendMoveCommand(command MoveCommand) {
	fmt.Printf("Sending move command: %d/%d, (angle/distance)", command.Angle, command.Distance)
	RobotDriverProtocol.Move(command.angle, command.distance)
}

func main() {
	fmt.Println("Starting...")
	
	http.HandleFunc("/kill/", killHandler)
	http.HandleFunc("/drive/", driveHandler)
	http.HandleFunc("/drive/submit/", driveCommand)
	http.ListenAndServe(":8080", nil)

}