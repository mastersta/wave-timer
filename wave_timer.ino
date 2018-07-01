// Prototype Wave Timer code
// Created by Tyler Masters

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();


const int relay_pin      = 13;
const int set_on_pin     = A2;
const int set_off_pin    = A3;
const int clock_pin      =  4;
const int load_in_pin    =  5;
const int data_in_pin    =  6;
const int latch_in_pin   =  7;
const int data_out_pin   =  8;
const int load_out_pin   =  9;
const int bell_pin       = 10;


unsigned long time_on = 10;  // time relay closed and waves on (in minutes)
unsigned long time_off = 15;  // time relay open and waves off (in minutes)
const unsigned long minutes_to_millis = 60000; //constant to convert minutes into millis

int button_set_on_state = 0;
int button_set_off_state = 0;

const boolean debug_mode = false;
unsigned long debug_timer = 0;
const unsigned long debug_write_interval = 1000;
unsigned long current_timer = 0;

int relay_state = LOW;
unsigned long relay_timer = 0;

boolean ring_bell = LOW;
unsigned long bell_timer = 0;
unsigned long bell_ring_length = 2000;

void setup() {

  pinMode(relay_pin, OUTPUT);      //pin that controls relay to operate waves
  pinMode(set_on_pin, INPUT);      //pin that reads the set-on button
  pinMode(set_off_pin, INPUT);     //pin that reads the set-off button
  pinMode(clock_pin, OUTPUT);      //clock for time input and display output
  pinMode(load_pin, OUTPUT);       //load pin for time input
  pinMode(data_pin, INPUT);        //data pin for time input
  pinMode(data_out_pin, OUTPUT);   //data pin for display output
  pinMode(load_out_pin, OUTPUT);   //load pin for display output
  pinMode(bell_pin, OUTPUT);       //pin that controls relay to operate bell

  digitalWrite(relay_pin, LOW);
  digitalWrite(clock_pin, LOW);
  digitalWrite(load_in_pin, HIGH);
  digitalWrite(data_out_pin, LOW);
  digitalWrite(load_out_pin, LOW);
  digitalWrite(bell_pin, LOW);

  Serial.begin(9600);
  matrix.begin(0x70);
  
  delay(5);
  //set_times();
}

void loop() {
  
  // current milliseconds since last reset
  current_timer = millis();
  
  if (relay_state == LOW) {  //if the relay state is low and waves are off
    if (current_timer - relay_timer > (time_off * minutes_to_millis)) {  //check to see if we've waited longer than the set time off
      turn_waves_on();
    }
  } else if (relay_state == HIGH) {  //if the relay state is high and waves are on
    if (current_timer - relay_timer > (time_on * minutes_to_millis)) {
      turn_waves_off();
    }
  }
  
  //write the relay state to the relay pin
  digitalWrite(relay_pin, relay_state);

  //write the bell flag state to the pin for its relay
  digitalWrite(bell_pin, ring_bell);

  //check if the bell has been on for long enough and switch the flag if so
  if ring_bell {
    if ((millis() - bell_timer) > bell_ring_length) {
      ring_bell = LOW;
    }
  }
  
  update_display();
  
  button_set_on_state = digitalRead(set_on_pin);
  if (button_set_on_state == HIGH) {
    if (relay_state == LOW) {
      turn_waves_on();
    }
  }
  
  button_set_off_state = digitalRead(set_off_pin);
  if (button_set_off_state == HIGH) {
    if (relay_state == HIGH) {
      turn_waves_off();
    }
  }
  
  if (debug_mode) {
    if (current_timer - debug_timer > debug_write_interval) {
      Serial.println(String("Relay: " + relay_state));
      Serial.println(String("Secs elpsd: " + (current_timer - relay_timer) / 1000));
      debug_timer = current_timer;
    }
  } 
}

void turn_waves_on() {
  if (debug_mode) {
    Serial.println("Switching to ON");
  }
      
  //set the relay timer to equal the current millis since reset
  relay_timer = current_timer;
      
  //write the relay state to on
  relay_state = HIGH;

  //set the ring_bell flag to high and mark the start time
  ring_bell = HIGH;
  bell_timer = millis();
}

void turn_waves_off() {
  if (debug_mode) {
    Serial.println("Switching to OFF");
  }
      
  //set the relay timer to equal the current millis since reset
  relay_timer = current_timer;
      
  //write the relay state to off
  relay_state = LOW;
}


void update_display() {
  unsigned long seconds_remaining = 0;
  
  //stores the number of seconds remaining in the current cycle as a plain number for display on 7seg
  if (relay_state) {
    seconds_remaining = (time_on * 60) - ((current_timer - relay_timer) / 1000);
  } else {
    seconds_remaining = (time_off * 60) - ((current_timer - relay_timer) / 1000);
  }
  
  //splits the number of remaining seconds into individual digits for display
  int mins_tens = round((seconds_remaining / 600) % 100);
  int mins_ones = round((seconds_remaining / 60) % 10);
  int secs_tens = ((seconds_remaining / 10) % 6);
  int secs_ones = (seconds_remaining % 10);
  matrix.writeDigitNum(0, mins_tens);
  matrix.writeDigitNum(1, mins_ones);
  matrix.writeDigitNum(3, secs_tens);
  matrix.writeDigitNum(4, secs_ones);
  matrix.drawColon((seconds_remaining - 1) % 2); //draws colon on even numbers
  matrix.writeDisplay();
}

void update_outdoor_display();
  unsigned long seconds_remaining = 0;
  
  //stores the number of seconds remaining in the current cycle as a plain number for display on 7seg
  if (relay_state) {
    seconds_remaining = (time_on  * 60) - ((current_timer - relay_timer) / 1000);
  } else {
    seconds_remaining = (time_off * 60) - ((current_timer - relay_timer) / 1000);
  }
  
  //splits the number of remaining seconds into individual digits for display
  int mins_tens = round((seconds_remaining / 600) % 100);
  int mins_ones = round((seconds_remaining /  60) %  10);
  int secs_tens =      ((seconds_remaining /  10) %   6);
  int secs_ones =       (seconds_remaining %  10);

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

  /*
     _a_
    f| |b
     _g_
    e| |c
     _d_
  */

  unsigned long output = {
    (segment_array[mins_tens] << 24 ) +
    (segment_array[mins_ones] << 16 ) +
    (segment_array[secs_tens] << 8  ) +
    (segment_array[secs_ones]       )
  };

  /*
  example:
  12:34

  mins_tens = 1
  mins_ones = 2
  secs_tens = 3
  secs_ones = 4

  so mins_tens = 01100000 << 24 = 01100000000000000000000000000000
     mins_ones = 11011010 << 16 = 00000000110110100000000000000000
     secs_tens = 11110010 <<  8 = 00000000000000001111001000000000
     secs_ones = 01100110 ==      00000000000000000000000001100110

  and all together:               01100000110110101111001001100110
  */

  shiftOut(data_out_pin, clock_pin, MSBFIRST, output);

}

void set_times() {
  byte input_byte = 0b00000000;
  
  digitalWrite(load_in_pin, LOW);
  delay(5);
  digitalWrite(load_in_pin, HIGH);
  delay(5);
  
  for(int i = 0; i < 8; i++) {
    bitWrite(input_byte, i, digitalRead(data_pin));
    digitalWrite(clock_pin, HIGH);
    delay(5);
    digitalWrite(clock_pin, LOW);
    delay(5);
  }

  //byte input_byte = shiftIn(data_pin, clock_pin, LSBFIRST);

  time_on = max(1,(input_byte >> 4));
  time_off = max(1,(input_byte &= 0b00001111));
  
  if (debug_mode) {
    Serial.println(String("Setting New Times"));
    Serial.println(String("ON: ") + time_on + String(" OFF: ") + time_off);
  }

}
