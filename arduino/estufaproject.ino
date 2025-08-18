#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <DHT.h>

using namespace websockets;

// ==== Configuração WiFi e WebSocket ====
const char* ssid = "IFPR_IoT";
const char* password = "31mfk76m";
const char* websocketServer = "ws://172.16.4.13:81/";

WebsocketsClient socket;
bool connected = false;

// ==== Configuração do Sensor DHT22 ====
#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0.0;
float humidity = 0.0;

// ==== LED e Botão ====
const int ledPin = 2;
bool ledState = false;
const int buttonPin = 0;

// ==== Setup ====
void setup() {
  Serial.begin(115200);

  // Inicializa sensores e pinos
  dht.begin();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Conexões
  connectWiFi();
  connectToWebSocket();

  socket.onMessage(handleMessage);
  socket.onEvent(handleEvent);
}

// ==== Loop ====
void loop() {
  if (!connected) {
    Serial.println("Tentando reconectar ao servidor WebSocket...");
    connectToWebSocket();
  }

  socket.poll();

  // Leitura do DHT22
  String dhtData = loadDHT22Data();
  socket.send(dhtData + ":dht:localhost:esp");
  delay(1000);

  // Controle do LED via botão
  if (digitalRead(buttonPin) == LOW) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    socket.send(String(ledState) + ":led:localhost:esp");
    delay(200);
  }
}

// ==== Manipula mensagens do WebSocket ====
void handleMessage(WebsocketsMessage message) {
  Serial.print("Mensagem recebida: ");
  Serial.println(message.data());

  String data = message.data();
  String status = parseData(data, 1);
  String sensor = parseData(data, 2);

  if (sensor == "led") {
    ledState = status.toInt();
    digitalWrite(ledPin, ledState);
  }
}

// ==== Eventos do WebSocket ====
void handleEvent(WebsocketsEvent event, WSInterfaceString data) {
  switch (event) {
    case WebsocketsEvent::ConnectionOpened:
      Serial.println("WebSocket conectado.");
      socket.send("Hello, server!");
      connected = true;
      break;
    case WebsocketsEvent::ConnectionClosed:
      Serial.println("WebSocket desconectado.");
      connected = false;
      break;
    case WebsocketsEvent::GotPing:
      Serial.println("Ping recebido.");
      break;
    case WebsocketsEvent::GotPong:
      Serial.println("Pong recebido.");
      break;
    default:
      break;
  }
}

// ==== Conecta ao WebSocket ====
void connectToWebSocket() {
  connected = socket.connect(websocketServer);
  if (connected) {
    Serial.println("Conectado ao WebSocket!");
  } else {
    Serial.println("Falha na conexão com o WebSocket.");
  }
}

// ==== Conecta ao WiFi ====
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado ao WiFi. IP: ");
  Serial.println(WiFi.localIP());
}

// ==== Lê dados do DHT22 ====
String loadDHT22Data() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Falha na leitura do DHT22!");
    return "0,0";
  }

  Serial.println("Temperatura: " + String(temperature) + " °C");
  Serial.println("Umidade: " + String(humidity) + " %");

  return String(temperature) + "," + String(humidity);
}

// ==== Função auxiliar para dividir mensagens ====
String parseData(String data, int index) {
  String result = "";
  int currentIndex = 1;
  int start = 0;
  int end = data.indexOf(":");

  while (end != -1 && currentIndex <= index) {
    if (currentIndex == index) {
      result = data.substring(start, end);
      break;
    }

    start = end + 1;
    end = data.indexOf(":", start);
    currentIndex++;
  }

  return result;
}
