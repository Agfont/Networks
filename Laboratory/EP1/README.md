# MAC0352 - Redes de  Computadores e Sistemas Distribuídos
## EP1 - Servidor MQTT
# Arthur Gouveia Font - 12036152
> Status: Finished

Este projeto implementa a interpretação e o processamento de algumas mensagens da camada de aplicação de um servidor MQTT.\
A implementação das camadas inferiores foi fornecida pelo Prof. Daniel Batista através de um servidor de eco disponibilizado no e-Disciplinas.

## Funcionalidades Implementadas:
+ Conexão de vários clientes simultaneamente (cada cliente simultâneo pode publicar ou requisitar mensagens do mesmo tópico ou de tópicos distintos);
+ Inscrição de cliente em um tópico e consequente envio das mensagens deste tópico para o cliente;
+ Publicação de mensagem em um tópico;
+ Desconexão de cliente.

## Mensagens MQTT Implementadas:
+ CONNECT
+ CONNACK
+ SUBSCRIBE
+ SUBACK
+ PUBLISH
+ PINGREQ
+ PINGRESP
+ DISCONNECT

## Tecnologias Utilizadas:

<table>
  <tr>
    <td>GCC Compiler</td>
    <td>Moquitto Clients (Subscriber and Publisher)</td>
    <td>Mosquitto Broker</td>
    <td>Wireshark</td>
  </tr>
  <tr>
    <td>9.3.0</td>
    <td>3.1.1</td>
    <td>1.6.9</td>
    <td>3.2.3</td>
  </tr>
</table>

## Compilação, Remoção e Inicialização:
Para compilar, basta excecutar o seguinte comando no shell:
```
$ make

```
Para limpar os arquivos de compilação, basta excecutar o seguinte comando no shell:
```
$ make clean
```
Para incializar o servidor MQTT, basta excecutar o seguinte comando no shell:
```
$ ./mqtt-server 1883
```
O programa recebe como argumento a porta em que desejamos que o servidor trabalhe.\
Por definição o protocolo MQTT utiliza a porta 1883.

## Testes:
Uma vez inicializado o servidor, para verificar sua correta execução, foram utilizados os clientes Mosquitto Subscriber e Publisher através dos seguintes comandos executados simultaneamente em diferentes terminais:

```
$ mosquitto_sub -t <tópico>
$ mosquitto_pub -t <tópico> -m <mensagem>
```
Onde:
+ **-t**: flag que indica o tópico em que se deseja publicar/inscrever
+ **-m**: flag que indica que há uma mensagem a ser publicada
+ **<tópico>**: campo destinado ao nome do tópico
+ **<**__mensagem__**>**: campo destinado a mensagem

Por exemplo:
```
$ mosquitto_sub -t "teste"
```
```
$ mosquitto_pub -t "teste" -m "Palmeiras"
```


