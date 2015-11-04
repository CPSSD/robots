User Protocol Definition
=========================

## Introduction ##

This document will outline the User Protocol API.

The API will be made using REST principles with HTTP and Json strings.


## Functionality ##

###Uri       Http Method     Post Body              Options              Response###
   Move         POST        Json String        left/right/up/down         200/405
  Rotate        POST        Json String      clockwise/anticlockwise      200/405
   Stop         GET            None                  None                 200/405



###Status Code     Description###
    200             OK
    404         Unknown Uri
    405        Unknown Option


## Examples ##

Example: Move Left

http://ip_address/move

'{
    "move" : "left"
}'

Response: 
'{
    "code" : "200",
    "message" : "ok"
}'
    

