#define GBA_NONE 0x3FF

/*
Teensy Pin | GBA key | TP | Name
 - - GND
 0 - 29 Vsync
 1 9 40 L
 2 6 42 Up
 3 5 39 Left
 4 7 41 Down
 5 4 38 Right
 6 2 33 Select
 7 3 34 Start
 8 8 35 R
 9 1 36 B
10 0 37 A
14 - 25 Reset
*/

void setup() {
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(14, OUTPUT);
  digitalWriteFast(14, HIGH);
  Serial.begin(9600);
  setState(GBA_NONE);
}

void setState(short state) {
  digitalWriteFast(1, (state >> 9) & 1);
  digitalWriteFast(2, (state >> 6) & 1);
  digitalWriteFast(3, (state >> 5) & 1);
  digitalWriteFast(4, (state >> 7) & 1);
  digitalWriteFast(5, (state >> 4) & 1);
  digitalWriteFast(6, (state >> 2) & 1);
  digitalWriteFast(7, (state >> 3) & 1);
  digitalWriteFast(8, (state >> 8) & 1);
  digitalWriteFast(9, (state >> 1) & 1);
  digitalWriteFast(10, (state >> 0) & 1);
  // Perform reset
  if ((state >> 11) & 1) {
    digitalWriteFast(14, LOW);
    delayMicroseconds(10);
    digitalWriteFast(14, HIGH);
  }
}

void loop() {
  int controllerState = GBA_NONE;
  while (true) {
    noInterrupts();
    while (digitalReadFast(0) == HIGH);
    interrupts();
    unsigned short bytes = controllerState;
    // Uncomment the following line for live playback
    // while (Serial.available() >= 2)
    Serial.readBytes((char*) &bytes, 2);
    controllerState = bytes;
    setState(controllerState);
    Serial.write((const char*) &bytes, 2);
    Serial.flush();
    while (digitalReadFast(0) == LOW);
  }
}
