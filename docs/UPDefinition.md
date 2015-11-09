User Protocol Definition
=========================

## Introduction ##

This document will outline the User Protocol API.

The API will be made using RESTful principles with HTTP and Json.


## Functionality ##

| Uri | Http Method | Post Body  |   Option   | Response  |
|-----|:-----------:|:----------:|:----------:|:---------:|
| Map | GET         | Json Object| bit/vector |200/404/405|

| Status Code | Description |
|-------------|-------------|
|   200       |      Ok     |
|   404       | No Map Found|
|   405       | Unknown Uri |


## Examples ##

Example: A square 4x4 room in vector form

http://ip_address/map=vector

Response: 
```
'{
    "code" : "200",
    "message" : "ok"
    "map":[
    {"x1":"0", "y1":"0", "x2":"0", "y2":"4"}, 
    {"x1":"0", "y1":"4", "x2":"4", "y2":"4"}, 
    {"x1":"4", "y1":"4", "x2":"4", "y2":"0"},
    {"x1":"4", "y1":"0", "x2":"0", "y2":"0"}
]
}'
```

