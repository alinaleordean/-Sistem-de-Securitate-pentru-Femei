#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const int touch = 7;
const int led = 9;

SoftwareSerial sim(4, 5);
SoftwareSerial ss(2, 3);

int _timeout;
String _buffer;
String number = "0040746807566"; 

TinyGPSPlus gps;
static const uint32_t GPSBaud = 9600;

void setup() {
  pinMode(touch, INPUT);
  pinMode(led, OUTPUT);

  Serial.begin(9600);
  sim.begin(9600);
  ss.begin(GPSBaud);

  Serial.println("Sistemul a pornit...");
  delay(1000);

  sim.println("AT");
  delay(1000);
  if (sim.available()) {
    String response = sim.readString();
    Serial.println("Modul SIM800L conectat: " + response);
  } else {
    Serial.println("Eroare: Modul SIM800L nu raspunde.");
  }

  Serial.println("Asteapta atingerea pentru a trimite un SMS si a initia un apel");
}

void loop() {
  int touchState = digitalRead(touch);
 
  if (touchState == HIGH) {
    digitalWrite(led, HIGH);
    displayGPS();

    delay(2000); 

    SendMessage();
    delay(5000);
    CallNumber();
    delay(5000);

    digitalWrite(led, LOW);
  }

  if (sim.available() > 0)
    Serial.write(sim.read());

  if (Serial.available() > 0) {
    switch (Serial.read()) {
      case 'r':
        ReceiveMessage();
        break;
    }
  }

  delay(200);
}

void SendMessage() {
  sim.println("AT+CMGF=1");
  delay(200);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(200);
  String SMS = "PERICOL";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(200);
  _buffer = _readSerial();
}

void ReceiveMessage() {
  Serial.println("SIM800L Citire SMS");
  sim.println("AT+CMGF=1");
  delay(200);
  sim.println("AT+CNMI=1,2,0,0,0");
  delay(200);
  Serial.write("Mesajele necitite au fost preluate");
}

String _readSerial() {
  _timeout = 0;
  while (!sim.available() && _timeout < 12000) {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  } else {
    return "";
  }
}

void CallNumber() {
  sim.print(F("ATD"));
  sim.print(number);
  sim.print(F(";\r\n"));
  delay(1000);
  _buffer = _readSerial();
  Serial.println(_buffer);
}

void displayGPS() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
    }
  }
}
