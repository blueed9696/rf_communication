#include <stdio.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>

int uart_fd;
const char *rx = "434.9000";   // Receive frequency
const char *tx = "436.0000";   // Transmit frequency
int serial_port = 115200;
int squelch = 3;

void send_ctrl_z(int uart_fd){
	serialPutchar(uart_fd, 26);
	printf("Ctrl+Z sent.\n");
}

void program_radio() {
	printf("Programming FM module!\n");

	pinMode(28, OUTPUT);
	pinMode(29, OUTPUT);
	digitalWrite(29,HIGH);  // Enable SR_FRS
	digitalWrite(28, LOW);   // LOW: transmit mode

	// Open serial communication with the radio module
	if ((uart_fd = serialOpen("/dev/ttyAMA0", serial_port)) >= 0) {
		printf("Serial opened at %d baud\n", serial_port);

		for (int i = 0; i < 5; i++) {
			sleep(1);  // Wait for 1 second

			char status_command[] = "AT+DMOCONT\r\n";  // Basic test command

			serialPuts(uart_fd, status_command);
			send_ctrl_z(uart_fd);

			// Read the response from the module to verify it accepted the command
			sleep(1);  // Delay to ensure response time
			printf("Module Response: ");

			while (serialDataAvail(uart_fd)) {
				char response = serialGetchar(uart_fd);
				printf("%c", response);  // Print the module's response
				fflush(stdout);          // Flush the output buffer
			}
			printf("\n");

			sleep(1);

			// Uncomment to send the frequency setting command
/*
			char command[100];
			snprintf(command, sizeof(command), "AT+DMOSETGROUP=0,%s,%s,0,%d,0,0\r\n", rx, tx, squelch);

			serialPuts(uart_fd, command);
			printf("Sending command: %s\n", command);

			sleep(2);  // Wait for 2 seconds for the module to process
			printf("Module Response: ");

			while (serialDataAvail(uart_fd)) {
			char response = serialGetchar(uart_fd);
			printf("%c", response);  // Print the module's response
			fflush(stdout);          // Flush the output buffer
			}
			printf("\n");
*/


	}

        printf("Programming FM tx %s, rx on %s MHz\n", tx, rx);
        digitalWrite(29, LOW);  // Disable SR_FRS
        pinMode(28, INPUT);
        pinMode(29, INPUT);

        serialClose(uart_fd);  // Close the serial port
    } else {
        printf("Failed to open serial port\n");
    }
}

int main() {
    wiringPiSetup();  // Initialize wiringPi
    program_radio();  // Call the radio programming function
    return 0;
}
