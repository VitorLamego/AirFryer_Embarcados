#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "../includes/uart.h"
#include "../includes/bme280.h"
#include "../includes/control_lcd.h"
#include "../includes/temperatures.h"

struct bme280_dev bme_connection;
int uart0_filestream;

int main(int argc, const char * argv[]) {

    // INICIALIZACAO
    uart0_filestream = initUart();
    lcd_init();
    bme_connection = connectBme();
    
    // bme_connection = connectBme();
    
    // while (1) {requestToUart(uart0_filestream, USER_COMM); sleep(0.5);}
    getTemperatures(uart0_filestream, bme_connection);
    // sendToUart(uart0_filestream, REFERENCE_SIGNAL, 3.0);
    // requestToUart(uart0_filestream, TEMP_INT, 0);
    
    // ENCERRAMENTO
    close(uart0_filestream);
   return 0;
}