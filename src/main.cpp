#include <Arduino.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define PUBLISH_DISPENSE_TOPIC "dispenser/maquina1/dispense_event"
#define PUBLISH_DETECT_TOPIC "dispenser/maquina1/product_detected"
#define IOT_SUBSCRIBE_TOPIC "dispenser/maquina1/stock"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

// button and debouncing
const int BUTTON_PIN = 14;    // Digital pin for the button
const int BUZZER_PIN = 27;    // Digital pin for the buzzer
const int LED_PIN = 26;       // Digital pin for the LED
const int LED_NO_STOCK = 32;
bool buttonState = false;     // Current button state
bool lastButtonState = false; // Previous button state
bool isButtonPressed = false; // Flag to indicate a button press

// HC-SR04
const int ECHO_PIN = 25;
const int TRIG_PIN = 33;
const double sound_speed = 0.0343; // in cm/microsecs
bool isObjectDetected = false;

int stock = 0;

void messageHandler(char *topic, byte *payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
  Serial.println(message);
}

void connectIOT()
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
  client.setCallback(messageHandler);

  Serial.println("Connecting to MQTT Server");

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

void flashLED()
{
  // Turn the LED on
  digitalWrite(LED_PIN, HIGH);
  delay(200); // Turn on for 200 milliseconds
  // Turn the LED off
  digitalWrite(LED_PIN, LOW);
}

void publishMessage()
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
    sound_buzzer(200);
    flashLED();

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
    sound_buzzer(50);

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
  connectIOT();
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set the button pin as input with a pull-up resistor
  pinMode(BUZZER_PIN, OUTPUT);       // Set the buzzer pin as an output
  pinMode(LED_PIN, OUTPUT);          // Set the LED pin as an output
  pinMode(LED_NO_STOCK, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);   // trigPin as output
  digitalWrite(TRIG_PIN, LOW); // trigPin to low
  pinMode(ECHO_PIN, INPUT);    // echoPin as input
}

void loop()
{
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
        digitalWrite(LED_NO_STOCK, HIGH);
        sound_buzzer(200);
        delay(100);
        sound_buzzer(200);
        delay(100);
        sound_buzzer(200);
        digitalWrite(LED_NO_STOCK, LOW);
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

  distance = duration * sound_speed / 2;

  if (distance < 5)
  {
    isObjectDetected = true;
    stock--;
    Serial.printf("distance = %f\n", distance);
  }

  publishMessage();

  client.loop();
}
