// wave machine controller
// 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();

// -----pin assignments-----
const int cycle_relay_pin           = 2;  // output; controls relay that turns on/off the cycle
const int diverter_relay_pin        = 3;  // output; controls relay that actuates the diverters
const int cycle_on_pin              = 4;  // input, digital; reads button that engages cycle
const int cycle_off_pin             = 5;  // input, digital; reads button that disengages cycle
const int diverter_off_pin          = A6; // input, analog; reads pot that determines diverter off time
const int diverter_on_pin           = A7; // input, analog; reads pot that determines diverter on time

// -----constant initialization-----
const unsigned long cycle_on_time   = 600000; // time wave cycle is on, in milliseconds
const unsigned long cycle_off_time  = 900000; // time wave cycle is off, in milliseconds



void setup()
{

  pinMode(cycle_relay_pin,    OUTPUT);
  pinMode(diverter_relay_pin, OUTPUT);
  pinMode(cycle_on_pin,       INPUT);
  pinMode(cycle_off_pin,      INPUT);
  pinMode(diverter_off_pin,   INPUT);
  pinMode(diverter_on_pin,    INPUT);

  digitalWrite(cycle_relay_pin,     LOW);
  digitalWrite(diverter_relay_pin,  LOW);

  matrix.begin(0x70);

}



bool timer(unsigned long current_time, unsigned long exit_time)
{
  
  if (current_time > exit_time) { return true; }
  else { return false; }

}



unsigned long exit_cycle_time = (millis() + cycle_off_time);

void set_wave_cycle();
{

  if (digitalRead(cycle_relay_pin))
  {
    if timer(millis(),exit_cycle_time)
    {
      digitalWrite(cycle_relay_pin, LOW);
      exit_cycle_time = (millis() + cycle_off_time);
    }
  }
  else
  {
    if timer(millis(),exit_cycle_time)
    {
      digitalWrite(cycle_relay_pin, HIGH);
      exit_cycle_time = (millis() + cycle_on_time);
    }

}



void update_display();
{
  
}



void loop()
{

  set_wave_cycle();
  update_display();

}
