#define BLYNK_TEMPLATE_ID "TMPL6l0Vn23Eq"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_AUTH_TOKEN "ioYus1M2fxlx5pyu4an8v7DLfq416-0a"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Wi-Fi credentials
#define WIFI_SSID "FORD_2.4G"   // Replace with your Wi-Fi SSID
#define WIFI_PASSWORD "0832464086" // Replace with your Wi-Fi Password

// Pin configuration for HC-SR04 sensors
#define TRIG1 D1  // GPIO5
#define ECHO1 D2  // GPIO4
#define TRIG2 D3  // GPIO0
#define ECHO2 D4  // GPIO2

// Pin configuration for IR Sensor
#define IR_SENSOR D5  // GPIO14

// Pin configuration for LEDs
#define LED1 D6  // GPIO12
#define LED2 D7  // GPIO13

// Blynk virtual pins
const int VIRTUAL_PIN_LED1 = V0;  // Virtual pin for LED1 status and blink
const int VIRTUAL_PIN_LED2 = V1;  // Virtual pin for LED2 status and blink

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi and Blynk
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

  // Set pin modes
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(IR_SENSOR, INPUT);  // Set IR Sensor as input
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Turn off LEDs initially
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = (duration / 2) * 0.0343;
  return (duration == 0) ? -1 : distance;
}

void blinkLEDs(int ledPin1, int ledPin2, int virtualPin1, int virtualPin2, int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    Blynk.virtualWrite(virtualPin1, 1);  // Send ON status for LED1
    Blynk.virtualWrite(virtualPin2, 1);  // Send ON status for LED2
    delay(delayMs);
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    Blynk.virtualWrite(virtualPin1, 0);  // Send OFF status for LED1
    Blynk.virtualWrite(virtualPin2, 0);  // Send OFF status for LED2
    delay(delayMs);
  }
}

void loop() {
  Blynk.run();

  // Check Slot 1
  long distance1 = measureDistance(TRIG1, ECHO1);
  if (distance1 != -1 && distance1 < 15) {  // Replace 15 with your desired threshold
    digitalWrite(LED1, HIGH);  // Turn on LED1
    Blynk.virtualWrite(VIRTUAL_PIN_LED1, 1);  // Update Blynk with ON status
  } else {
    digitalWrite(LED1, LOW);   // Turn off LED1
    Blynk.virtualWrite(VIRTUAL_PIN_LED1, 0);  // Update Blynk with OFF status
  }

  // Check Slot 2
  long distance2 = measureDistance(TRIG2, ECHO2);
  if (distance2 != -1 && distance2 < 15) {
    digitalWrite(LED2, HIGH);  // Turn on LED2
    Blynk.virtualWrite(VIRTUAL_PIN_LED2, 1);  // Update Blynk with ON status
  } else {
    digitalWrite(LED2, LOW);   // Turn off LED2
    Blynk.virtualWrite(VIRTUAL_PIN_LED2, 0);  // Update Blynk with OFF status
  }

  // Check IR Sensor for Slot 3
  if (digitalRead(IR_SENSOR) == LOW) {  // LOW indicates object detected
    int objectDistance = 5; // สมมติว่า IR Sensor ระยะตรวจจับเป็น 5 ซม.
    if (objectDistance <= 5) {  // เงื่อนไขตรวจจับในระยะ 5 ซม.
      Serial.println("Slot 3: Occupied - Blinking LEDs");
      blinkLEDs(LED1, LED2, VIRTUAL_PIN_LED1, VIRTUAL_PIN_LED2, 3, 500);  // Blink LEDs and update Blynk
    } else {
      Serial.println("Slot 3: Object out of range.");
    }
  } else {
    Serial.println("Slot 3: Available");
  }

  // Print statuses to Serial Monitor
  Serial.print("LED1: ");
  Serial.println(digitalRead(LED1) ? "ON" : "OFF");
  Serial.print("LED2: ");
  Serial.println(digitalRead(LED2) ? "ON" : "OFF");
  Serial.println("=====================");

  delay(1000);
}
