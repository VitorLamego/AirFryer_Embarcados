#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../includes/crc.h"

int initUart(){
    int uart_filestream = -1;
    char uart_path[] = "/dev/serial0";
    uart_filestream = open(uart_path, O_RDWR | O_NOCTTY | O_NDELAY);
    if(uart_filestream == -1){
        printf("Não foi possível iniciar a Uart.\n");
    }
    else {
        printf("UART inicializado.\n");
    }
    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH);
    tcsetattr(uart_filestream, TCSANOW, &options);
    return uart_filestream;
}

int readIntFromUart(int uart_filestream){
    unsigned char buffer[20];
    int number = -1;

    int content = read(uart_filestream, buffer, 20);
    if(!content){
        printf("Nenhum dado foi recebido\n");
    }
    else if(content < 0){
        printf("Erro ao ler dados\n");
    }
    else {
        buffer[content] = '\0';
        memcpy(&number, &buffer[3], sizeof(int));
        return number;
    }
    return number; 
}

float readFloatFromUart(int uart_filestream) {
    unsigned char buffer[20];
    float number = -1.0;

    int content = read(uart_filestream, buffer, 20);
    if(!content){
        printf("Nenhum dado foi recebido\n");
    }
    else if(content < 0){
        printf("Erro ao ler dados\n");
    }
    else {
        buffer[content] = '\0';
        memcpy(&number, &buffer[3], sizeof(float));
        return number;
    }
    return number; 
}

float requestTemperatureToUart(int uart_filestream, int code){
    unsigned char package[7] = {0x01, 0x23, code, 0x00, 0x09, 0x00, 0x03};
    short crc = calcula_CRC(package, 7);

    unsigned char message[9];
    memcpy(message, &package, 7);
    memcpy(&message[7], &crc, 2);
    int check = write(uart_filestream, &message[0], 9);
    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
    sleep(1);
    
    float result = readFloatFromUart(uart_filestream);
    return result;
}

int requestKeyToUart(int uart_filestream, unsigned char code){
    unsigned char package[7] = {0x01, 0x23, code, 0x00, 0x09, 0x00, 0x03};
    short crc = calcula_CRC(package, 7);

    unsigned char message[9];
    memcpy(message, &package, 7);
    memcpy(&message[7], &crc, 2);
    int check = write(uart_filestream, &message[0], 9);
    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
    sleep(1);
    
    int result = readIntFromUart(uart_filestream);
    return result;
}

void sendIntToUart(int uart_filestream, int code, int value) {
    unsigned char package[7] = {0x01, 0x16, code, 0x00, 0x09, 0x00, 0x03};
    unsigned char message[13];

    memcpy(message, &package, 7);
    memcpy(&message[7], &value, 4);

    short crc = calcula_CRC(message, 11);

    memcpy(&message[11], &crc, 2);

    int check = write(uart_filestream, &message[0], 13);

    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
    sleep(1);
}

void sendFloatToUart(int uart_filestream, int code, float value) {
    unsigned char package[7] = {0x01, 0x16, code, 0x00, 0x09, 0x00, 0x03};
    unsigned char message[13];

    memcpy(message, &package, 7);
    memcpy(&message[7], &value, 4);

    short crc = calcula_CRC(message, 11);

    memcpy(&message[11], &crc, 2);

    int check = write(uart_filestream, &message[0], 13);

    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
    sleep(1);
}

void sendByteToUart(int uart_filestream, int code, char value) {
    unsigned char package[7] = {0x01, 0x16, code, 0x00, 0x09, 0x00, 0x03};
    unsigned char message[10];

    memcpy(message, &package, 7);
    memcpy(&message[7], &value, 1);

    short crc = calcula_CRC(message, 8);

    memcpy(&message[8], &crc, 2);

    int check = write(uart_filestream, &message[0], 10);

    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
   sleep(0.5);
}