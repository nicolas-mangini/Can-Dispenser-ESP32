#include <Arduino.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <ESP32Servo.h>

#define PUBLISH_DISPENSE_TOPIC "dispenser/maquina1/dispense_event"
#define PUBLISH_DETECT_TOPIC "dispenser/maquina1/product_detected"
#define IOT_SUBSCRIBE_TOPIC "dispenser/maquina1/stock"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

const int BUZZER_PIN = 27; // Digital pin for the buzzer
const int LED_PIN = 26;    // Digital pin for the LED
const int LED_NO_STOCK = 32;

// button and debouncing
const int BUTTON_PIN = 14;    // Digital pin for the button
bool buttonState = false;     // Current button state
bool lastButtonState = false; // Previous button state
bool isButtonPressed = false; // Flag to indicate a button press

// HC-SR04
const int ECHO_PIN = 25;
const int TRIG_PIN = 33;
const double SOUND_SPEED = 0.0343; // in cm/microsecs
bool isObjectDetected = false;

// servo
const int SERVO_PIN = 12;
Servo myServo;

int stock = 20;

void message_handler(char *topic, byte *payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
  Serial.println(message);
}

void connect_IOT()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(CERT_CA);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(message_handler);

  Serial.println("\nConnecting to MQTT Server");

  while (!client.connect("Pepe"))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("Server Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(IOT_SUBSCRIBE_TOPIC);

  Serial.println("IoT Connected!");
}

void sound_buzzer(int delayTime)
{
  // Turn the buzzer on
  digitalWrite(BUZZER_PIN, HIGH);
  delay(delayTime); // Sound for 200 milliseconds
  // Turn the buzzer off
  digitalWrite(BUZZER_PIN, LOW);
}

void flash_led(int ledPin, int delayTime, int repeat, boolean withSound)
{
  for (int i = 0; i < repeat; i++)
  {
    // Turn the LED on
    digitalWrite(ledPin, HIGH);
    if (withSound)
    {
      sound_buzzer(delayTime);
    }
    delay(delayTime); // Wait for 200 milliseconds
    // Turn the LED off
    digitalWrite(ledPin, LOW);
    delay(delayTime); // Wait for 200 milliseconds
  }
}

void publish_message()
{
  if (isButtonPressed)
  {
    StaticJsonDocument<200> doc;
    doc["button-pressed"] = true;
    doc["machine_id"] = MACHINE_ID;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client

    client.publish(PUBLISH_DISPENSE_TOPIC, jsonBuffer);
    Serial.println("published-button-pressed");

    // Sound the buzzer and flash the LED
    flash_led(LED_PIN, 100, 1, true);

    myServo.attach(SERVO_PIN);
    myServo.write(90);
    delay(5000);
    myServo.write(0);
    myServo.detach();

    isButtonPressed = false; // Reset the isButtonPressed state after publishing
  }

  if (isObjectDetected)
  {
    StaticJsonDocument<200> doc;
    doc["object-detected"] = true;
    doc["machine_id"] = MACHINE_ID;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client

    client.publish(PUBLISH_DISPENSE_TOPIC, jsonBuffer);
    Serial.println("published-object-detected");

    sound_buzzer(100);

    isObjectDetected = false;
    delay(5000);
  }
}

void send_trigger(void)
{
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
}

long get_pulse(void)
{
  return pulseIn(ECHO_PIN, HIGH); // in microseconds
}

void setup()
{
  Serial.begin(115200);
  connect_IOT();
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set the button pin as input with a pull-up resistor
  pinMode(BUZZER_PIN, OUTPUT);       // Set the buzzer pin as an output
  pinMode(LED_PIN, OUTPUT);          // Set the LED pin as an output
  pinMode(LED_NO_STOCK, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);   // trigPin as output
  digitalWrite(TRIG_PIN, LOW); // trigPin to low
  pinMode(ECHO_PIN, INPUT);    // echoPin as input
  myServo.attach(SERVO_PIN);
}

void loop()
{
  publish_message();

  // BUTTON-DETECT
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState != lastButtonState)
  {
    if (buttonState == LOW)
    {
      // Button pressed
      if (stock > 0)
      {
        isButtonPressed = true;
      }
      else
      {
        // No stock left, beep twice
        flash_led(LED_NO_STOCK, 50, 3, true);
        Serial.println("No stock available");
      }
    }
    lastButtonState = buttonState;
  }

  // HC-SR04
  long duration;
  double distance;

  send_trigger();
  duration = get_pulse();

  distance = duration * SOUND_SPEED / 2;

  if (distance < 5)
  {
    if (stock > 0)
    {
      isObjectDetected = true;
      stock--;
      Serial.printf("distance = %f\n", distance);
    }
    else
    {
      // No stock left, beep twice
      flash_led(LED_NO_STOCK, 50, 3, true);
      Serial.println("No stock available");
      delay(5000);
    }
  }

  client.loop();
}
