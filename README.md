# Fechadura Remota para Arduino
Fechadura remota feita com arduíno por meio de módulo Wi-Fi

## Funcionamento
A fechadura, funcionando como um receptor Wi-Fi, vai receber uma senha numérica e, se correta, permitirá abertura da porta.

### Participantes:
* **Billy Joe Santos**
* **Gabriel Henrique Rocha**

### Componentes:
* Placa ESP32
* 1 Stepper Motor
* 1 A4988 Stepper Motor Driver
* 2 LED, 1 verde e um vermelho
* 1 Protoboard
* Cabos jumper
* 1 Tela LCD

### Programas utilizados:
**Arduino IDE** (desenvolvimento e conexão com placa ESP32)

### Descrição do projeto:
O projeto é uma fechadura inteligente que utiliza um ESP32 para receber comandos via Wi-Fi. Os valores são enviados ao ESP32 ao longo das interações solicitadas no painel LCD.

Se a senha estiver correta, o motor da fechadura gira, e um LED verde acende, indicando que a porta foi desbloqueada. Se a senha estiver errada, o motor não gira, e um LED vermelho acende. Após três tentativas erradas, o sistema bloqueia por cinco minutos.

Para redefinir a senha, o usuário pode enviar o número 0. O ESP32 então solicita a senha atual e, se estiver correta, permite que uma nova senha seja definida, acendendo um LED verde como confirmação. A senha padrão para o primeiro uso é 1234, que deve ser passada para o modo de redefinição de senha.

Para simulação, foi utilizado o Wokwi.

<img width="779" height="594" alt="image" src="https://github.com/user-attachments/assets/bbacf53a-86fd-4cae-80a4-6accc144a8fa" />

*Ligação dos Componentes*

<img width="746" height="459" alt="image" src="https://github.com/user-attachments/assets/683fa937-5b06-4069-a03c-34aa049da03c" />

*Equipamento Funcionando*


### Link do projeto:
https://wokwi.com/projects/446642167249268737


