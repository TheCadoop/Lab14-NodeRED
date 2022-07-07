SYSTEM_THREAD(ENABLED);

#include "MQTT.h"
#include "oled-wing-adafruit.h";

void callback(char* topic, byte* payload, unsigned int length);
MQTT client("lab.thewcl.com", 1883, callback);

OledWingAdafruit display;

float lat;
float lon;
unsigned long startTime = millis();
const unsigned long period = 10000;
bool gotLat = false;
bool gotLong = false;

void callback(char *topic, byte *payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String s = p;
  double value = s.toFloat();

  if (String(topic).equals("cade/isslat")) {
    gotLat = true;
    lat = value;
  } else if (String(topic).equals("cade/isslong")) {
    gotLong = true;
    lon = value;
  }

  if (gotLat && gotLong) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Lat: ");
    display.println(lat);
    display.print("Long: ");
    display.print(lon);
    display.display();

    gotLat = false;
    gotLong = false;
  }
}
void setup() {
  Serial.begin(9600);
  display.setup();

  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.setTextSize(1);
  display.display();
}

void loop() {
  if (client.isConnected()) {
    client.loop();
  } else {
    client.connect(System.deviceID());
    client.subscribe("cade/isslat");
    client.subscribe("cade/isslong");
  }
  
  display.loop();

  unsigned long currentTime = millis();
  if (currentTime - startTime >= 10000) {
    client.publish("cade/issin", "go");
    startTime = currentTime;
  }
}