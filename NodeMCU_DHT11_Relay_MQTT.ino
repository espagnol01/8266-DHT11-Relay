//Import Libraries
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

// DHT11 Temperature chip i/o
#define DHTPIN 2 //NodeMCU pin D4
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);
char tmp[50];
char hum[50];
int tellstate = 0;

int RelayPin = 5; //Set Relay to be connected to digital pin 5 (NodeMCU pin D1)

//Wifi/Broker parameters
const char* ssid = "comhem_D64E3F"; //Wifi network SSID
const char* password = "b7at87t6"; //Wifi network PASSWORD
const char* mqtt_server = "192.168.0.10"; //Broker IP Address

//MQTT Configuration
WiFiClient espClient; //Creates a partially initialised client instance.
PubSubClient client(espClient); //Before it can be used, the server details must be configured

void connect_to_MQTT() {
 client.setServer(mqtt_server, 1883);//Set the MQTT server details
  client.setCallback(callback);

  if (client.connect("temperature_sensor_relay")) {
    Serial.println("Connected to MQTT Server");
    client.subscribe("sensor/relay");
  } else {
    Serial.println("Could not connect to MQTT");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic);
  Serial.print(" => ");

char* payload_str;
  payload_str = (char*) malloc(length + 1);
  memcpy(payload_str, payload, length);
  payload_str[length] = '\0';
Serial.println(String(payload_str));
  
  if ( String(topic) == "sensor/relay" ) {
    if (String(payload_str) == "on" ) {
      digitalWrite(RelayPin, HIGH);   // turn the RELAY on
      client.publish("sensor/relay_state","on");
    } else if ( String(payload_str) == "off" ) {
      digitalWrite(RelayPin, LOW);    // turn the RELAY off
      client.publish("sensor/relay_state","off");
    } else {
      Serial.print("I cannot process ");
      Serial.print(String(payload_str));
      Serial.print(" on topic ");
      Serial.println( String(topic));
    }
  }
   free(payload_str);
}

void setup() {
  Serial.begin(115200); //Sets the data rate in bits per second (baud) for serial data transmission.

// Connecting to our WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // initialize pin 5, where the relay is connected to.
  pinMode(RelayPin, OUTPUT);

  connect_to_MQTT();
}

void getTemperature() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Temp as string
  itoa(t,tmp,10);
  client.publish("sensor/temperature",tmp);
  Serial.println(tmp);

  //Humidity as string
  itoa(h,hum,10);
  client.publish("sensor/humidity",hum);
  Serial.println(hum);
}

void getVoltage() {
  int iVcc = ESP.getVcc();
  float fVcc = (float)ESP.getVcc() / 1000;
  char cVcc[5];
  dtostrf(fVcc,5, 3, cVcc);
  
  client.publish("sensor/voltage",cVcc);
  Serial.println(cVcc);
}
  
void loop() {
  client.loop();

  if (! client.connected()) {
    Serial.println("Not connected to MQTT....");
    connect_to_MQTT();
  }

 //Every 60 seconds read the temperature, humidity and relay state
  if ( (millis() - tellstate) > 60000 ) {
    getTemperature();
    getVoltage();
    if ( digitalRead(RelayPin) ) {
       client.publish("sensor/relay_state","on");
    } else {
      client.publish("sensor/relay_state","off");
    }
    tellstate = millis();
  }
}
