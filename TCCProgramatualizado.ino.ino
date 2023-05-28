#include "EmonLib.h"
#define VOLT_CAL 579  //VALOR DE CALIBRAÇÃO (DEVE SER AJUSTADO EM PARALELO COM UM MULTÍMETRO)

EnergyMonitor SCT013;
EnergyMonitor emon1; //CRIA UMA INSTÂNCIA

String ssid = "Simulator Wifi"; // SSID para conectar em um WiFi, neste caso, uma rede "Simulada"
String password = ""; // Este WiFi não possui senha
const int httpPort = 80;
String _apiHost = "api.thingspeak.com"; //URL do Serviço de Nuvem (ThingSpeak)
String _apiIdWrite = "YZGN826WQ1O8AQ39"; //Colar sua key de escrita - substituindo KEY_ESCRITA
String _apiRequestUpdate = "/update?api_key=";
String _apiField1 = "&field1=";
String _apiField2 = "&field2=";
String _apiField3 = "&field3=";
String _apiPathReadChannel = "/channels/";
String _apiChannelID = "1728815"; // Colar seu channel ID - substituindo CHANNEL_ID
String _apiReadFeed = "/feeds/last.json?api_key=";
String _apiIdRead = "M17XL7RZ1P3EH5Y9"; //Colar sua key de leitura - substituindo KEY_LEITURA
String _apiIdRead2 = "MNJ015O2U66KAOG0";
String _apiReadEnd = "";
String uriWrite1 = _apiRequestUpdate + _apiIdWrite + _apiField1;
String uriWrite2 = _apiRequestUpdate + _apiIdWrite + _apiField2;
String uriRead = _apiPathReadChannel + _apiChannelID + _apiReadFeed + _apiIdRead + _apiReadEnd;
String uriRead2 = _apiPathReadChannel + _apiChannelID + _apiReadFeed + _apiIdRead2 + _apiReadEnd;

int pinSCT = A0;   //Pino analógico conectado ao SCT-013
int pinVolt = A2; //Pino analógico conectado ao ZMPT101b
int potencia; //Variavel calculada pela multiplicação do valor dos sensores
const int pinoSensor = A3; //PINO ANALÓGICO EM QUE O SENSOR ESTÁ CONECTADO
float voltage;
 
float tensaoEntrada = 0.0; //VARIÁVEL PARA ARMAZENAR O VALOR DE TENSÃO DE ENTRADA DO SENSOR
float tensaoMedida = 0.0; //VARIÁVEL PARA ARMAZENAR O VALOR DA TENSÃO MEDIDA PELO SENSOR
 
float valorR1 = 30000.0; //VALOR DO RESISTOR 1 DO DIVISOR DE TENSÃO
float valorR2 = 7500.0; // VALOR DO RESISTOR 2 DO DIVISOR DE TENSÃO
int leituraSensor = 0; //VARIÁVEL PARA ARMAZENAR A LEITURA DO PINO ANALÓGICO


int setupESP8266() {
Serial.begin(115200);
Serial.println("AT");
delay(10);
if (!Serial.find("OK"))
return 1;
Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
delay(10);
if (!Serial.find("OK"))
return 2;
Serial.println("AT+CIPSTART=\"TCP\",\"" + _apiHost + "\"," + httpPort);
delay(50);
if (!Serial.find("OK"))
return 3;
return 0;
}

float receberDadosESP8266(){
String httpPacket3 = "GET " + uriRead + " HTTP/1.1\r\nHost: " + _apiHost + "\r\n\r\n";
int length3 = httpPacket3.length();
Serial.print("AT+CIPSEND=");
Serial.println(length3);
delay(10);
Serial.print(httpPacket3);
delay(10);
while(!Serial.available()) delay(5);
String saida = "";
if (Serial.find("\r\n\r\n")){
delay(5);
unsigned int i = 0;
while (!Serial.find("\"field1\":")){}
while (i<60000) {
if(Serial.available()) {
int c = Serial.read();
if (c == '.')
break;
if (isDigit(c)) {
saida += (char)c;
}
}
i++;
}
}
return saida.toFloat();
}

float receberDadosESP82662(){
String httpPacket3 = "GET " + uriRead2 + " HTTP/1.1\r\nHost: " + _apiHost + "\r\n\r\n";
int length3 = httpPacket3.length();
Serial.print("AT+CIPSEND=");
Serial.println(length3);
delay(10);
Serial.print(httpPacket3);
delay(10);
while(!Serial.available()) delay(5);
String saida = "";
if (Serial.find("\r\n\r\n")){
delay(5);
unsigned int i = 0;
while (!Serial.find("\"field2\":")){}
while (i<60000) {
if(Serial.available()) {
int c = Serial.read();
if (c == '.')
break;
if (isDigit(c)) {
saida += (char)c;
}
}
i++;
}
}
return saida.toFloat();
}




void enviaCorrenteESP8266() {
int corrente = map(analogRead(pinSCT),0,1023,0,100);
// Construindo a requisição, tipo GET com HTTP
String httpPacket1 = "GET " + uriWrite1 + String(corrente) + " HTTP/1.1\r\nHost: " + _apiHost + "\r\n\r\n";
int length1 = httpPacket1.length();
// Enviando tamanho da mensagem por Serial para ESP8266
Serial.print("AT+CIPSEND=");
Serial.println(length1);
delay(10);
// Enviando requisicao/mensagem para ESP8266
Serial.print(httpPacket1);
delay(10);
if (!Serial.find("SEND OK\r\n"))
return;
}  
  void enviaTensaoESP8266() { 
int tensao = map(analogRead(pinVolt),0,1023,0,1000);
// Construindo a requisição, tipo GET com HTTP
String httpPacket2 = "GET " + uriWrite2 + String(tensao) + " HTTP/1.1\r\nHost: " + _apiHost + "\r\n\r\n";
int length2 = httpPacket2.length();
// Enviando tamanho da mensagem por Serial para ESP8266
Serial.print("AT+CIPSEND2=");
Serial.println(length2);
delay(10);
// Enviando requisicao/mensagem para ESP8266
Serial.print(httpPacket2);
delay(10);
if (!Serial.find("SEND OK\r\n"))
return;
}


void setup()
{
    setupESP8266();
    SCT013.current(pinSCT, 5.5757); // PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO)
    emon1.voltage(pinVolt, VOLT_CAL, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
    pinMode(pinoSensor, INPUT); //DEFINE O PINO COMO ENTRADA
    
    Serial.begin(9600);
    
}

void loop()
{
    double Irms = SCT013.calcIrms(1480);   // ATRIBUI O VALOR DA CORRENTE À VARIAVEL IRMS (NUMERO DE AMOSTRAS DE LEITURA)
    emon1.calcVI(17,2000); //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)    
    
    float supplyVoltage = emon1.Vrms; //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO

    potencia = Irms * supplyVoltage ; // CALCULA O VALOR DA POTENCIA   
    
    leituraSensor = analogRead(pinoSensor); //FAZ A LEITURA DO PINO ANALÓGICO E ARMAZENA NA VARIÁVEL O VALOR LIDO
    tensaoEntrada = (leituraSensor * 5.0) / 1024.0; //VARIÁVEL RECEBE O RESULTADO DO CÁLCULO
    tensaoMedida = tensaoEntrada / (valorR2/(valorR1+valorR2)); //VARIÁVEL RECEBE O VALOR DE TENSÃO DC MEDIDA PELO SENSOR
    unsigned int x=0;
    float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
    for (int x = 0; x < 10; x++)          //Get 10 samples
    {
    AcsValue = analogRead(A0);           //Read current sensor values   
    Samples = Samples + AcsValue;        //Add samples together
    delay (3);                           // let ADC settle before next sample 3ms
    }
    
    AvgAcs=Samples/10.0;                   //Taking Average of Samples
    voltage=AvgAcs*(5.0 / 1024.0);         //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
    Serial.print("Raw Voltage:");
    Serial.print(voltage);
    AcsValueF = (2.5 - voltage)*1000/0.185; //2.5 is offset,,,   0.185v is rise in output voltage when 1A current flows at input
    Serial.print("\t");
    Serial.print("Motor Current :");
    Serial.print(AcsValueF);               //Print the read current on Serial monitor
    Serial.println(" mA");

    Serial.print("Tensão DC medida: "); //IMPRIME O TEXTO NA SERIAL
    Serial.print(tensaoMedida,2); //IMPRIME NA SERIAL O VALOR DE TENSÃO DC MEDIDA E LIMITA O VALOR A 2 CASAS DECIMAIS
    Serial.println("V"); //IMPRIME O TEXTO NA SERIAL
    delay(500); //INTERVALO DE 500 MILISSEGUNDOS

    Serial.print("Tensão medida na rede AC: "); //IMPRIME O TEXTO NA SERIAL
    Serial.print(supplyVoltage, 0); //IMPRIME NA SERIAL O VALOR DE TENSÃO MEDIDO E REMOVE A PARTE DECIMAL
    Serial.println("V"); 
    
    Serial.print("Corrente = "); //IMPRIME O TEXTO NA SERIAL
    Serial.print(Irms); // IMPRIME NA SERIAL O VALOR DA CORRENTE 
    Serial.println(" A");
    
    Serial.print("Potencia = "); //IMPRIME O TEXTO NA SERIAL
    Serial.print(potencia); //IMPRIME NA SERIAL O VALOR DA POTENCIA CALCULADA
    Serial.println(" W");
   
    delay(500);

    Serial.print(".");
    delay(500);
    Serial.print(".");
    delay(500);
    Serial.println(".");
    delay(500);

    enviaCorrenteESP8266();
    float current = receberDadosESP8266();
    Serial.print(current);
    delay(3000);

    enviaTensaoESP8266();
    float voltage = receberDadosESP82662();
    Serial.print(voltage);
    delay(3000);

}