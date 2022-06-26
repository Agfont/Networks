## MAC0352 - Redes de  Computadores e Sistemas Distribuídos
### EP3 - Controladores SDN (Software Defined Networking)
#### Arthur Font Gouveia - 12036152

### Regras do firewall
As regras do firewall foram definidas através de um arquivo JSON (rules.json) e podem ser alteradas. Este formato foi escolhido porque facilita a integração com o python.
É importante notar que um fluxo é barrado pelo firewall somente caso todos os campos de uma ou mais regras coincidem. 

### Parâmetros das regras
+ Endereço IP fonte
+ Endereço IP destino
+ Porta (Da camada de transporte)
+ Protocolo (TCP ou UDP)

### Formato do arquivo JSON
O arquivo deve possuir o seguinte padrão, sendo opcional a(s) escolha(s) do(s) parâmetro(s) desejado(s):  
Nome: rules.json  
Localização: /home/mininet/pox  
Conteúdo:
```
{
    "0" : {
        "src_ip": <Endereço IP fonte>,
        "dst_ip": <Endereço IP destino>,
        "port": <Porta>,
        "protocol": <"TCP" ou "UDP">
    }
    ...
}
```
Exemplo:
```
{
    "0" : {
        "src_ip": "10.0.0.1",
        "dst_ip": "10.0.0.2"
}
```
Um parâmetro aceita apenas um valor (IP, porta ou protocolo). Caso deseja-se bloquear mais de um valor, deve-se adicionar uma nova regra.

### Adicionando novas regras
Para adicionar novas regras, basta criar um novo índice e definir os parâmetros desejados.
Exemplo:
```
{
    "0" : {
        "src_ip": "10.0.0.1",
        "dst_ip": "10.0.0.2"
    }
    "1" : {
        "protocol" : "UDP"
    }
}
```

### Tecnologias Utilizadas:

<table>
  <tr>
    <td>Python</td>
    <td>OpenFlow</td>
    <td>OpenSSH</td>
    <td>VirtualBox</td>
    <td>Wireshark</td>
  </tr>
</table>
