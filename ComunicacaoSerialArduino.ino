// Biblioteca necessária para a comunicação serial
#include <SoftwareSerial.h>

// Pino do sensor conectado à entrada analógica A0
const int sensorPin1 = A0;
const int sensorPin2 = A1;

// Objeto da biblioteca SoftwareSerial para a comunicação com o ESP8266
SoftwareSerial espSerial(2, 3);  // RX, TX (Pinos digitais 2 e 3)

void setup() {
  // Inicializa a comunicação serial com a velocidade de 9600 bps
  Serial.begin(9600);
  
  // Inicializa a comunicação serial com o ESP8266 na velocidade de 9600 bps
  espSerial.begin(9600);
}

void loop() {
  // Lê o valor do sensor
  int sensorValue1 = analogRead(sensorPin1);
  int sensorValue2 = analogRead(sensorPin2);
  
// Calcula a potência
  float potencia = sensorValue1 * sensorValue2;
  
  // Envia os dados para o ESP8266 via comunicação serial
  espSerial.print(sensorValue1);
  espSerial.print(',');
  espSerial.print(sensorValue2);
  espSerial.print(',');
  espSerial.print(potencia);
  espSerial.print('\n');  // Adiciona uma nova linha para indicar o fim da transmissão
  
  // Aguarda um pequeno intervalo antes de ler os próximos valores dos sensores
  delay(1000);

  Serial.println(sensorValue1);
  Serial.println(sensorValue2);
  Serial.println(potencia);
  delay(500);
}
