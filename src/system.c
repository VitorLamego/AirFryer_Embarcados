#include "../includes/main.h"
#include "../includes/uart.h"
#include "../includes/temperatures.h"
#include "../includes/control_lcd.h"
#include <signal.h>
#include <string.h>

int WORKING = 0;
int ON = 0;
int START_TIMER = 0;
int FINISHED_PROCESS = 0;
int COOL_DOWN = 0;

int timer = 0;
int indexMenu = 0;

typedef struct {
    char nome[10];
    float TR;
    int tempo;
} menu_item; 

menu_item menu_list[3];

void terminalMenu() {
    signal(SIGINT, finishProgram);
    populateMenuItems();

    while (1) {
        FINISHED_PROCESS = 0;
        int terminal_command = printMenu();
        clrLcd();
        if(ON == 1) printSystemOn();
        else printSystemOff();

        if (terminal_command == 1) {
            controlVariablesFromTerminal();
        } else if (terminal_command == 2) {
            printf("Controle o seu aquecimento pelo dashboard!\n");
            listenCommands();
        } else {
            printf("Alternativa inválida!\n\n");
        }
    }
    
}

void listenCommands() {
    
    int command = 0;

    while (!FINISHED_PROCESS) {
        command = requestKeyToUart(uart0_filestream, USER_COMM);
        controlCommand(command);
        command = 0;
        delay(500);
    }
}

void coolDownSystem() {
    float TE = getCurrentTemperature(bme_connection);
    float TI = getTemperatures(uart0_filestream, bme_connection);

    while (TI > TE - 5) {
        if (TE <= 0) TE = getCurrentTemperature(bme_connection);
        TI = getTemperatures(uart0_filestream, bme_connection);
        if (TI > 0) {
            pid_atualiza_referencia(TE);
            int pid = pid_controle(TI);
            sendIntToUart(uart0_filestream, CONTROL_SIGNAL, pid);
            manage_gpio_devices(pid);
        }
    }
}

void startFrying() {
    clrLcd();
    while (WORKING) {
        float TI = getTemperatures(uart0_filestream, bme_connection);
        if (TI > 0) {
            int pid = pid_controle(TI);
            sendIntToUart(uart0_filestream, CONTROL_SIGNAL, pid);
            manage_gpio_devices(pid);
        }
        int command = requestKeyToUart(uart0_filestream, USER_COMM);
        if (command != 3) controlCommand(command);
    }
    printFryingFinished();
    COOL_DOWN = 1;
    printf("Resfriando sistema! Aguarde.\n");
    coolDownSystem();
    COOL_DOWN = 0;
    delay(1000);
}

void controlCommand(int command) {
    if (command == 1) {
        ON = 1;
        sendByteToUart(uart0_filestream, SYSTEM_STATE, 1);
        clrLcd();
        printSystemOn();
        sendByteToUart(uart0_filestream, WORKING_STATE, 0);
    }
    else if (command == 2) {
        ON = 0;
        sendByteToUart(uart0_filestream, SYSTEM_STATE, 0);
        printSystemOff();
    }
    else if (command == 3 && !WORKING) {
        sendByteToUart(uart0_filestream, WORKING_STATE, 1); 
        WORKING = 1;
        clrLcd();
        startFrying();
    }
    else if (command == 4 && ON) {
        sendByteToUart(uart0_filestream, WORKING_STATE, 0); 
        WORKING = 0;
        FINISHED_PROCESS = 1;
    }
    else if (command == 5 && ON) {
        sendIntToUart(uart0_filestream, TIMER, ++timer);
    }
    else if (command == 6 && ON) {
        sendIntToUart(uart0_filestream, TIMER, --timer);
    }
    else if (command == 7 && ON) {
        printMenuItem(menu_list[indexMenu].nome, (int) menu_list[indexMenu].TR, menu_list[indexMenu].tempo);
        sendIntToUart(uart0_filestream, TIMER, menu_list[indexMenu].tempo);
        sendFloatToUart(uart0_filestream, REFERENCE_SIGNAL, menu_list[indexMenu].TR);
        timer = menu_list[indexMenu].tempo;
        if (indexMenu == 2) indexMenu = 0;
        else indexMenu++;
    }
}

void controlTimer() {
    signal(SIGINT, finishProgram);

    while (1) {
        while (START_TIMER && timer > 0) {
        delay(60000);
        timer--;
        sendIntToUart(uart0_filestream, TIMER, timer);
        if (timer == 0) {
            START_TIMER = 0;
            sendByteToUart(uart0_filestream, WORKING_STATE, 0); 
            WORKING = 0;
            FINISHED_PROCESS = 1;
        }
    }
    }
}

int printMenu() {
    int terminal_command;

    printf("========== AIR FRYER ===========\n\n");
    printf("1. Enviar temperatura e tempo pelo terminal\n");
    printf("2. Configurar pelo dashboard\n");
    printf("Ps: Para utilizar o menu pre configurado escolha a opcao 2\n");
    printf("Digite a sua opcao: ");
    scanf(" %d", &terminal_command);

    return terminal_command;
}

void controlVariablesFromTerminal() {
    float temperatura;

    printf("Insira a temperatura que deseja configurar (30 - 90): ");
    scanf(" %f", &temperatura);
    while(temperatura < 30 || temperatura > 90) {
        printf("Temperatura inválida. Digite novamente\n");
        scanf(" %f", &temperatura);
    }
    printf("Insira o tempo que deseja configurar (1 - 15): ");
    scanf(" %d", &timer);
    while (timer < 1 || timer > 10) {
        printf("Tempo inválido. Digite novamente: \n");
        scanf(" %d", &timer);
    }

    ON = 1;
    printf("Ligando sistema automaticamente!\n");
    clrLcd();
    printSystemOn();
    sendByteToUart(uart0_filestream, SYSTEM_STATE, 1);
    sendFloatToUart(uart0_filestream, REFERENCE_SIGNAL, temperatura);
    sendIntToUart(uart0_filestream, TIMER, timer);
    printf("Iniciando aquecimento!\n");
    sendByteToUart(uart0_filestream, WORKING_STATE, 1); 
    WORKING = 1; 
    startFrying();
}

void populateMenuItems() {
    strcpy(menu_list[0].nome, "Frango");
    menu_list[0].TR = 35.0f;
    menu_list[0].tempo = 5;
    strcpy(menu_list[1].nome, "Peixe");
    menu_list[1].TR = 45.0f;
    menu_list[1].tempo = 7;
    strcpy(menu_list[2].nome, "Batata");
    menu_list[2].TR = 50.0f;
    menu_list[2].tempo = 2;
}
