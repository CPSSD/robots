# Commands:

#### SPI communication

- Edison sends the byte 0xFF.
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

- scan 		(Command #4)
Command Number	- 1 byte  		(Unsigned 8  bit int)	
Unique ID 		- 2 bytes		(Unsigned 16 bit int)
Checksum		- 1 byte		(Unsigned 8  bit int) 	Sum of all other bytes mod 255

```

#### Arduino -> Edison Commands
```

```
