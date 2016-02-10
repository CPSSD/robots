package main
import(
	"log"
	"net/http"
	"server"
	"mapping/maps"
)

func main() {
	
	maps.MapInit()
	router := server.NewRouter()

	log.Fatal(http.ListenAndServe(":8080",router))	
}
