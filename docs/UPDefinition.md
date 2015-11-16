User Protocol Definition
=========================

## Introduction ##

This document will outline the User Protocol API.

The API will be made using RESTful principles with HTTP and Json.


## Functionality ##

| Uri | Http Method |   Option   | Response  |
|-----|:-----------:|:----------:|:---------:|
| Map | GET         | bit/vector |Json object|

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
    [0,0,0,4], 
    [0,4,4,4],
    [4,4,0,4],
    [0,4,0,0],
]
}'
```

