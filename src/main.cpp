#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include "html/index_html.h"

#define MOTOR_1 D2
#define MOTOR_2 D1
#define MOTOR_SPEED D0

#define STEER_1 D3
#define STEER_2 D4
#define STEER_SPEED D5

const char *ssid = "scylla";
const char *pass = "dev-scylla";

ESP8266WebServer server(80);
WebSocketsServer socket(81);

void drive(int speed, int turn);

void root() {
  server.sendHeader("Content-Encoding", "gzip");
  server.send(200, "text/html", index_html, INDEX_HTML_SIZE);
}

void socketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
  case WStype_CONNECTED: {
    Serial.println("connected!");
    break;
  };
  case WStype_BIN: {
    int mode = payload[0];
    if (mode == 0) {
      int speed = ((int)payload[1] - 50) * 2;
      int turn = (int)payload[2] - 1;

      drive(speed, turn);
    }
    break;
  };
  default:
    break;
  };
}

void setup() {
  Serial.begin(9600);

  pinMode(MOTOR_1, OUTPUT);
  pinMode(MOTOR_2, OUTPUT);
  pinMode(MOTOR_SPEED, OUTPUT);
  pinMode(STEER_1, OUTPUT);
  pinMode(STEER_2, OUTPUT);
  pinMode(STEER_SPEED, OUTPUT);

  IPAddress local_ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  bool success = WiFi.softAP(ssid, pass);

  Serial.println(success ? "Success" : "Failed");

  if (success) {
    Serial.println(WiFi.softAPIP().toString());
  }

  server.enableCORS(true);
  server.on("/", root);
  socket.onEvent(socketEvent);

  server.begin();
  socket.begin();
}

void loop() {
  server.handleClient();
  socket.loop();
}

void drive(int speed, int turn) {
  digitalWrite(MOTOR_1, speed > 0);
  digitalWrite(MOTOR_2, speed <= 0);

  speed = abs(speed);
  if (speed != 0) {
    int throttle = map(speed, 0, 100, 0, 75);
    Serial.print("speed = ");
    Serial.println(throttle);
    analogWrite(MOTOR_SPEED, throttle);
  } else
    analogWrite(MOTOR_SPEED, 0);

  switch (turn) {
  case -1:
    digitalWrite(STEER_1, 1);
    digitalWrite(STEER_2, 0);
    analogWrite(STEER_SPEED, 250);
    break;
  case 1:
    digitalWrite(STEER_1, 0);
    digitalWrite(STEER_2, 1);
    analogWrite(STEER_SPEED, 250);
    break;
  default:
    analogWrite(STEER_SPEED, 0);
    break;
  }
}
