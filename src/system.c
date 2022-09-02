#include "../includes/main.h"
#include "../includes/uart.h"
#include "../includes/temperatures.h"
#include <signal.h>

int WORKING = 0;
int ON = 1;
int START_TIMER = 0;

int timer = 0;

void listenCommands() {
    signal(SIGINT, finishProgram);
    int command = 0;

    while (1) {
        command = requestKeyToUart(uart0_filestream, USER_COMM);

        printf("Inteiro recebido: %d\n", command);
        controlCommand(command);
        command = 0;
        delay(500);
    }
}

void startFrying() {
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
 
    delay(1000);
}

void controlCommand(int command) {
    printf("Inteiro checado: %d\n", command);
    if (command == 1) {
        printf("Entrei no envio de ON\n");
        ON = 1;
        // sendByteToUart(uart0_filestream, SYSTEM_STATE, 1); 
        printSystemOn();
    }
    else if (command == 2) {
        ON = 0;
        sendByteToUart(uart0_filestream, SYSTEM_STATE, 0);
        printSystemOff();
    }
    else if (command == 3 && !WORKING) {
        sendByteToUart(uart0_filestream, WORKING_STATE, 1); 
        WORKING = 1; 
        startFrying();
    }
    else if (command == 4 && ON) {
        sendByteToUart(uart0_filestream, WORKING_STATE, 0); 
        WORKING = 0;
    }
    else if (command == 5 && ON) {
        sendIntToUart(uart0_filestream, TIMER, ++timer);
    }
    else if (command == 6 && ON) {
        sendIntToUart(uart0_filestream, TIMER, --timer);
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
            printFryingFinished();
        }
    }
    }
}
