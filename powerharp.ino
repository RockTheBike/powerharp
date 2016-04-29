#define VOLTPIN A1 // MINUSRAIL Voltage Sensor Pin
#define VOLTCOEFF 13.179  // larger number interprets as lower voltage
#define VOLTKNOB A4
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
  if (volts > 10) {
    digitalWrite(REDLEDS,LOW);
    digitalWrite(GREENLEDS,HIGH);
  } else {
    digitalWrite(REDLEDS,HIGH);
    digitalWrite(GREENLEDS,LOW);
  }
  unsigned long analogAdder = 0;
  for (int j=0; j<OVERSAMPLES; j++) analogAdder += analogRead(WATTKNOB);
  byte harpBits = analogAdder / OVERSAMPLES / 16;
  for (int i=0; i < NUM_PINS; i++) {
    digitalWrite(pins[i],harpBits & (1 << i));
    if (harpBits & (1 << i)) {
      Serial.print(i);
    } else {
      Serial.print(' ');
    }
  }
  Serial.print('   ');
  Serial.println(volts);
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
