# Commands:

#### SPI communication

- Edison sends the byte 147.
- Edison and Arduino transfer the length of the command they have to send (or 0 for no command).
- Transfer command (followed by null bytes if length of other command exceeds length of command).
- One byte indicating if the device has more commands to send. 0 for "Finished", 1 for "Not Finished". If not finished repeat the process.

#### Edison -> Arduino Commands
```

Parameter Name 	Size in bytes	Parameter Type			Notes
-------------------------------------------
- move 		(Command #1)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Angle 			- 2 bytes		(Unsigned 16 bit int) 	Number from 0 - 360 in degrees
Magnitude 		- 2 bytes		(Unsigned 16 bit int)	Measured in cm
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

- stop 		(Command #2)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

- rotate 	(Command #3)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Angle 			- 2 bytes		(Unsigned 16 bit int) 	Number from 0 - 360 in degrees
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

- scan 		(Command #4)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

- getHeading (Command #5)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
```


#### Arduino -> Edison Responses
```

Parameter Name 	Size in bytes	Parameter Type  Notes
---------------------------------------------------
- move  (Response #1)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Magnitude 		- 2 bytes (Unsigned 16 bit int)	Distance moved measured in cm.
Angle           - 2 bytes (Unsigned 16 bit int) Angle moved at in 10ths of a degree.
Status          - 1 byte  (Unsigned 8 bit int ) 0 for failure, non-zero for success.

- stop  (Response #2)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Magnitude 		- 2 bytes (Unsigned 16 bit int)	Distance moved measured in cm.
Angle           - 2 bytes (Unsigned 16 bit int) Angle moved at in 10ths of a degree.
Status          - 1 byte  (Unsigned 8 bit int ) 0 for failure, non-zero for success.

- rotate  (Response #3)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Angle           - 2 bytes (Unsigned 16 bit int) Angle rotated in 10ths of a degree.
Status          - 1 byte  (Unsigned 8 bit int ) 0 for failure, non-zero for success.

- scan  (Response #4)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Last            - 1 byte  (8 bit boolean value) 0 for false, anything else for true.
Angle           - 2 bytes (Unsigned 16 bit int) Angle in degrees
Distance        - 2 bytes (Unsigned 16 bit int) Distance in cm.
Status          - 1 byte  (Unsigned 8 bit int ) 0 for failure, non-zero for success.

- compassResponse (Response #5)
Command Number	- 1 byte  	(Unsigned 8  bit int)	
Unique ID 		- 2 bytes	(Unsigned 16 bit int)
Angle			- 2 bytes 	(Unsigned 16 bit int)
Status          - 1 byte  	(Unsigned 8 bit int ) 0 for failure, non-zero for success.

```
