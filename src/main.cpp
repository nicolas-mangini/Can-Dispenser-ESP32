#include <Arduino.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <ESP32Servo.h>

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
Servo servo;

bool buttonState = false;     // Current button state
bool lastButtonState = false; // Previous button state
bool isButtonPressed = false; // Flag to indicate a button press
int stock = 20;

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

void go_servo(int pos)
{
  servo.write(pos);
  delay(3000);
}

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

void dispense()
{
  //    Positioning table
  static const int grads[] = {0, 120, -1};

  int i, c, last, t;

  for (i = 0; (c = grads[i]) >= 0; ++i)
    go_servo(c);
  for (i -= 2; (c = grads[i]) > 0; --i)
    go_servo(c);
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

    dispense();
    Serial.println("dispensed");

    isButtonPressed = false; // Reset the isButtonPressed state after publishing
  }
}

void setup()
{
  Serial.begin(BAUD);
  connect_IOT();
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set the button pin as input with a pull-up resistor
  pinMode(BUZZER_PIN, OUTPUT);       // Set the buzzer pin as an output
  pinMode(BUZZER_PIN, OUTPUT);       // Set the buzzer pin as an output
  pinMode(LED_PIN, OUTPUT);          // Set the LED pin as an output
  pinMode(LED_NO_STOCK, OUTPUT);

  servo.attach(SERVO_PIN);
  servo.write(0);
  delay(2000);
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
  client.loop();
}
