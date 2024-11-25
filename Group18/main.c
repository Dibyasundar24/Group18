#include <stdint.h>
#include "tm4c123gh6pm.h"

void UART0_Init(void);
void PWM0_Init(void);
char UART0_Read(void);
void UART0_WriteChar(char data);
void UART0_SendString(const char *str);
void UART0_SendInt(int num);
void PWM0_SetDutyCycle(uint8_t dutyCycle);

double value1=25;


int main(void) {
    UART0_Init(); // Initialize UART0
    PWM0_Init();  // Initialize PWM0

    UART0_SendString("UART and PWM Initialized\n"); // Startup message

    double value = 0;
        double Kc=2;
        double Ir=5;

        double ra=0.1;
        double Vs=100;
        double Vt;
        volatile double duty;



    while (1) {
        char input = UART0_Read(); // Read a character from UART0

        if (input == '+') {
            value = 2;

                   value1=value+value1;  //0 <value1<49.55
                   if(value1>49)
                   {
                       value1=49;
                   }

                   Vt=Kc*value1+Ir*ra;
                   duty=Vt/Vs;
            // Set value to 10 for '+'
            PWM0_SetDutyCycle(duty*100); // Set duty cycle to 50%
            UART0_SendString("speed :");
                       UART0_SendInt(value1);

            UART0_SendString(" Duty Cycle: ");
            UART0_SendInt(duty*100);  // Display value
            UART0_SendString(" % ");
        } else if (input == '-') {
            value = -2; // Set value to -10 for '-'
            value1=value+value1;  //0 <value1<49.55
            if(value1<0)
                      {
                      value1=0;
                              }
                              Vt=Kc*value1+Ir*ra;
                              duty=Vt/Vs;

            PWM0_SetDutyCycle(duty*100); // Set duty cycle to 25%
            UART0_SendString("speed : ");
                                   UART0_SendInt(value1);

                        UART0_SendString(" Duty Cycle: ");
                        UART0_SendInt(duty*100);  // Display value
                        UART0_SendString(" % ");
        }
    }
}

void UART0_Init(void) {
    SYSCTL_RCGCUART_R |= 0x01; // Enable UART0 clock
    SYSCTL_RCGCGPIO_R |= 0x01; // Enable Port A clock
    while ((SYSCTL_PRUART_R & 0x01) == 0); // Wait for UART0 to be ready
    while ((SYSCTL_PRGPIO_R & 0x01) == 0); // Wait for Port A to be ready

    GPIO_PORTA_AFSEL_R |= 0x03;  // Enable alternate functions on PA0, PA1
    GPIO_PORTA_PCTL_R |= 0x11;   // Configure PA0, PA1 as UART
    GPIO_PORTA_DEN_R |= 0x03;    // Digital enable PA0, PA1
    GPIO_PORTA_AMSEL_R &= ~0x03; // Disable analog functionality

    UART0_CTL_R &= ~0x01;        // Disable UART0 during setup
    UART0_IBRD_R = 104;          // Integer part of baud rate for 16 MHz
    UART0_FBRD_R = 11;           // Fractional part of baud rate for 16 MHz
    UART0_LCRH_R = 0x70;         // 8-bit, no parity, 1 stop bit
    UART0_CTL_R |= 0x301;        // Enable UART0, TXE, RXE
}

char UART0_Read(void) {
    while ((UART0_FR_R & 0x10) != 0); // Wait for RXFE bit to clear
    return (char)(UART0_DR_R & 0xFF);
}

void UART0_WriteChar(char data) {
    while ((UART0_FR_R & 0x20) != 0); // Wait for TXFF bit to clear
    UART0_DR_R = data;
}

void UART0_SendString(const char *str) {
    while (*str) {
        UART0_WriteChar(*(str++));
    }
}

void UART0_SendInt(int num) {
    char buffer[12]; // Buffer to hold the string representation of the number
    int i = 0, isNegative = 0;

    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    // Convert the number to a string (reversed order)
    do {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    if (isNegative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // Null-terminate the string

    // Reverse the string to correct the order
    int start = 0, end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start++] = buffer[end];
        buffer[end--] = temp;
    }

    UART0_SendString(buffer); // Send the string via UART
}

void PWM0_Init(void) {
    SYSCTL_RCGCPWM_R |= 0x01;   // Enable clock for PWM module 0
    SYSCTL_RCGCGPIO_R |= 0x02;  // Enable clock for Port B
    SYSCTL_RCC_R |= 0x00100000; // Use PWM divider (set to 64)

    GPIO_PORTB_AFSEL_R |= 0x40;  // Enable alternate function for PB6
    GPIO_PORTB_PCTL_R |= 0x04000000; // Configure PB6 as M0PWM0
    GPIO_PORTB_DEN_R |= 0x40;    // Enable digital functionality on PB6

    PWM0_0_CTL_R = 0;            // Disable PWM0 generator during setup
    PWM0_0_GENA_R = 0x0000008C;  // Set PWM signal to drive high on load, low on match
    PWM0_0_LOAD_R = 16000 - 1;   // Set load value for 1 kHz frequency (assuming 16 MHz clock)
    PWM0_0_CMPA_R = 0;           // Start with 0% duty cycle
    PWM0_0_CTL_R |= 0x01;        // Enable PWM0 generator
    PWM0_ENABLE_R |= 0x01;       // Enable PWM0 output on PB6
}

void PWM0_SetDutyCycle(uint8_t dutyCycle) {
    if (dutyCycle > 100) dutyCycle = 100; // Cap at 100%
    PWM0_0_CMPA_R = ((16000 * (100 - dutyCycle)) / 100) - 1; // Set compare value for duty cycle
}
