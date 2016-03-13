package server

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"mapping/maps"
	"net/http"
	"os"
	"strconv"
	"strings"
	"time"
)

type jsonMap struct {
	Code    string   `json:"code"`
	Message string   `json:"message"`
	Map     [][]bool `json:"map"`
}

type moveResponse struct {
	Type     string
	UniqueID int
	Angle    int
	Distance int
	Time     string
}

type moveCommand struct {
	Angle    int
	Distance int
}

type page struct {
	Title string
	Body  []byte
}

var moveResponseHistory []moveResponse

func load(filename string) *page {
	body, _ := ioutil.ReadFile(filename)
	return &page{Title: filename, Body: body}
}

func driveHandler(w http.ResponseWriter, r *http.Request) {
	var page *page
	if r.URL.Path == "/drive/" {
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
	cmd := moveCommand{angle, distance}
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

func displayBitmap(w http.ResponseWriter, r *http.Request) {
	bitmap, _ := maps.RobotMap.GetBitmap()
	var jsonmap = jsonMap{Code: "200", Message: "ok", Map: bitmap}

	w.Header().Set("Content-Type", "application/json;charset=UTF-8")
	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.WriteHeader(http.StatusOK)

	if err := json.NewEncoder(w).Encode(jsonmap); err != nil {
		log.Fatal(err)
	}
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
	response := moveResponse{commandType, 0, int(angle), int(magnitude), getTime()}
	moveResponseHistory = append(moveResponseHistory, response)
}

func killHandler(w http.ResponseWriter, r *http.Request) {
	os.Exit(0)
}

// ResponseHandler should be called when a response is received over SPI
func ResponseHandler(data interface{}) {
	fmt.Println("Data Recieved: ", data)

	switch response := data.(type) {
	case RobotDriverProtocol.MoveResponse:
		moveResponse("Move Response", int(response.Angle), int(response.Magnitude))
	}
}

func sendMoveCommand(command moveCommand) {
	moveResponse("Move Command", command.Angle, command.Distance)
	RobotDriverProtocol.Move(uint16(command.Angle), uint32(command.Distance))
}

// StartServer starts a server for the robots web API
func StartServer() {
	fmt.Println("Starting...")

	RobotDriverProtocol.RegisterResponseHandler(responseHandler)
	RobotDriverProtocol.Init()

	http.HandleFunc("/kill/", killHandler)
	http.HandleFunc("/drive/", driveHandler)
	http.HandleFunc("/drive/submit/", driveCommand)
	http.HandleFunc("/drive/response/", moveResponseDisplay)
	http.HandleFunc("/map/bit", displayBitmap)
	http.ListenAndServe(":8080", nil)
}
