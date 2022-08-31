#include "../includes/main.h"
#include "../includes/uart.h"
#include "../includes/temperatures.h"
#include <signal.h>

#define WORKING 0

int timer = 0;

void listenCommands() {
    signal(SIGINT, finishProgram);

    while (1) {
        int command = requestKeyToUart(uart0_filestream, USER_COMM);

        if (command == 1) sendToUart(uart0_filestream, SYSTEM_STATE, 1);
        else if (command == 2) sendToUart(uart0_filestream, SYSTEM_STATE, 0);
        else if (command == 3) sendToUart(uart0_filestream, WORKING_STATE, 1);
        else if (command == 4) sendToUart(uart0_filestream, WORKING_STATE, 0);
        else if (command == 5) sendToUart(uart0_filestream, TIMER, ++timer);
        else if (command == 6) sendToUart(uart0_filestream, TIMER, --timer);
        // else menu

        delay(500);
    }
}

void startFrying() {
    signal(SIGINT, finishProgram);

    while (1) {
        if (WORKING) {
            float TI = getTemperatures(uart0_filestream, bme_connection);
            int pid = pid_controle(TI);
            sendToUart(uart0_filestream, CONTROL_SIGNAL, pid);
            manage_gpio_devices(pid);
        }
        else {
            manage_gpio_devices(0);
        }
        delay(500);
    }
}
