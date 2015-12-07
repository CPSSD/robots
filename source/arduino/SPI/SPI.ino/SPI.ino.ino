// Plz work SPI... Plz
// Some kind of test file to run the SPI library.
#include <SPI.h>

volatile byte in_buf[100]; // Stores the received bytes.
volatile byte in_pos;  // What position through the byte we are currently reading.
volatile byte out_buf[100];  // Stores the next SPI transfer's data
volatile byte out_pos;  // What position through the byte array we are.

volatile int bytes_left_to_read; // -1 if not in a command.
volatile boolean process_it;  // Received the total command. - start processing.


void setup() {    
  Serial.begin(9600);
  SPISettings(100000, MSBFIRST, SPI_MODE0);
  
  // turn on SPI in slave mode
  SPCR |= bit (SPE);

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // get ready for an interrupt 
  in_pos = 0;   // buffer empty
  out_pos = 0;
  process_it = false;
  bytes_left_to_read = -1;
  
  
  char temp[] = "lol dis won't work...";
  for(int i=0; i<100; i++)
    *(out_buf+i) = 0;
  for(int i=0; i<sizeof(temp)/(sizeof(temp[0])); i++)
    *(out_buf+i) = *(temp+i);
    
  // now turn on interrupts
  SPI.attachInterrupt();
}


void zero_array(byte* array_pointer) {
  memset(array_pointer, 0, sizeof array_pointer);
}


// SPI interrupt routine
ISR(SPI_STC_vect) {
  in_buf[in_pos++] = SPDR;  // grab byte from SPI Data Register
  
  if(bytes_left_to_read <= 0 && (in_buf[in_pos-1] == 255 || in_buf[in_pos-1] == 1)) { // Sentinel byte.
    bytes_left_to_read = -1;
    in_pos = 0;
    // TODO: Set SPDR to length we have to send here.
    
    return;
  }
  
  if(bytes_left_to_read == -1) { // We're starting to read a command.
    bytes_left_to_read = in_buf[in_pos-1];
  }
  
  if(bytes_left_to_read == 0 && in_buf[in_pos-1] == 0) { // Command finished and no more left to send.
    process_it = true;
    
    out_pos = 0;
  }
  
  Serial.println("Just received: " + in_buf[in_pos-1]);
  Serial.println(bytes_left_to_read + " bytes left to receive.");

  SPDR = out_buf[out_pos++];
  Serial.println("Sending next clock cycle: " + out_buf[out_pos-1]);
}


// main loop - wait for flag set in interrupt routine
void loop(void) {
  if(process_it) {
    
    int command_size = in_buf[0];
    in_buf = in_buf<<1;  // Read length and shift it out.
    
    int command_number = in_buf[0];
    in_buf <<= 1;
    
    int uid = in_buf << 2;
    in_buf <<= 2;
    
    switch(command_number) {
       default:
         Serial.println("We're fucked...");
         break;
         
       case 1:
         Serial.println("Move...");
         break;
         
       case 2:
         Serial.println("Stop...");
         break;
         
       case 3:
         Serial.println("Rotate...");
         break;
         
       case 4:
         Serial.println("Scan...");
         break;
    }
 
 
    Serial.print("in_buf: ");
    for(int i=0; i<in_pos; i++)
      Serial.print(in_buf[i]);
    Serial.println();

    // TODO: All buffer parsing here.
    process_it = false;
  }
}

