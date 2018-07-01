const int clock_pin = 2;
const int latch_pin = 3;
const int data_pin = 4;
const int enable_pin = 5;
const int reset_pin = 6;

const int delay_amount = 10;

void setup() {
  pinMode(clock_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(enable_pin, OUTPUT);
  pinMode(reset_pin, OUTPUT);
  
  
  digitalWrite(clock_pin, LOW);
  digitalWrite(latch_pin, LOW);
  digitalWrite(data_pin, LOW);
  digitalWrite(enable_pin, HIGH);
  digitalWrite(reset_pin, LOW);
  
  //Serial.begin(9600);
  
  delay(500);
}

void loop() {
  
    byte segment_array[10] = {
    0b11111100, //0
    0b01100000, //1
    0b11011010, //2
    0b11110010, //3
    0b01100110, //4
    0b10110110, //5
    0b10111110, //6
    0b11100000, //7
    0b11111110, //8
    0b11100110, //9
  };
  
  byte test_byte = 0b10110110;
  
  for(int i = 0; i < 10; i++) {
    digitalWrite(reset_pin, LOW);
    digitalWrite(enable_pin, LOW);
    delay(delay_amount);
    
    for(int j = 8; j > 0; j--) {
      digitalWrite(data_pin, bitRead(test_byte,j));//segment_array[i],j));
      delay(delay_amount);
      
      digitalWrite(clock_pin, HIGH);
      delay(delay_amount);
      digitalWrite(clock_pin, LOW);
      
    }
    
    digitalWrite(latch_pin, HIGH);
    delay(delay_amount);
    digitalWrite(latch_pin, LOW);
    delay(delay_amount);
    
    digitalWrite(enable_pin, HIGH);
    
    delay(1000);
  }
  
}
