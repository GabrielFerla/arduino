#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define PIN_LCD 0x27
#define buzzerPin 4
#define buttonPin 34

unsigned long intervalTime = millis();

LiquidCrystal_I2C lcd(PIN_LCD, 16, 2);

const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "Cliente";
const char *TOPIC_PUBLISH_TIMER = "Med/timer";

WiFiClient espClient;
PubSubClient MQTT(espClient);

void setup() {
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  lcd.begin(16, 2);  
  lcd.init();
  lcd.backlight();
  lcd.clear();

  WiFi.begin(SSID, PASSWORD);
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);

  lcd.print("Med Timer");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long currentTime = millis();

  checkWiFiAndMQTT();
  MQTT.loop();

  if(currentTime - intervalTime >= 5000)
  {
    showMsg("Tomar Remedio", 0);
    showMsg("Ibuprofeno", 1);
    digitalWrite(buzzerPin, HIGH);
    char msgContent[] = "Tomar Remédio: Ibuprofeno";
    MQTT.publish(TOPIC_PUBLISH_TIMER, msgContent);

    intervalTime = currentTime;
  }

  if(digitalRead(buttonPin) == HIGH)
  {
    lcd.clear();
    digitalWrite(buzzerPin, LOW);
  }

  delay(100);
}

void showMsg(String msg, int line) {
  lcd.setCursor(0, line);
  lcd.print(msg);
}

void checkWiFiAndMQTT() 
{
  if (WiFi.status() != WL_CONNECTED) reconnectWiFi();
    if (!MQTT.connected()) reconnectMQTT();
}

void reconnectWiFi() {
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    if (MQTT.connect(ID_MQTT)) {
      MQTT.subscribe(TOPIC_PUBLISH_TIMER);
    } else {
      delay(2000);
    }
  }
}