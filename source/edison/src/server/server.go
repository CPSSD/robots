package server

import (
	"RobotDriverProtocol"
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
	RobotX  int      `json:"robotX"`
	RobotY  int      `json:"robotY"`
}

type response struct {
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

var responseHistory []response

func load(filename string) *page {
	body, _ := ioutil.ReadFile(filename)
	return &page{Title: filename, Body: body}
}

func indexHandler(w http.ResponseWriter, r *http.Request) {
	page := load("../server/public/index.html")
	fmt.Fprintf(w, "%s", page.Body)
}

func driveHandler(w http.ResponseWriter, r *http.Request) {
	var page *page
	if r.URL.Path == "/drive/" {
		page = load("../server/public/manualControl.html")
	} else {
		page = load(("../server/public/" + r.URL.Path[len("/drive/"):]))
	}
	fmt.Fprintf(w, "%s", page.Body)
}

func scanCommand(w http.ResponseWriter, r *http.Request) {
	outputManager("Scan Command", 0, 0)
	RobotDriverProtocol.Scan()
}

// The URL should contain the filename to save the map to: /saveMap/<fileName>
func saveMap(w http.ResponseWriter, r *http.Request) {
	fileName := r.URL.Path[len("/saveMap/"):]
	maps.RobotMap.SaveMap(fileName)
}

// The URL should contain the filename to load the map from: /saveMap/<fileName>
func loadMap(w http.ResponseWriter, r *http.Request) {
	fileName := r.URL.Path[len("/loadMap/"):]
	maps.RobotMap.LoadMap(fileName)
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

// X first, then Y: ie.. /setDestination/<X>/<Y> where X and Y are the X and Y co-ordinates of the destination on the map
func destinationHandler(w http.ResponseWriter, r *http.Request) {
	data := strings.Split(r.URL.Path[len("/setDestination/"):], "/")
	X, _ := strconv.Atoi(data[0])
	Y, _ := strconv.Atoi(data[1])
	path, possible := maps.GetRoute(maps.RobotMap, X, Y)
	if possible {
		fmt.Fprintf(w, "Moving robot to (%d, %d)", X, Y)
		go maps.RobotMap.MoveRobotAlongPath(path, false)
	} else {
		fmt.Fprintf(w, "Path to (%d, %d) is not possible", X, Y)
	}
}

func moveResponseDisplay(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "{\"Responses\": [")
	for i, response := range responseHistory {
		if i != 0 {
			fmt.Fprintf(w, ",")
		}
		fmt.Fprintf(w, "{ \"Type\": \"%s\", \"Time\": \"%s\", \"UniqueID\": %d, \"Angle\": %d, \"Magnitude\": %d }", response.Type, response.Time, i, response.Angle, response.Distance)

	}
	fmt.Fprintf(w, "]}")
}

func displayBitmap(w http.ResponseWriter, r *http.Request) {
	bitmap, _ := maps.RobotMap.GetBitmap()
	var jsonmap = jsonMap{Code: "200", Message: "ok", Map: bitmap,
		RobotX: int(maps.RobotMap.GetRobot().GetX()), RobotY: int(maps.RobotMap.GetRobot().GetY())}

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

func outputManager(commandType string, angle int, magnitude int) {
	response := response{commandType, 0, int(angle), int(magnitude), getTime()}
	responseHistory = append(responseHistory, response)
}

func killHandler(w http.ResponseWriter, r *http.Request) {
	os.Exit(0)
}

// ResponseHandler should be called when a response is received over SPI
func ResponseHandler(data interface{}) {
	fmt.Println("Data Recieved: ", data)

	switch response := data.(type) {
	case RobotDriverProtocol.MoveResponse:
		outputManager("Move Response", int(response.Angle), int(response.Magnitude))
	case RobotDriverProtocol.ScanResponse:
		outputManager("Scan Response", int(response.Degree), int(response.Distance))
	}
}

func sendMoveCommand(command moveCommand) {
	outputManager("Move Command", command.Angle, command.Distance)
	RobotDriverProtocol.Move(uint16(command.Angle), uint32(command.Distance))
}

// StartServer starts a server for the robots web API
func StartServer() {
	fmt.Println("Starting...")

	RobotDriverProtocol.RegisterResponseHandler(ResponseHandler)

	http.HandleFunc("/", indexHandler)
	http.HandleFunc("/kill/", killHandler)
	http.HandleFunc("/drive/", driveHandler)
	http.HandleFunc("/drive/submit/", driveCommand)
	http.HandleFunc("/drive/response/", moveResponseDisplay)
	http.HandleFunc("/scan/", scanCommand)
	http.HandleFunc("/setDestination/", destinationHandler)
	http.HandleFunc("/map/bit", displayBitmap)
	http.HandleFunc("/saveMap/", saveMap)
	http.HandleFunc("/loadMap/", loadMap)
	http.ListenAndServe(":8080", nil)
}
