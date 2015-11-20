# Commands:

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
