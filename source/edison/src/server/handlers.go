//File for handlers for the server calls

package server

import (
	"encoding/json"
	"fmt"
	"net/http"

	"mapping/maps"
)

//json struct
type JsonMap struct {
	Code string 	`json:"code"`
	Message string	`json:"message"`
	Map [][]bool	`json:"map"`
}

func Index(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintln(w, "Welcome!")
}

// sends json to web client
func BitmapIndex(w http.ResponseWriter, r *http.Request){

	//RobotMap := maps.RobotMap
	//RobotMap := maps.CreateMap()
	//RobotMap.DrawSquare(4)
	

	bitmap, _ := maps.RobotMap.GetBitmap()
	var jsonmap JsonMap = JsonMap{Code: "200", Message: "ok", Map: bitmap}

	w.Header().Set("Content-Type","application/json;charset=UTF-8")
	w.Header().Set("Access-Control-Allow-Origin","*")
	w.WriteHeader(http.StatusOK)

	if err := json.NewEncoder(w).Encode(jsonmap); err != nil {
    	panic(err)
    }
}

//not implemented correctly yet

/*func VectormapIndex(w http.ResponseWriter, r *http.Request){

	RobotMap := maps.CreateMap()
	RobotMap.DrawSquare(4)
	RobotMap.GetBitmap()
	var vectorMap [][]RobotMap.BitmapToVector()


}*/