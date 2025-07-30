#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_ADS1X15.h>


const char *ssid = "rede-wifi";
const char *password = "senhadarede";
const char *mqtt_server = "192.168.0.180";
const uint16_t mqtt_port = 1883;

const char *mqtt_user = "usuariomqtt";
const char *mqtt_password = "senhamqtt";

const uint8_t daq_addr = 0x4A; 
const adsGain_t daq_gain = GAIN_TWO;

Adafruit_ADS1115 daqboard(daq_addr);


WiFiClient esp_client;
PubSubClient client(esp_client);

const char *bname = "pwm";
//MQTTAnem mqtt_anem(bname, &anem, &client);


void setup_daq(Adafruit_ADS1115 &daqboard, adsGain_t gain=GAIN_TWO){
  daqboard.setGain(gain);
  if (!daqboard.begin()){
    return;
  }

})

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentando conectar com o Broker do MQTT ...");
    // Attempt to connect
    if (client.connect("JAnem", mqtt_user, mqtt_password)) {
      Serial.println("conectado");
      mqtt_anem.publish_params();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup(){
  Serial.begin(9600);
  
  setup_daq(daqboard, daq_gain)
  WiFi.begin(ssid, password);
  
  Serial.println("\nConnecting");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }  
  Serial.println("\nConnected to the WiFi network!");

  Serial.print("\nLocal ESP32 IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  
  
  anem.setup_anemometer();
  anem.setup_temperature();

  reconnect();
  

}



void loop(){

  if (!client.connected()) {
    reconnect();
    Serial.println(WiFi.localIP());
  }

  mqtt_anem.loop();


}



// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}