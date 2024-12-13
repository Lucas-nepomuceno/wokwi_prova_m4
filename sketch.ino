#include <WiFi.h>
#include <HTTPClient.h>

#define LED_VERDE 2 // Pino utilizado para controle do led verda
#define LED_VERMELHO 40 // Pino utilizado para controle do led vermelho
#define LED_AMARELO 9 // Pino utilizado para controle do led azul

const int botaoPino = 18;  // Pino utilizado para o botão the
int estadoBotao = 0;  // Variável para ler o estado do botao

const int LDRPino = 4;  // Pino utilizado para o sensor LDR
int limite=600; // Limite do sensor LDR
int clique = 0;

unsigned long tempo = 0;
unsigned long auxiliar = 0;
int tentativas = 0;
unsigned long ultimoDebounce = 0; // última vez que o botão foi pressionado
unsigned long tempoDebounce = 400;   // intevalo de delay

void mandaRequisicao(){
  if(WiFi.status() == WL_CONNECTED){ // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do Resultado da Requisição HTTP

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      }
      http.end();
    }

  else {
    Serial.println("WiFi Disconnected");
  }
}

void setup() {

  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(LED_VERDE,OUTPUT);
  pinMode(LED_VERMELHO,OUTPUT);
  pinMode(LED_AMARELO,OUTPUT);

  // Inicialização das entradas
  pinMode(botaoPino, INPUT_PULLDOWN); // Inicializa o botão como input_pulldown

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_AMARELO, LOW);

  Serial.begin(9600); // Configuração para debug por interface serial entre ESP e computador com baud rate de 9600

  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECTED & tentativas <= 20) {
    delay(100);
    Serial.print(".");
    tentativas++;
  }
  tentativas = 0;
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando que saiu do loop acima, o ESP32 agora está conectado ao WiFi (outra opção é colocar este comando dentro do if abaixo)
} 

void loop() {

  // Verifica estado do botão
  estadoBotao = digitalRead(botaoPino);
  if (estadoBotao == HIGH) {
    if ((millis() - ultimoDebounce) > tempoDebounce) {
      Serial.println("Botão pressionado!");
    }
  } else {
    Serial.println("Botão não pressionado!");
  }

  tempo = millis();
  int LDRStatus=analogRead(LDRPino);

  if(LDRStatus>=limite){
    if (tempo - auxiliar > 1000) {
      digitalWrite(LED_AMARELO, HIGH);
      auxiliar = tempo;
    } else {
      digitalWrite(LED_AMARELO, LOW);
    }

  }else{

    if (tempo - auxiliar >= 0) {
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_VERMELHO, LOW);
      digitalWrite(LED_AMARELO, LOW);
    }

    if (tempo - auxiliar >= 3000) {
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_VERMELHO, LOW);
      digitalWrite(LED_AMARELO, HIGH);
    }

    if (tempo - auxiliar >= 5000) {
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_VERMELHO, HIGH);
      if(tempo - auxiliar >= 8000) {
        auxiliar = tempo;
      }


      if (estadoBotao) {
        if ((millis() - ultimoDebounce) > tempoDebounce) {
          clique++;
          delay(1000);
          auxiliar = tempo;
        }

        if (clique >= 3) {
          mandaRequisicao();
          clique = 0;
        }

        ultimoDebounce = millis();
      }

    }

  }
}