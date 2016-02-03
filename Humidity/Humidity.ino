// TM1638 code taken from https://github.com/moozzyk/TM1638
const int tm_strobe = 5;
const int tm_clock = 4;
const int tm_data = 3;

int d[8];
// Taken from https://github.com/adafruit/DHT-sensor-library
// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to

#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  dht.begin();
  pinMode(tm_strobe, OUTPUT);
  pinMode(tm_clock, OUTPUT);
  pinMode(tm_data, OUTPUT);
  sendCommand(0x8f);  // activate and set brightness to max
  tm_reset();
 
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
   
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

 int ti = (int) (t*10);
 int hi = (int) (h*10);
 
 d_num1000(ti, 0);
d_num1000(hi, 4);
d[2] |= 0x80;
d[6] |= 0x80;

update_display();
}

void sendCommand(uint8_t value)
{
  digitalWrite(tm_strobe, LOW);
  shiftOut(tm_data, tm_clock, LSBFIRST, value);
  digitalWrite(tm_strobe, HIGH);
}
 
void tm_reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(tm_strobe, LOW);
  shiftOut(tm_data, tm_clock, LSBFIRST, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOut(tm_data, tm_clock, LSBFIRST, 0x00);
  }
  digitalWrite(tm_strobe, HIGH);
}

 void d_num1000(int num, int start_index) {
                       /*0*/ /*1*/ /*2*/ /*3*/ /*4*/ /*5*/ /*6*/ /*7*/ /*8*/ /*9*/
  uint8_t digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };

 d[start_index] = digits[num/1000%10]; 
 d[start_index+1] = digits[num/100%10]; 
 d[start_index+2] = digits[(num/10)%10]; 
 d[start_index+3] = digits[num%10]; 
}

void d_time(int hour, int minute, int second, int offset, int start_index) {
                         /*0*/ /*1*/ /*2*/ /*3*/ /*4*/ /*5*/ /*6*/ /*7*/ /*8*/ /*9*/
  uint8_t digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };
  int offset_h = hour;
  int offset_m = minute + offset;
  offset_h += offset_m / 60;
  offset_m = offset_m % 60;
  d[start_index] = digits[offset_h/10];
  d[start_index+1] = digits[offset_h%10] + ((second%2 == 1) ? 0x80 : 0x00);
  d[start_index+2] = digits[offset_m/10];
  d[start_index+3] = digits[offset_m%10];
}

void update_display(void) {
  int i=0;    
  sendCommand(0x40);  // set auto increment address
  digitalWrite(tm_strobe, LOW);
  shiftOut(tm_data, tm_clock, LSBFIRST, 0xc0); 

  for (i=0; i<8; i++) {
    shiftOut(tm_data, tm_clock, LSBFIRST, d[i]&0xff);
    shiftOut(tm_data, tm_clock, LSBFIRST, (d[i] & 0x0100) >> 8);
  }
 
 digitalWrite(tm_strobe, HIGH);
}

uint8_t readButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(tm_strobe, LOW);
  shiftOut(tm_data, tm_clock, LSBFIRST, 0x42);
 
  pinMode(tm_data, INPUT);
 
  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftIn(tm_data, tm_clock, LSBFIRST) << i;
    buttons |= v;
  }
 
  pinMode(tm_data, OUTPUT);
  digitalWrite(tm_strobe, HIGH);
  return buttons;
}
  

