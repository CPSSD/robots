# Commands:

#### I2C communication

- Mega sends the byte 0xFF and an identifier for the Uno it will be sending the next command to.
- Mega and Uno transfer the length of the command they have to send (or 0 for no command).
- Transfer command (followed by null bytes if length of other command exceeds length of command).
- One byte indicating if the device has more commands to send. 0 for "Finished", 1 for "Not Finished". If not finished repeat the process.

#### Mega -> Uno Commands
```

Parameter Name 	Size in bytes	Parameter Type			Notes
-------------------------------------------
- move 		(Command #1)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Angle 			- 2 bytes		(Unsigned 16 bit int) 	Number from 0 - 3,600 in 10ths of a degree
Magnitude 		- 2 bytes		(Unsigned 16 bit int)	Measured in cm
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

- stop 		(Command #2)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

- rotate 	(Command #3)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Angle 			- 2 bytes		(Unsigned 16 bit int) 	Number from 0 - 3,600 in 10ths of a degree
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

```


#### Uno -> Mega Responses
```

Parameter Name 	Size in bytes	Parameter Type  Notes
---------------------------------------------------
- move  (Response #1)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Magnitude 		- 2 bytes		(Unsigned 16 bit int)	Distance moved measured in cm.
Angle           - 2 bytes (Unsigned 16 bit int) Angle moved at in 10ths of a degree.
Status          - 1 byte  (Unisgned 8 bit int ) 0 for failure, non-zero for success.

- stop  (Response #2)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Magnitude 		- 2 bytes		(Unsigned 16 bit int)	Distance moved measured in cm.
Angle           - 2 bytes (Unsigned 16 bit int) Angle moved at in 10ths of a degree.
Status          - 1 byte  (Unisgned 8 bit int ) 0 for failure, non-zero for success.

- rotate  (Response #3)
Command Number  - 1 byte  (Unsigned 8  bit int)	
Unique ID       - 2 bytes (Unsigned 16 bit int)
Angle           - 2 bytes (Unsigned 16 bit int) Angle rotated in 10ths of a degree.
Status          - 1 byte  (Unisgned 8 bit int ) 0 for failure, non-zero for success.

```
