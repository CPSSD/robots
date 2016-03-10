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
	Type string
	UniqueID int
	Angle int
	Distance int
	Time string
}

type MoveCommand struct {
	Angle int
	Distance int
}

type Page struct {
	Title string
	Body []byte
}

var moveResponseHistory []MoveResponse

func load(filename string) *Page {
	body, _ := ioutil.ReadFile(filename)
	return &Page{Title: filename, Body: body}
}

func driveHandler(w http.ResponseWriter, r *http.Request) {
	var page *Page
	if r.URL.Path == "/drive/"{
		page = load("public/index.html")
	} else {
		page = load(("public/" + r.URL.Path[len("/drive/"):]))
	}
	fmt.Fprintf(w, "%s", page.Body)
}

// Angle first, then distance: ie.. /drive/submit/<angle>/<distance>
func driveCommand(w http.ResponseWriter, r *http.Request) {
	data := strings.Split(r.URL.Path[len("/drive/submit/"):], "/")
	angle, _ := strconv.Atoi(data[0])
	distance, _ := strconv.Atoi(data[1])
	cmd := MoveCommand{angle, distance}
	sendMoveCommand(cmd)
	fmt.Fprintf(w, "Sending move command: %d/%d, (angle/distance)", angle, distance)
}

func moveResponseDisplay(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "{\"Responses\": [")
	for i, response := range moveResponseHistory {
		if i != 0 {
			fmt.Fprintf(w, ",")
		}
		fmt.Fprintf(w, "{ \"Type\": \"%s\", \"Time\": \"%s\", \"UniqueID\": %d, \"Angle\": %d, \"Magnitude\": %d }", response.Type, response.Time, i, response.Angle, response.Distance)
		
	}
	fmt.Fprintf(w, "]}")
}

func getTime() (timeResp string) {
	curTime := time.Now()

	hour := curTime.Hour()
	if hour < 10 {
		timeResp += "0"
	} 
	timeResp += fmt.Sprintf("%d", hour) 
	timeResp += ":"

	minute := curTime.Minute()
	if minute < 10 {
		timeResp += "0"
	} 
	timeResp += fmt.Sprintf("%d", minute) 
	timeResp += ":"

	second := curTime.Second()
	if second < 10 {
		timeResp += "0"
	} 
	timeResp += fmt.Sprintf("%d", second) 
	return
}

func moveResponse(commandType string, angle int, magnitude int) {
	response := MoveResponse {commandType, 0, int(angle), int(magnitude), getTime()}
	moveResponseHistory = append(moveResponseHistory, response)
}

func killHandler(w http.ResponseWriter, r*http.Request) {
	os.Exit(0)
}

func responseHandler(data interface{}) {
	fmt.Println("Data Recieved: ", data)

	switch response := data.(type) {
	case RobotDriverProtocol.MoveResponse:
		moveResponse("Move Response", int(response.Angle), int(response.Magnitude))
	}
}

func sendMoveCommand(command MoveCommand) {
	moveResponse("Move Command", command.Angle, command.Distance)
	RobotDriverProtocol.Move(uint16(command.Angle), uint32(command.Distance))
}

func main() {
	fmt.Println("Starting...")

	RobotDriverProtocol.RegisterResponseHandler(responseHandler)
	RobotDriverProtocol.Init()
	
	http.HandleFunc("/kill/", killHandler)
	http.HandleFunc("/drive/", driveHandler)
	http.HandleFunc("/drive/submit/", driveCommand)
	http.HandleFunc("/drive/response/", moveResponseDisplay)
	http.ListenAndServe(":8080", nil)

}