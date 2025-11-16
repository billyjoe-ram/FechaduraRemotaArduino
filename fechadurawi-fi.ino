#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servidor
WebServer server(80);

// Pinos
const int ledVerdePin = 4;
const int ledVermelhoPin = 2;
const int stepPin = 5;
const int dirPin = 17;

String senha_padrao = "1234";
int tentativas_erradas = 0;
unsigned long bloqueado_ate = 0;
bool aberto = false;

void abrir(int duracao = 2000);
void fechar(int duracao = 2000);
void handleRoot();
void handleForm();

void setup() {
    Serial.begin(115200);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Inicializando");

    pinMode(ledVerdePin, OUTPUT);
    pinMode(ledVermelhoPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    // WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Conectando...");
    }
    Serial.println("WiFi conectado!");
    Serial.println(WiFi.localIP());

    // Servidor
    server.on("/", handleRoot);
    server.on("/form", handleForm);
    server.begin();

    lcd.clear();
    lcd.print("WiFi OK");
}

void loop() {
    server.handleClient();
}

void abrir(int duracao) {
    lcd.clear();
    lcd.print("Abrindo porta");
    digitalWrite(dirPin, HIGH);
    for (int i = 0; i < duracao / 2; i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
    }
    aberto = true;
}

void fechar(int duracao) {
    lcd.clear();
    lcd.print("Fechando porta");
    digitalWrite(dirPin, LOW);
    for (int i = 0; i < duracao / 2; i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
    }
    aberto = false;
}

void handleRoot() {
    String html = "<html><body><h1>Fechadura Inteligente</h1>"
                  "<form action='/form' method='get'>"
                  "<input type='text' name='senha' placeholder='Digite a senha'>"
                  "<input type='submit' value='Enviar'></form>"
                  "</body></html>";
    server.send(200, "text/html", html);
}

void handleForm() {
    String senha_digitada = server.arg("senha");

    if (millis() < bloqueado_ate) {
        server.send(200, "text/html", "Sistema bloqueado. Aguarde 5 minutos.");
        return;
    }

    if (senha_digitada == senha_padrao) {
        digitalWrite(ledVerdePin, HIGH);
        delay(300);
        digitalWrite(ledVerdePin, LOW);

        if (aberto) {
            fechar();
            server.send(200, "text/html", "Senha correta! Porta fechada.");
        } else {
            abrir();
            server.send(200, "text/html", "Senha correta! Porta aberta.");
        }

        tentativas_erradas = 0;

    } else {
        tentativas_erradas++;
        digitalWrite(ledVermelhoPin, HIGH);
        delay(300);
        digitalWrite(ledVermelhoPin, LOW);

        if (tentativas_erradas >= 3) {
            bloqueado_ate = millis() + 300000;
            server.send(200, "text/html", "Muitas tentativas falhas. Sistema bloqueado por 5 minutos.");
        } else {
            server.send(200, "text/html", "Senha incorreta! Tentativa " + String(tentativas_erradas));
        }
    }
}
