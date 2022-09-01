#include "../includes/main.h"
#include "../includes/uart.h"
#include "../includes/temperatures.h"
#include <signal.h>

int WORKING = 0;

int timer = 0;

void listenCommands() {
    signal(SIGINT, finishProgram);

    while (1) {
        int command = requestKeyToUart(uart0_filestream, USER_COMM);

        if (command == 1) {sendByteToUart(uart0_filestream, SYSTEM_STATE, 1);printSystemOn();}
        else if (command == 2) sendByteToUart(uart0_filestream, SYSTEM_STATE, 0);
        else if (command == 3) {sendByteToUart(uart0_filestream, WORKING_STATE, 1); WORKING = 1;}
        else if (command == 4) sendByteToUart(uart0_filestream, WORKING_STATE, 0);
        else if (command == 5) sendIntToUart(uart0_filestream, TIMER, ++timer);
        else if (command == 6) sendIntToUart(uart0_filestream, TIMER, --timer);
        // else menu

        delay(5000);
    }
}

void startFrying() {
    signal(SIGINT, finishProgram);

    while (1) {
        if (1) {
            float TI = getTemperatures(uart0_filestream, bme_connection);
            if (TI > 0) {
                int pid = pid_controle(TI);
                sendIntToUart(uart0_filestream, CONTROL_SIGNAL, pid);
                manage_gpio_devices(pid);
            }
        }
        else {
            manage_gpio_devices(0);
        }
        delay(1000);
    }
}
