## O programa

Programa referente ao Trabalho 2 da disciplina de Sistemas Embarcos, referente ao desenvolvimento de um programa para controle de aparelhos "Air Fryer".

## Como executar

Após clonar o repositório, acesse a pasta do projeto, e no local onde tem o arquivo "Makefile" rode os seguintes comandos:

```
cd AirFryer_Embarcados
make
bin/run
```

## Testes Exemplificados

```
Teste 1: Realização de aquecimento a partir do controle pelo dashboard
Temperatura: 35
Tempo: 2 minutos
```

```
Teste 2: Realização de aquecimento a partir do controle pelo terminal
Temperatura: 40
Tempo: 3 minutos
```

Teste 3: Realização de aquecimento a partir de menu pré definido


## Observação

IMPORTANTE: O programa só funciona até o resfriamento na placa Raspberry 43, ao solicitar a temperatura do ambiente a placa 43 tem dado Segmentation Fault no programa.

O programa apresentou determinado bug no momento de enviar o estado do sistema como LIGADO, por conta disso é necessário que apareça no display "SISTEMA LIGADO" antes que qualquer outro comando seja lido pela comunicação UART.


