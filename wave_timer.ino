// Prototype Wave Timer code
// Created by Tyler Masters

const int relay_pin = 13;
const int set_on_pin = 2;
const int set_off_pin = 3;

const unsigned long time_on = 4;  // time relay closed and waves on (in minutes)
const unsigned long time_off = 6;  // time relay open and waves off (in minutes)
const unsigned long minutes_to_millis = 1000;

int button_set_on_state = 0;

const boolean debug_mode = true;
unsigned long debug_timer = 0;
const unsigned long debug_write_interval = 1000;
unsigned long current_timer = 0;

int relay_state = LOW;
unsigned long relay_timer = 0;

void setup() {
  pinMode(relay_pin, OUTPUT);
  pinMode(set_on_pin, INPUT);
	pinMode(set_off_pin, INPUT);
  Serial.begin(9600);
}

void loop() {
  
  // current milliseconds since last reset
  current_timer = millis();
  
  if (relay_state == LOW) {  //if the relay state is low and waves are off
    if (current_timer - relay_timer > (time_off * minutes_to_millis)) {  //check to see if we've waited longer than the set time off
      
      turn_waves_on();
      
    }
  
  }
  else if (relay_state == HIGH) {  //if the relay state is high and waves are on
    if (current_timer - relay_timer > (time_on * minutes_to_millis)) {
  
      if (debug_mode) {
        turn_waves_off();
      }
      
    }
    
  }
  //write the relay state to the relay pin
  digitalWrite(relay_pin, relay_state);
  
  button_set_on_state = digitalRead(set_on_pin);
  if (button_set_on_state == HIGH) {
    if (relay_state == LOW) {
      turn_waves_on();
    }
  }
  
  if (debug_mode) {
    if (current_timer - debug_timer > debug_write_interval) {
      Serial.println(relay_state + String(" ") + (current_timer / 1000));
      debug_timer = current_timer;
      Serial.println((current_timer - relay_timer) / 1000);
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

