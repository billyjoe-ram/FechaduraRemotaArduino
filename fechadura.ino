from machine import Pin, I2C
from time import sleep, time, ticks_ms, ticks_diff
from i2c_lcd import I2cLcd

AddressOfLcd = 0x27
i2c = I2C(scl=Pin(22), sda=Pin(21), freq=400000)
lcd = I2cLcd(i2c, AddressOfLcd, 2, 16)

led_verde = Pin(4, Pin.OUT)
led_vermelho = Pin(2, Pin.OUT)

senha_padrao = "1234"
tentativas_erradas = 0
bloqueado_ate = 0
aberto = 0

step_pin = Pin(5, Pin.OUT)
dir_pin = Pin(17, Pin.OUT)

def escreve_lcd(valor):
    delay = 0.3
    max_chars = 32

    lcd.move_to(0, 0)
    lcd.putstr(" " * max_chars)

    if len(valor) <= max_chars:
        lcd.move_to(0,0)
        lcd.putstr(valor)

        sleep(4)
    else:
        for i in range(len(valor) - max_chars + 1):
            janela = valor[i : i + max_chars]
            lcd.move_to(0, 0)
            lcd.putstr(janela)
            sleep(delay)
    
    lcd.move_to(0, 0)
    lcd.putstr(" " * max_chars)

def step(delay=0.001):
    step_pin.value(1)
    sleep(delay)
    step_pin.value(0)
    sleep(delay)

def abrir(duracao=2):    
    escreve_lcd("Abrindo porta")
    dir_pin.value(0)
    inicio = ticks_ms()
    while ticks_diff(ticks_ms(), inicio) < duracao * 1000:
        step()

def fechar(duracao=2):
    escreve_lcd("Fechando porta")  
    dir_pin.value(1)
    inicio = ticks_ms()
    while ticks_diff(ticks_ms(), inicio) < duracao * 1000:
        step()

def acender_led_verde():
    led_verde.on()
    sleep(1)
    led_verde.off()

def acender_led_vermelho():
    led_vermelho.on()
    sleep(1)
    led_vermelho.off()

def processar_movimento_motor():
    global aberto
    
    if aberto == 0:
        escreve_lcd("Senha correta! Abrindo fechadura...")
        acender_led_verde()
        abrir()
        aberto = 1
    else:
        acender_led_vermelho()
        fechar()
        aberto = 0

def redefinir_senha():
    global senha_padrao
    escreve_lcd("Modo de redefinicao de senha ativado.")
    escreve_lcd("Digite a nova senha de 4 dígitos: ")
    nova = input()
    if len(nova) == 4 and nova.isdigit():
        senha_padrao = nova
        acender_led_verde()
        escreve_lcd("Senha redefinida com sucesso!")
    else:
        escreve_lcd("Senha invalida, redefinicao cancelada.")

def processar_senha(senha_digitada):
    global senha_padrao, tentativas_erradas, bloqueado_ate

    if time() < bloqueado_ate:
        escreve_lcd("Sistema bloqueado. Aguarde 5 minutos.")
        return

    if senha_digitada == "0":
        escreve_lcd("Digite a senha atual para redefinir: ")
        senha_teste = input()

        if senha_teste == senha_padrao:
            redefinir_senha()
            return
        else:
            escreve_lcd("Senha incorreta. Redefinição cancelada.")
            return

    if senha_digitada == senha_padrao:
        processar_movimento_motor()
        tentativas_erradas = 0
    else:
        tentativas_erradas += 1
        acender_led_vermelho()
        escreve_lcd("Senha incorreta! Tentativa: " + tentativas_erradas)

        if tentativas_erradas >= 3:
            bloqueado_ate = time() + 300
            escreve_lcd("Muitas tentativas erradas. Sistema bloqueado por 5 minutos.")

escreve_lcd("Fechadura inteligente iniciada.")
escreve_lcd("Senha padrao inicial: 1234 (modo de redefinicao).")

while True:
    escreve_lcd("Para modo de redefinicao digite '0'")
    escreve_lcd("Digite a senha de 4 digitos: ")
    senha_digitada = input()
    processar_senha(senha_digitada)
