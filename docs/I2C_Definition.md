# Commands:

#### I2C communication

- The Mega can send a command to a Slave device connected to the I2C bus at any time.
- The Slave devices are polled by the Mega for responses.

#### Mega -> Uno Commands
```

Parameter Name 	Size in bytes	Parameter Type			Notes
-------------------------------------------
- move 		(Command #1)
Length          - 1 byte      (Unsigned 8 bit int)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Angle 			- 2 bytes		(Unsigned 16 bit int) 	Number from 0 - 3,600 in 10ths of a degree
Magnitude 		- 4 bytes		(Unsigned 32 bit int)	Measured in cm

- stop 		(Command #2)
Length          - 1 byte      (Unsigned 8 bit int)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)

- rotate 	(Command #3)
Length          - 1 byte      (Unsigned 8 bit int)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Angle 			- 2 bytes		(Unsigned 16 bit int) 	Number from 0 - 3,600 in 10ths of a degree

```


#### Uno -> Mega Responses
```

Parameter Name 	Size in bytes	Parameter Type  Notes
---------------------------------------------------
- move  (Response #1)
Length          - 1 byte  (Unsigned 8 bit int)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Magnitude 		- 4 bytes		(Unsigned 32 bit int)	Distance moved measured in cm.
Angle           - 2 bytes (Unsigned 16 bit int) Angle moved at in 10ths of a degree.
Status          - 1 byte  (Unisgned 8 bit int ) 0 for failure, non-zero for success.

- stop  (Response #2)
Length          - 1 byte (Unsigned 8 bit int)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Magnitude 		- 4 bytes		(Unsigned 32 bit int)	Distance moved measured in cm.
Angle           - 2 bytes (Unsigned 16 bit int) Angle moved at in 10ths of a degree.
Status          - 1 byte  (Unisgned 8 bit int ) 0 for failure, non-zero for success.

- rotate  (Response #3)
Length          - 1 byte  (Unsigned 8 bit int)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Angle           - 2 bytes (Unsigned 16 bit int) Angle rotated in 10ths of a degree.
Status          - 1 byte  (Unisgned 8 bit int ) 0 for failure, non-zero for success.

```
