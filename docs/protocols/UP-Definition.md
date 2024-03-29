User Protocol Definition
=========================

## Introduction ##

This document will outline the User Protocol API.

The API will be made using RESTful principles with HTTP and Json.


## Functionality ##

| Uri  | Http Method |   Option                  | Response  |
|------|:-----------:|:-------------------------:|:---------:|
| map  | GET         | bit                       |Json object|
| drive| GET         |                           |index.html |
| drive| GET         | submit/<angle>/<distance> |Plain Text |
| drive| GET         | response                  |Json object|
| kill | GET         |                           |None       |

| Status Code | Description    |
|-------------|----------------|
|   200       |       Ok       |
|   404       | Page not found |
|   405       |  No Map Found  |



## Examples ##

Example: A square 4x4 room in vector form

http://ip_address/map/bit

Response: 
```
'{
    "code" : "200",
    "message" : "ok",
    "map":[
    [true,true,true,true], 
    [true,false,false,true],
    [true,false,false,true],
    [true,true,true,true]
]
}'
```

