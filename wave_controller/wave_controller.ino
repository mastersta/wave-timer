// wave machine controller
// created by Tyler Masters (mastersta on github)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();

// -----pin assignments-----
const int cycle_relay_pin           = 2;  // output; controls relay that turns on/off the cycle
const int diverter_relay_pin        = 3;  // output; controls relay that actuates the diverters
const int cycle_on_pin              = 4;  // input, digital; reads button that engages cycle
const int cycle_off_pin             = 5;  // input, digital; reads button that disengages cycle
const int bell_pin                  = 6;  // output, controls relay that turns on/off the bell
const int diverter_off_pin          = A6; // input, analog; reads pot that determines diverter off time
const int diverter_on_pin           = A7; // input, analog; reads pot that determines diverter on time

// -----constant initialization-----
const unsigned long cycle_on_time   = 600000; // time wave cycle is on, in milliseconds
const unsigned long cycle_off_time  = 900000; // time wave cycle is off, in milliseconds
const unsigned long bell_ring_time  = 3000  ; // time bell should ring at start of cycle
const float analog_read_multiplier  = 3     ;



void setup()
{

  pinMode(cycle_relay_pin,    OUTPUT);
  pinMode(diverter_relay_pin, OUTPUT);
  pinMode(cycle_on_pin,       INPUT);
  pinMode(cycle_off_pin,      INPUT);
  pinMode(bell_pin,           OUTPUT);
  pinMode(diverter_off_pin,   INPUT);
  pinMode(diverter_on_pin,    INPUT);
  
  pinMode(13, OUTPUT);

  digitalWrite(cycle_relay_pin,     LOW);
  digitalWrite(diverter_relay_pin,  LOW);
  digitalWrite(bell_pin,            LOW);

  matrix.begin(0x70);
  Serial.begin(9600);
  
  delay(1000);

}



bool timer(unsigned long current_time, unsigned long exit_time)
{
  
  if (current_time > exit_time) { return true; }
  else { return false; }

}



unsigned long set_wave_cycle(unsigned long exit_cycle_time)
{
  
  static bool i = digitalRead(cycle_relay_pin);
  static unsigned long bell_exit_time = 0;
  
  if(timer(millis(),exit_cycle_time))
  {
    i = !i;
    digitalWrite(cycle_relay_pin, i);
    if (i) 
    {
      exit_cycle_time = (millis() + cycle_off_time);
      bell_exit_time = (millis() + bell_ring_time);
    }
    else 
    {
      exit_cycle_time = (millis() + cycle_on_time);
    }
    Serial.println("auto");
    digitalWrite(13, i);
  }
  
  else if (digitalRead(cycle_on_pin) && !i)
  {
    i = true;
    digitalWrite(cycle_relay_pin, i);
    exit_cycle_time = (millis() + cycle_on_time);
    bell_exit_time = (millis() + bell_ring_time);
    Serial.println("manual on");
    digitalWrite(13, i);
  }
  
  else if (digitalRead(cycle_off_pin) && i)
  {
    i = false;
    digitalWrite(cycle_relay_pin, i);
    exit_cycle_time = (millis() + cycle_off_time);
    Serial.println("manual off");
    digitalWrite(13, i);
  }

  if(i) { set_diverters(); }
  else { digitalWrite(diverter_relay_pin, LOW); }

  digitalWrite(bell_pin,!timer(millis(),bell_exit_time));
  
  return exit_cycle_time;

}



unsigned long get_diverter_time(int state)
{

  int analog_pin = diverter_off_pin;
  if (state == 1) { analog_pin = diverter_on_pin; }
  
  return (analogRead(analog_pin) * analog_read_multiplier);
  
}



void set_diverters()
{
  
  static unsigned long exit_diverter_time = (millis() + get_diverter_time(1));
  static int i = digitalRead(diverter_relay_pin);
  
  if(timer(millis(),exit_diverter_time))
  {
    i = !i;
    digitalWrite(diverter_relay_pin, i);
    exit_diverter_time = (millis() + get_diverter_time(i));
    Serial.println("div");
  }

}



void update_display(unsigned long exit_cycle_time)
{

  unsigned long cycle_seconds_remaining = ((exit_cycle_time - millis())/1000);

  unsigned long mins_tens = round((cycle_seconds_remaining / 600) % 100);
  unsigned long mins_ones = round((cycle_seconds_remaining /  60) %  10);
  unsigned long secs_tens =      ((cycle_seconds_remaining /  10) %   6);
  unsigned long secs_ones =       (cycle_seconds_remaining        %  10);
  
  matrix.writeDigitNum(0, mins_tens);
  matrix.writeDigitNum(1, mins_ones);
  matrix.writeDigitNum(3, secs_tens);
  matrix.writeDigitNum(4, secs_ones);

  matrix.drawColon((cycle_seconds_remaining - 1) % 2);
  
  matrix.writeDisplay();

}



void loop()
{

  static unsigned long exit_cycle_time = (millis() + cycle_off_time);
  exit_cycle_time = set_wave_cycle(exit_cycle_time);
  update_display(exit_cycle_time);

}
