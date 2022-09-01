#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>

#include "../includes/uart.h"
#include "../includes/bme280.h"
#include "../includes/control_lcd.h"
#include "../includes/temperatures.h"
#include "../includes/pid.h"
#include "../includes/gpio.h"
#include "../includes/system.h"

struct bme280_dev bme_connection;
int uart0_filestream;

void finishProgram();
void initSystem();

int main(int argc, const char * argv[]) {

    // INICIALIZACAO
    uart0_filestream = initUart();
    setup_gpio();
    lcd_init();
    bme_connection = connectBme();

    initSystem();
    // THREADS
    pthread_t tid[2];

    // while(1){float TI = requestTemperatureToUart(uart0_filestream, TEMP_INT);
    // float TR = requestTemperatureToUart(uart0_filestream, TEMP_REF);

    // printf("Temperatura Interna: %.2f\n", TI);
    // printf("Temperatura Referencia: %.2f\n", TR);}
    
    pthread_create(&tid[0], NULL, (void *)listenCommands, (void *)NULL); // Escuta os comandos via UART
    pthread_create(&tid[1], NULL, (void *)startFrying, (void *)NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
        
    // while (1) {requestToUart(uart0_filestream, USER_COMM); sleep(0.5);}

    // sendToUart(uart0_filestream, REFERENCE_SIGNAL, 3.0);
    // requestToUart(uart0_filestream, TEMP_INT, 0);
    
    // ENCERRAMENTO
   return 0;
}

void initSystem() {
    unsigned char buffer[100];
    read(uart0_filestream, buffer, 100);
    sendToUart(uart0_filestream, CONTROL_SIGNAL, 0);
    sendToUart(uart0_filestream, SYSTEM_STATE, 0);
    sendToUart(uart0_filestream, WORKING_STATE, 0);
    sendToUart(uart0_filestream, TIMER, 0);
}

void finishProgram() {
    disable_fan_and_resistor();
    clrLcd();
    close(uart0_filestream);
    exit(0);
}
