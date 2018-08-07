// wave machine controller
// created by Tyler Masters (mastersta on github)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <avr/wdt.h>

Adafruit_7segment matrix = Adafruit_7segment();

// -----pin assignments-----
const int cycle_relay_pin           = 2;  // output; controls relay that turns on/off the cycle
const int diverter_relay_pin        = 3;  // output; controls relay that actuates the diverters
const int cycle_on_pin              = 4;  // input, digital; reads button that engages cycle
const int cycle_off_pin             = 5;  // input, digital; reads button that disengages cycle
const int bell_pin                  = 6;  // output, controls relay that turns on/off the bell
const int power_led_pin             = 7;  // output, powers LED that displays that the controller is on
const int diverter_off_pin          = A6; // input, analog; reads pot that determines diverter off time
const int diverter_on_pin           = A7; // input, analog; reads pot that determines diverter on time

// -----constant initialization-----
const unsigned long cycle_on_time              = 600000; // time wave cycle is on, in milliseconds
const unsigned long cycle_off_time             = 900000; // time wave cycle is off, in milliseconds
const unsigned long bell_ring_time             = 5000  ; // time bell should ring at start of cycle
const float analog_read_multiplier             = 3     ;



void setup()
{

  pinMode(cycle_relay_pin,    OUTPUT);
  pinMode(diverter_relay_pin, OUTPUT);
  pinMode(cycle_on_pin,       INPUT);
  pinMode(cycle_off_pin,      INPUT);
  pinMode(bell_pin,           OUTPUT);
  pinMode(power_led_pin,      OUTPUT);
  pinMode(diverter_off_pin,   INPUT);
  pinMode(diverter_on_pin,    INPUT);
  
  pinMode(13, OUTPUT);  // debug led

  digitalWrite(cycle_relay_pin,     LOW);
  digitalWrite(diverter_relay_pin,  LOW);
  digitalWrite(bell_pin,            LOW);
  digitalWrite(power_led_pin,      HIGH); // remains high to show that power is on

  matrix.begin(0x70);
  Serial.begin(9600);
  
  watchdog_setup();

  delay(1000);

}



void watchdog_setup()
{

  cli();
  wdt_reset();

  WDTCSR |= B00011000;
  WDTCSR  = B01000110;

  sei();

}


// checks to see if an event has passed a time limit
// pass in millis() and a variable initialized at the event start + time event should last
bool timer(unsigned long current_time, unsigned long exit_time)
{
  
  if (current_time > exit_time) { return true; }
  else { return false; }

}



// determines if we're on the cycle or off, then swaps the cycle if enough time has passed, based on the
// constant times defined up top
// also handles the button presses to manually turn on or off the wave cycle, plus calls the function to
// actuate the diverters
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
      exit_cycle_time = (millis() + cycle_on_time);
      bell_exit_time = (millis() + bell_ring_time);
    }
    else 
    {
      exit_cycle_time = (millis() + cycle_off_time);
    }
    Serial.println("auto");
  }
  
  else if (digitalRead(cycle_on_pin) && !i)
  {
    i = true;
    digitalWrite(cycle_relay_pin, i);
    exit_cycle_time = (millis() + cycle_on_time);
    bell_exit_time = (millis() + bell_ring_time);
    Serial.println("manual on");
  }
  
  else if (digitalRead(cycle_off_pin) && i)
  {
    i = false;
    digitalWrite(cycle_relay_pin, i);
    exit_cycle_time = (millis() + cycle_off_time);
    Serial.println("manual off");
  }

  if(i) { set_diverters(); }
  else { digitalWrite(diverter_relay_pin, LOW); }
  
  digitalWrite(bell_pin,!timer(millis(),bell_exit_time));

  return exit_cycle_time;

}


// pulls the analog values from the potentiometers to determine how long to spend on each side of the diverter
// cycle
unsigned long get_diverter_time(int state)
{

  int analog_pin = diverter_off_pin;
  if (state == 1) { analog_pin = diverter_on_pin; }
  
  return (analogRead(analog_pin) * analog_read_multiplier);
  
}



// sets the state of the diverter relay, switching it if enough time has elapsed, based on what the analog pots
// are feeding the get_diverter_time() function
void set_diverters()
{
  
  static unsigned long exit_diverter_time = (millis() + get_diverter_time(1));
  static int i = digitalRead(diverter_relay_pin);
  
  if(timer(millis(),exit_diverter_time))
  {
    i = !i;
    digitalWrite(diverter_relay_pin, i);
    exit_diverter_time = (millis() + get_diverter_time(i));
  }

}


// updates the seven segment display on the front of the case with how much time is left in the wave cycle
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



// flashes the power led pin off periodically to indicate that the loop is still running
const unsigned long flash_duty_cycle = 0.1; // percent of one second that power led flashes off

void flash_power_led()
{
  
  static unsigned long exit_flash_time = 0;
  static bool i = false;

  if(timer(millis(),exit_flash_time) && i)
  {
    i = !i;
    exit_flash_time = (millis() + (1000 * flash_duty_cycle));
  }
  else if(timer(millis(),exit_flash_time) && !i)
  {
    i = !i;
    exit_flash_time = (millis() + (1 - (1000 * flash_duty_cycle)));
  }

  digitalWrite(power_led_pin, i);

}



ISR(WDT_vect)
{

  digitalWrite(cycle_relay_pin,    LOW);
  digitalWrite(diverter_relay_pin, LOW);
  digitalWrite(bell_pin,           LOW);
  digitalWrite(power_led_pin,      LOW);

}



void loop()
{

  static unsigned long exit_cycle_time = (millis() + cycle_off_time);
  exit_cycle_time = set_wave_cycle(exit_cycle_time);
  update_display(exit_cycle_time);
  flash_power_led();
  wdt_reset();

}
