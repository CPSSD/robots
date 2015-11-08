User Protocol Definition
=========================

## Introduction ##

This document will outline the User Protocol API.

The API will be made using RESTful principles with HTTP and Json.


## Functionality ##

| Uri | Http Method | Post Body  | Response |
|-----|:-----------:|:----------:|:--------:|
| Map | POST        | Json Array | 200/405  |

###Status Code   |  Description###
    200             OK
    404         Unknown Uri


## Examples ##

Example: A square 4x4 room 

http://ip_address/map
```
"map":[
    {"x1":"0", "y1":"0", "x2":"0", "y2":"4"}, 
    {"x1":"0", "y1":"4", "x2":"4", "y2":"4"}, 
    {"x1":"4", "y1":"4", "x2":"4", "y2":"0"},
    {"x1":"4", "y1":"0", "x2":"0", "y2":"0"}
]
```

Response: 
'{
    "code" : "200",
    "message" : "ok"
}'
    

