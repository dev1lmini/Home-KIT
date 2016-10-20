#include <Thread.h>
#include <ThreadController.h>
#include <Keypad.h>
#include "dht.h"
#include "UIPEthernet.h"
#include "PubSubClient.h"

EthernetClient ethClient;
PubSubClient client;

byte mac[]    = {  0x00, 0x01, 0x02, 0x03, 0x04, 0x05D };
byte server[] = { 192, 168, 0, 33 };


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("callback");
}
void startEthernet() {
  Serial.println("start Ethernet");
  Ethernet.begin(mac); //start ethernet
}
void startMqtt() {
  client.setClient(ethClient); //set client for mqqt (ethernet)
  client.setServer(server, 1883); // set server for mqqt(raspberry)
  client.setCallback(callback); // set callback for subscripting

  client.connect("arduino"); //connect as arduino
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.print(".");
      client.connect("arduino");
    }
  }
  Serial.println("Connected to MQTT server");
}
void subscripting() {
  client.subscribe("status");
}

// ThreadController that will controll all threads
ThreadController controll = ThreadController();
Thread gasSensorThread = Thread();
Thread photocellThread = Thread();
Thread keypadThread = Thread();
Thread meteoThread = Thread();

const int gasPin = A0;
const int photocellPin = A1;
const byte numRows = 4; //number of rows on the keypad
const byte numCols = 4; //number of columns on the keypad
dht DHT;
#define DHT11_PIN 30

char keymap[numRows][numCols] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {9, 8, 7, 6}; //Rows 0 to 3
byte colPins[numCols] = {5, 4, 3, 2}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

int photocellReading;

void gasSensorCallback() {
  Serial.println("Gas sensor says:");
  Serial.println(analogRead(gasPin));
}

void photocellCallback() {
  photocellReading = analogRead(photocellPin);
  Serial.println("Photocell data");
  Serial.println(photocellReading);
}

void keypadCallback() {
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY)
  {
    Serial.print(keypressed);
  }
}

void meteoCallback() {
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
}
void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC

  //  startEthernet();
  // startMqtt();
  //  subscripting();

  gasSensorThread.onRun(gasSensorCallback);
  gasSensorThread.setInterval(1000);

  photocellThread.onRun(photocellCallback);
  photocellThread.setInterval(1000);

  keypadThread.onRun(keypadCallback);
  keypadThread.setInterval(0);

  meteoThread.onRun(meteoCallback);
  meteoThread.setInterval(1000);

  controll.add(&gasSensorThread);
  controll.add(&photocellThread);
  controll.add(&keypadThread);
  controll.add(&meteoThread);
}

void loop() {
  controll.run();
  //    if (!client.connected())
  //  {
  //    // clientID, username, MD5 encoded password
  //    client.connect("arduino");
  //    subscripting();
  //    client.loop();
  //  }
  //
  //  // MQTT client loop processing
  //  client.loop();
}
