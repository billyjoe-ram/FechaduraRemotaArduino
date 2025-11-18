#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== CONFIG WIFI =====
const char* ssid = "ESP32_Hotspot";
const char* password = "1234abcd";

// Configuração: Endereço I2C, Colunas, Linhas
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== SERVIDOR WEB =====
WebServer server(80);

// ===== PINOS =====
int led_verde = 4;
int led_vermelho = 2;
int step_pin = 5;
int dir_pin = 17;

// ===== VARIÁVEIS DO SISTEMA =====
String senha_padrao = "1234";
int tentativas_erradas = 0;
unsigned long bloqueado_ate = 0;
int aberto = 0;


void escreve_lcd(String valor) {
  float delay_scroll = 0.3;
  int max_chars = 32;

  lcd.clear();
  lcd.setCursor(0, 0);  

  if (valor.length() <= max_chars) {
      lcd.setCursor(0, 0);
      lcd.print(valor);
      delay(4000);
  } else {
      for (int i = 0; i <= valor.length() - max_chars; i++) {
        String janela = valor.substring(i, i + max_chars);
        lcd.setCursor(0, 0);
        lcd.print(janela);
        delay(delay_scroll * 1000);
      }
  }  
}

void step_motor(float d = 0.001) {
  digitalWrite(step_pin, HIGH);
  delayMicroseconds(d * 1000000);
  digitalWrite(step_pin, LOW);
  delayMicroseconds(d * 1000000);
}

void abrir(int duracao = 2) {
  escreve_lcd("Abrindo porta");
  digitalWrite(dir_pin, LOW);
  unsigned long inicio = millis();
  while (millis() - inicio < duracao * 1000) {
    step_motor();
  }
}

void fechar(int duracao = 2) {
  escreve_lcd("Fechando porta");
  digitalWrite(dir_pin, HIGH);
  unsigned long inicio = millis();
  while (millis() - inicio < duracao * 1000) {
    step_motor();
  }
}

void acender_led_verde() {
  digitalWrite(led_verde, HIGH);
  delay(1000);
  digitalWrite(led_verde, LOW);
}

void acender_led_vermelho() {
  digitalWrite(led_vermelho, HIGH);
  delay(1000);
  digitalWrite(led_vermelho, LOW);
}

void processar_movimento_motor() {
  if (aberto == 0) {
    escreve_lcd("Senha correta! Abrindo fechadura...");
    acender_led_verde();
    abrir();
    aberto = 1;
  } else {
    acender_led_vermelho();
    fechar();
    aberto = 0;
  }
}

void redefinir_senha() {
  escreve_lcd("Modo de redefinicao de senha ativado.");
  escreve_lcd("Digite a nova senha de 4 dígitos: ");
    
  //while (Serial.available() == 0) {}
  //String nova = Serial.readStringUntil('\n');
  nova.trim();

  if (nova.length() == 4 && nova.toInt() >= 0) {
    senha_padrao = nova;
    acender_led_verde();
    escreve_lcd("Senha redefinida com sucesso!");
  } else {
    escreve_lcd("Senha invalida, redefinicao cancelada.");
  }
}

void processar_senha(String senha_digitada) {
    unsigned long agora = millis() / 1000;

    if (agora < bloqueado_ate) {
      escreve_lcd("Sistema bloqueado. Aguarde 5 minutos.");
      return;
    }

    if (senha_digitada == "0") {
      escreve_lcd("Digite a senha atual para redefinir: ");

      //while (Serial.available() == 0) {}
      //String senha_teste = Serial.readStringUntil('\n');
      senha_teste.trim();

      if (senha_teste == senha_padrao) {
        redefinir_senha();
        return;
      } else {
        escreve_lcd("Senha incorreta. Redefinição cancelada.");
        return;
      }
    }

    if (senha_digitada == senha_padrao) {
      processar_movimento_motor();
      tentativas_erradas = 0;
    } else {
      tentativas_erradas++;
      acender_led_vermelho();

      escreve_lcd("Senha incorreta! Tentativa: " + String(tentativas_erradas));

      if (tentativas_erradas >= 3) {
        bloqueado_ate = agora + 300;
        escreve_lcd("Muitas tentativas erradas. Sistema bloqueado por 5 minutos.");
      }
    }
}

void setup() {
  Serial.begin(115200);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Inicializando...");

  // PINOS
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);

  // WIFI modo AP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  lcd.clear();
  lcd.print("Hotspot Ativo");

  // SERVIDOR
  server.on("/", handleRoot);
  server.on("/action", handleAction);
  server.begin();

  delay(1000);
  lcd.clear();
    
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();

  escreve_lcd("Fechadura inteligente iniciada.");
  escreve_lcd("Senha padrao inicial: 1234 (modo de redefinicao).");
}

void loop() {
  server.handleClient();
}

// ===== PÁGINA PRINCIPAL =====
void handleRoot() {
    String estado = aberto ? "Aberta" : "Fechada";
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<title>Fechadura Inteligente</title>";
    html += "<style>";
    html += "body{font-family:Arial;text-align:center;margin-top:50px;}";
    html += "button{font-size:20px;padding:15px 30px;margin:10px;}";
    html += ".status{font-size:22px;margin:20px;}";
    html += "</style></head><body>";
    html += "<h1>Fechadura Inteligente</h1>";
    html += "<div class='status'>Porta: " + estado + "</div>";
    html += "<form action='/action' method='get'>";
    html += "<input type='hidden' name='senha' value='" + senha_padrao + "'>";
    html += "<button name='acao' value='abrir'>Abrir</button>";
    html += "<button name='acao' value='fechar'>Fechar</button>";
    html += "</form></body></html>";

    server.send(200, "text/html", html);
}

// ===== VERIFICA SENHA E EXECUTA AÇÃO =====
void handleAction() {
    String senha_digitada = server.arg("senha");
    String acao = server.arg("acao");

    // Bloqueio por tentativas
    if (millis() < bloqueado_ate) {
        escreve_lcd("Sistema bloqueado. Aguarde 5 minutos.");
        return;
    }

    // SENHA CORRETA
    if (senha_digitada == senha_padrao) {
        digitalWrite(ledVerdePin, HIGH);
        delay(300);
        digitalWrite(ledVerdePin, LOW);

        if (acao == "abrir" && !aberto)
            abrir();
        else if (acao == "fechar" && aberto)
            fechar();

        tentativas_erradas = 0;
        handleRoot();
        return;
    }

    // SENHA ERRADA
    tentativas_erradas++;
    digitalWrite(ledVermelhoPin, HIGH);
    delay(300);
    digitalWrite(ledVermelhoPin, LOW);

    if (tentativas_erradas >= 3) {
        bloqueado_ate = millis() + 300000;
        escreve_lcd("Muitas tentativas. Sistema bloqueado por 5 minutos.");
    } else {
        escreve_lcd("Senha incorreta! Tentativa " + String(tentativas_erradas));
    }
}
