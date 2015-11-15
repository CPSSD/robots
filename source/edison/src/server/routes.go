// Router file to send routes to handler file

package main

import (
	"net/http"
)


type Route struct {
	Name        string
	Method      string
	Pattern     string
	HandlerFunc http.HandlerFunc
}

type Routes []Route

// route array
var routes = Routes{
	Route{
		"Index",
		"GET",
		"/",
		Index,
	},
	Route{
		"Bitmap",
		"GET",
		"/map/bit",
		BitmapIndex,
	},
	/*Route{
	  "Vectormap",
		"Get",
		"/map/vector",
		VectormapIndex,
	},*/
}
