#define SWITCHPIN A2 // hardware pullup resistor
#define VOLTPIN A1 // MINUSRAIL Voltage Sensor Pin
#define VOLTCOEFF 13.179  // larger number interprets as lower voltage
#define VOLTKNOB A4
#define HIGHVOLT 24 // voltage with knob all the way up
#define LOWVOLT 10 // voltage with knob all the way down
#define WATTKNOB A5
#define OVERSAMPLES 25
#define AVG_CYCLES 10 // average measured values over this many samples
#define GREENLEDS 12 // pedalometer green
#define REDLEDS 11 // pedalometer red
#define NUM_PINS 6 // Number of outputs.

const int pins[NUM_PINS] = {
  3, 4, 5, 6, 7, 8};

int voltsAdc = 0;
float voltsAdcAvg = 0;
float volts = 0;
float voltTarget = 0;
byte harpBits = 0;

void setup() {
  for (int i=0; i < NUM_PINS; i++) {
    pinMode(pins[i],OUTPUT);
  }
  pinMode(REDLEDS,OUTPUT);
  pinMode(GREENLEDS,OUTPUT);
  Serial.begin(57600);
  Serial.println("power harp 1.0");
}

void loop() {
  getVolts();
  if (digitalRead(SWITCHPIN)) { // switch is open
    voltTarget = voltKnob();
    harpBits = wattKnob();
  } else { // switch is closed
    voltTarget = 24;
    harpBits = 1;
  }
  doPedalometer();
  for (int i=0; i < NUM_PINS; i++) {
    digitalWrite(pins[i],harpBits & (1 << i));
    if (harpBits & (1 << i)) {
      Serial.print(i);
    } else {
      Serial.print(' ');
    }
  }
  Serial.print("   ");
  Serial.print(volts);
  Serial.print(" volts      voltTarget = ");
  Serial.println(voltTarget);
  delay(100);
}

void getVolts(){
  voltsAdc = analogRead(VOLTPIN);
  voltsAdcAvg = average(voltsAdc, voltsAdcAvg);
  volts = adc2volts(voltsAdcAvg);
}

float average(float val, float avg){
  if(avg == 0)
    avg = val;
  return (val + (avg * (AVG_CYCLES - 1))) / AVG_CYCLES;
}

float adc2volts(float adc){
  return adc * (1 / VOLTCOEFF);
}

float voltKnob() {
  return (float)analogRead(VOLTKNOB) / 1023.0 * (float)(HIGHVOLT - LOWVOLT) + LOWVOLT;
}

byte wattKnob() {
  unsigned long analogAdder = 0;
  for (int j=0; j<OVERSAMPLES; j++) analogAdder += analogRead(WATTKNOB);
  return analogAdder / OVERSAMPLES / 16;
}

void doPedalometer() {
  if (volts > voltTarget) { // if volts is higher than target
    digitalWrite(REDLEDS,LOW);
    digitalWrite(GREENLEDS,HIGH); // green lights on
  } else {                 // if volts is higher than knob setting
    digitalWrite(REDLEDS,HIGH); // red lights on
    digitalWrite(GREENLEDS,LOW);
  }
}
