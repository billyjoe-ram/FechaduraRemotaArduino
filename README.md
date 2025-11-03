# Fechadura Remota para Arduino
Fechadura remota feita com arduíno por meio de módulo bluetooth

## Funcionamento
A fechadura, funcionando como um receptor bluetooth, vai receber uma senha numérica e, se correta, permitirá abertura da porta.

Participantes:
Billy Joe Santos
Gabriel Henrique Rocha

Componentes:
Placa ESP32
1 Stepper Motor
1 A4988 Stepper Motor Driver
2 LED, 1 verde e um vermelho
1 Protoboard
Cabos jumper
1 Tela LCD

Programas utilizados:
Arduino IDE (desenvolvimento e conexão com placa ESP32)
NRF Connect for Mobile (aplicativo de telefone utilizado para enviar os valores por bluetooth)

Descrição do projeto:
O projeto é uma fechadura inteligente que utiliza um ESP32 para receber comandos via Bluetooth. Um aplicativo se conecta ao ESP32 e envia uma senha de quatro dígitos.

Se a senha estiver correta, o motor da fechadura gira, e um LED verde acende, indicando que a porta foi desbloqueada. Se a senha estiver errada, o motor não gira, e um LED vermelho acende. Após três tentativas erradas, o sistema bloqueia por cinco minutos.

Para redefinir a senha, o usuário pode enviar o número 0. O ESP32 então solicita a senha atual e, se estiver correta, permite que uma nova senha seja definida, acendendo um LED verde como confirmação. A senha padrão para o primeiro uso é 1234, que ativa o modo de redefinição de senha.

Link do projeto:
https://wokwi.com/projects/446642167249268737


