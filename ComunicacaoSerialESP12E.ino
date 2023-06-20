#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ThingSpeak.h>

#define rxPin D1
#define txPin D0
const char* ssid = "Vinicius Luis";         // Nome da rede Wi-Fi
const char* password = "72116604";    // Senha da rede Wi-Fi
const char* apiKey = "HYYB3QF7I8LOJO54"; // Chave de API do ThingSpeak
const int field = 1;
const char* serverAddress = "api.thingspeak.com";     // Servidor ThingSpeak
const int THINGSPEAK_CHANNEL_ID = 2168510;  // ID do seu canal ThingSpeak

SoftwareSerial arduinoSerial(rxPin, txPin); // RX, TX
ESP8266WebServer webServer;
WiFiClient client;  // Objeto WiFiClient

void setup() {
  // Inicializa a comunicação serial com a velocidade de 9600 bps
  Serial.begin(9600);
  
  // Inicializa a comunicação serial com o Arduino na velocidade de 9600 bps
  arduinoSerial.begin(9600);

    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }

  ThingSpeak.begin(client);  // Inicializa a biblioteca ThingSpeak

  webServer.on("/", handleRoot);
  webServer.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  webServer.handleClient();

   // Verifica se há dados disponíveis na comunicação serial com o Arduino
  if (arduinoSerial.available()) {
    // Lê os valores recebidos do Arduino
    int sensorValue1 = arduinoSerial.parseInt();
    arduinoSerial.read(); // Lê a vírgula
    int sensorValue2 = arduinoSerial.parseInt();
    arduinoSerial.read(); // Lê a vírgula
    float potencia = arduinoSerial.parseFloat();

    ThingSpeak.setField(1, sensorValue1);      // Define o valor do campo 1
    
    int httpCode = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, apiKey);
      if (httpCode == 200) {
        Serial.println("Dados enviados para o ThingSpeak com sucesso!");
      } else {
        Serial.println("Falha ao enviar dados para o ThingSpeak. Código de erro HTTP: " + String(httpCode));
      }

    // Exibe os valores no monitor serial
    Serial.print("Valor da Tensão: ");
    Serial.println(sensorValue1);
    delay(500);
    Serial.print("Valor da Corrente: ");
    Serial.println(sensorValue2);
    delay(500);
    Serial.print("Valor da Potência: ");
    Serial.println(potencia);
    delay(500);
  }
  delay(1000);
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP8266 ThingSpeak</h1>";
  html += "<p>Valor do sensor de tensão: " + String(analogRead(A0)) + "</p>";
  html += "</body></html>";
  webServer.send(200, "text/html", html);
}
