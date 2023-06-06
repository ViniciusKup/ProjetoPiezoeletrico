#include "EmonLib.h" 
EnergyMonitor emonl;
float ruido = 0;

const int pinoSensor = A3; //PINO ANALÓGICO EM QUE O SENSOR ESTÁ CONECTADO
int pinLed = 10;
int moduloRele = 13;

float tensaoEntrada = 0.0; //VARIÁVEL PARA ARMAZENAR O VALOR DE TENSÃO DE ENTRADA DO SENSOR
float tensaoMedida = 0.0; //VARIÁVEL PARA ARMAZENAR O VALOR DA TENSÃO MEDIDA PELO SENSOR

float valorR1 = 30000.0; //VALOR DO RESISTOR 1 DO DIVISOR DE TENSÃO
float valorR2 = 7500.0; // VALOR DO RESISTOR 2 DO DIVISOR DE TENSÃO
int leituraSensor = 0; //VARIÁVEL PARA ARMAZENAR A LEITURA DO PINO ANALÓGICO

void setup(){
  pinMode(pinoSensor, INPUT); //DEFINE O SENSOR DE TENSÃO PINO COMO ENTRADA
  pinMode(pinLed, OUTPUT);
  pinMode(moduloRele, OUTPUT);

  emonl.current(A1, 5.5);
  Serial.begin(9600); //INICIALIZA A SERIAL
}

void loop(){

  double Irms = emonl.calcIrms(1480);
  Irms = Irms - ruido;

  
  if (Irms < 0){
    Irms = 0;
  }
  

  leituraSensor = analogRead(pinoSensor); //FAZ A LEITURA DO PINO ANALÓGICO E ARMAZENA NA VARIÁVEL O VALOR LIDO
  tensaoEntrada = (leituraSensor * 5.0) / 1024.0; //VARIÁVEL RECEBE O RESULTADO DO CÁLCULO
  tensaoMedida = tensaoEntrada / (valorR2/(valorR1+valorR2)); //VARIÁVEL RECEBE O VALOR DE TENSÃO DC MEDIDA PELO SENSOR
  digitalWrite(pinLed, HIGH);

  if(tensaoMedida >= 12){
  // Ativa o relé
  digitalWrite(moduloRele, HIGH);
  Serial.println("rele ligado");
  } else if (tensaoMedida <= 7) {  
  // Desativa o relé
  digitalWrite(moduloRele, LOW);
  Serial.println("rele desligaddo");
  }

  Serial.println("Valor da Corrente DC: ");
  Serial.print(Irms,3);
  Serial.println("A");
  delay(500);
  Serial.println("Tensão DC medida: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(tensaoMedida,3); //IMPRIME NA SERIAL O VALOR DE TENSÃO DC MEDIDA E LIMITA O VALOR A 2 CASAS DECIMAIS
  Serial.println("V"); //IMPRIME O TEXTO NA SERIAL
  delay(500); //INTERVALO DE 500 MILISSEGUNDOS

  
}