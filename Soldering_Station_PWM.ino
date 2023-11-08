#include <Arduino.h>
#include <TM1637Display.h>
#include <PID_v1.h>


uint8_t CLK = 4;
uint8_t DIO = 5;
uint8_t TempSen = A1; //soldering gun thermocouple signal
uint8_t SetPnt = A2; //soldering set point
double tempsensor = 0;
double Setpoint;
//int err, integ, derr, lasterr;
int sptold = 0;
double Output;
uint8_t Signal_Pin = A0;
double Temp_Avr=0;
int index=0;

TM1637Display display(CLK, DIO);
unsigned long StartTime;

PID myPID(&tempsensor, &Output, &Setpoint, 10, 1, 2, DIRECT);


void setup() {
  Setpoint = 100;

  StartTime = millis();

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  //Serial.begin(9600);

}

void loop() {

  const uint8_t digitToSegment[] = {
    // XGFEDCBA
    0b00111111,    // 0
    0b00000110,    // 1
    0b01011011,    // 2
    0b01001111,    // 3
    0b01100110,    // 4
    0b01101101,    // 5
    0b01111101,    // 6
    0b00000111,    // 7
    0b01111111,    // 8
    0b01101111,    // 9
    0b00111001    // C

  };
//  n++;
  index++;

  //uint8_t k;
  display.setBrightness(0x0f);
  tempsensor = analogRead(TempSen);
  Setpoint = analogRead(SetPnt);
  Temp_Avr=tempsensor+Temp_Avr;
  int spt = Setpoint;
  // Serial.println(spt);
  

  myPID.Compute();
  double OutTrans = map(Output, 0, 255, 255, 0);
  analogWrite(Signal_Pin, OutTrans);

  //

  if (abs(spt - sptold) < 5 && (millis() - StartTime ) > 200)
  {
    StartTime = millis();
    Temp_Avr=Temp_Avr/index;

    uint16_t a = Temp_Avr * 0.704 + 23;
    
    uint8_t data[] = {0, 0, 0, 0}; //{ 0xff, 0xff, 0xff, 0xff };
    uint8_t b1 = a % 10;
    a /= 10;
    uint8_t b2 = a % 10;
    a /= 10;
    uint8_t b3 = a % 10;
    a /= 10;

    data[3] = digitToSegment[10];
    data[2] = digitToSegment[b1];//& 0x0f];
    data[1] = digitToSegment[b2];//& 0x0f];
    data[0] = digitToSegment[b3];// & 0x0f];

    display.setSegments(data);
    index=0;
    Temp_Avr=tempsensor;

  }

  if (abs(spt - sptold) > 5)
  {
    uint16_t a = Setpoint * 0.704 + 23;
    sptold = Setpoint;
    uint8_t data[] = {0, 0, 0, 0}; //{ 0xff, 0xff, 0xff, 0xff };
    uint8_t b1 = a % 10;
    a /= 10;
    uint8_t b2 = a % 10;
    a /= 10;
    uint8_t b3 = a % 10;
    a /= 10;

    data[3] = digitToSegment[10];
    data[2] = digitToSegment[b1];//& 0x0f];
    data[1] = digitToSegment[b2];//& 0x0f];
    data[0] = digitToSegment[b3];// & 0x0f];

    display.setSegments(data);
    delay(500);

  }

}


