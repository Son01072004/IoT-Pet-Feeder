#define BLYNK_TEMPLATE_ID "TMPL63FowuTGz"
#define BLYNK_TEMPLATE_NAME "SERVO MOTOR"
#define BLYNK_AUTH_TOKEN "ycDS4bcJ2PeoIg40Y7N1rQ6hDNO3g4km"

#include <BlynkSimpleEsp8266.h>
#include <NewPing.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

Servo servo;
char auth[] = "ycDS4bcJ2PeoIg40Y7N1rQ6hDNO3g4km";
char ssid[] = "IP";
char pass[] = "11111111";

const int trigPin = D6;          // Trigger pin for the ultrasonic sensor
const int echoPin = D5;          // Echo pin for the ultrasonic sensor
const int containerHeight = 18;  // Height of the pet food container in centimeters
int angleOpen = 0;               // Angle for opening the trap door
int angleClosed = 180;           // Angle for closing the trap door
int trapDoorState = 0;           // 0: Closed, 1: Open
NewPing sonar(trigPin, echoPin);

BLYNK_WRITE(V1) {
  if (param.asInt() == 1 && trapDoorState == 0) {
    servo.write(angleOpen);
    trapDoorState = 1;
    delay(2000);  // Delay for 2 seconds
    servo.write(angleClosed);
    trapDoorState = 0;
  }
}
void setup() {
  Serial.begin(115200);
  servo.attach(D3);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  int foodLevel = measureFoodLevel();
  updateBlynkGauge(foodLevel);

  // Check for food level and send events
  if (foodLevel != -1) {
    if (foodLevel < 20) {
      Blynk.logEvent("pet_feeder_low", "Food level is below 20. Refill the container!");
    }
    if (foodLevel < 10) {
      Blynk.logEvent("pet_feeder_urgent", "Food level is below 10. Urgent: Refill the container!");
    }
  }

  delay(10);  // Adjust delay as needed
}

int measureFoodLevel() {
  delay(50);
  int distance = sonar.ping_cm();
  if (distance == 0) {
    Serial.println("Error: Unable to determine distance. Please check the sensor.");
    return -1;
  }
  int foodLevel = containerHeight - distance;
  return max(0, foodLevel);  // Ensure the food level is non-negative
}

void updateBlynkGauge(int foodLevel) {
  if (foodLevel != -1) {
    int mappedValue = map(foodLevel, 0, containerHeight, 1, 120);
    Blynk.virtualWrite(V3, mappedValue);  // Assuming V3 is the virtual pin for the gauge in the Blynk app
  }
}
