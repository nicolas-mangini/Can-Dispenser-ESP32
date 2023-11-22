#include <Arduino.h>
#include <secrets.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <string>

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
Servo servo;
MD_Parola ledMatrix = MD_Parola(MATRIX_HARDWARE_TYPE, MATRIX_CS_PIN, MATRIX_MAX_DEVICES);

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

void display_matrix_stock()
{
  char text[100];
  sprintf(text, "%d", stock);
  ledMatrix.displayScroll(text, PA_CENTER, PA_SCROLL_LEFT, 200);
}

void update_stock(int newStock)
{
  stock = newStock;
  Serial.print("Updated stock: ");
  Serial.println(stock);
}

void dispense()
{
  //    Positioning table
  static const int grads[] = {90, 0, -1};

  int i, c, last, t;

  for (i = 0; (c = grads[i]) >= 0; ++i)
    go_servo(c);
  for (i -= 2; (c = grads[i]) > 0; --i)
    go_servo(c);
}

void suscribe_message_handler(char *topic, byte *payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);

  // Assuming the payload is a JSON string, create a buffer to hold it
  char payloadBuffer[length + 1];
  memcpy(payloadBuffer, payload, length);
  payloadBuffer[length] = '\0';

  // Parse the JSON object using a JSON parser library
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payloadBuffer);

  // Extract the stock value and machine ID from the JSON object
  int receivedStock = doc["stock-updated"];
  String machineId = doc["machine_id"];

  if (strcmp(topic, STOCK_TOPIC) == 0)
  {
    update_stock(receivedStock);
    display_matrix_stock(); // Display the updated stock on the LED matrix
  }

  if (strcmp(topic, DISPENSE_TOPIC_UI) == 0)
  {
    if (stock > 0)
    {
      Serial.println("Dispense from UI received");
      flash_led(LED_STOCK, 100, 1, true);
      dispense();
      stock--;
    }
    else
    {
      // No stock left, beep twice
      flash_led(LED_NO_STOCK, 50, 3, true);
      Serial.println("\nNo stock available");
    }
  }
}

void publish_message_handler()
{
  if (isButtonPressed)
  {
    StaticJsonDocument<200> doc;
    doc["button-pressed"] = true;
    doc["machine_id"] = MACHINE_ID;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client

    client.publish(DISPENSE_TOPIC, jsonBuffer);
    Serial.println("Button pressed");

    // Sound the buzzer and flash the LED
    flash_led(LED_STOCK, 100, 1, true);

    dispense();
    stock--;
    display_matrix_stock(); // Display the updated stock on the LED matrix
    Serial.println("Machine dispensed succesfully");

    isButtonPressed = false; // Reset the isButtonPressed state after publishing
  }
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
  client.setCallback(suscribe_message_handler);

  Serial.println("\nConnecting to MQTT Server");

  while (!client.connect(MACHINE_ID))
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
  client.subscribe(STOCK_TOPIC);
  client.subscribe(DISPENSE_TOPIC_UI);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("IoT Connected!");
}

void setup()
{
  Serial.begin(BAUD);
  connect_IOT();
  pinMode(LED_BUILTIN, OUTPUT);      // Set the onboard LED as output
  pinMode(LED_STOCK, OUTPUT);        // Set the stock LED as output
  pinMode(LED_NO_STOCK, OUTPUT);     // Set the no stock LED as output ()
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set the button pin as input with a pull-up resistor
  pinMode(BUZZER_PIN, OUTPUT);       // Set the buzzer pin as an output

  servo.attach(SERVO_PIN);
  servo.write(90);

  ledMatrix.begin();          // initialize the object
  ledMatrix.setIntensity(15); // set the brightness of the LED matrix display (from 0 to 15)
  ledMatrix.displayClear();   // clear led matrix display

  delay(2000);
}

void loop()
{
  ledMatrix.displayAnimate();

  publish_message_handler();

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
        Serial.println("\nNo stock available");
      }
    }
    lastButtonState = buttonState;
  }
  client.loop();
}
