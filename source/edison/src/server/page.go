package main

import (
	"fmt"
	"os"
	"io/ioutil"
	"net/http"
	"strings"
	"strconv"
	"RobotDriverProtocol"
)

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
	if r.URL.Path == "/drive/"{
		page := load("public/index.php")
	} else {
		page := load(("public/" + r.URL.Path[len("/drive/"):]))
	}
	fmt.Fprintf(w, "%s", page.Body)
}

// Angle first, then distance: ie.. /drive/response/<angle>/<distance>
func driveResponse(w http.ResponseWriter, r *http.Request) {
	data := strings.Split(r.URL.Path[len("/drive/response/"):], "/")
	angle, _ := strconv.Atoi(data[0])
	distance, _ := strconv.Atoi(data[1])
	cmd := MoveCommand{angle, distance}
	sendMoveCommand(cmd)
	fmt.Fprintf(w, "Sending move command: %d/%d, (angle/distance)", angle, distance)
}

func killHandler(w http.ResponseWriter, r*http.Request) {
	os.Exit(0)
}

func sendMoveCommand(command MoveCommand){
	fmt.Printf("Sending move command: %d/%d, (angle/distance)", command.Angle, command.Distance)
	RobotDriverProtocol.Move(command.angle, command.distance)
}

func main(){
	fmt.Println("Starting...")
	
	http.HandleFunc("/kill/", killHandler)
	http.HandleFunc("/drive/", driveHandler)
	http.HandleFunc("/drive/response/", driveResponse)
	http.ListenAndServe(":8080", nil)

}