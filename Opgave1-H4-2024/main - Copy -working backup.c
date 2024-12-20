/*
 * AdaFruit_Display_C.c
 *
 * Created: 23-06-2023 00:18:42
 * Author : ltpe
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "Lib/ProjectDefines.h"
#include "Lib/Display_Lib/ssd1306.h"
#include "Lib/UART_Lib/UART.h"

//static volatile char ReceivedCharacterFromUART;
static volatile bool CharacterReceivedFromUART = false;
static volatile uint8_t DisplayLineCounter = 0;
static volatile uint8_t I2C_Address = SSD1306_ADDR;

static volatile char DisplayBuffer[DisplayBufferSize];
static uint8_t BufferHead = 0;
static uint8_t BufferTail = 0;
static uint16_t BufferOverrunCounter = 0;
static volatile bool SkippedCharacterIndicator = false;
static volatile char SkippedCharacter;

typedef enum {
	Modtag_Adresse,
	Modtag_BitPosition,
	Modtag_BitVaerdi
} StatePortHandler;

void ReceiceCharacterFromUart(char ReceivedCharacter)
{
	//ReceivedCharacterFromUART = ReceivedCharacter;
	
	if ((BufferHead + 1) % DisplayBufferSize == BufferTail) 
	{
		// Buffer is full, discard the oldest character
		SkippedCharacter = DisplayBuffer[BufferTail];
		SkippedCharacterIndicator = true;
		BufferOverrunCounter++;
		BufferTail = (BufferTail + 1) % DisplayBufferSize;
	}
	DisplayBuffer[BufferHead] = ReceivedCharacter;
	BufferHead = (BufferHead + 1) % DisplayBufferSize;
	CharacterReceivedFromUART = true;
}

void WriteReceivedCharacterFromUARTInDisplay()
{
	char ReceivedCharacterFromUARTString[2];
	
	while (BufferTail != BufferHead) {
		SSD1306_SetPosition(0, DisplayLineCounter++);
		SSD1306_DrawString("character : ", NORMAL);
		
		sprintf(ReceivedCharacterFromUARTString, "%c", DisplayBuffer[BufferTail]);
		SSD1306_DrawString(ReceivedCharacterFromUARTString, BOLD);
		  // Increment the cursor position for the next line

		// Move the tail pointer to the next character
		BufferTail = (BufferTail + 1) % DisplayBufferSize;
		DisplayLineCounter = DisplayLineCounter % MAX_NUMBER_OF_LINES_IN_DISPLAY;
	}
}

void WriteStringWhenReceivedCharacterFromUARTInDisplay(const char text[])
{
	SSD1306_SetPosition(0, DisplayLineCounter++);
	SSD1306_DrawString("State : ", NORMAL);
	
	SSD1306_DrawString(text, NORMAL);
	DisplayLineCounter = DisplayLineCounter % MAX_NUMBER_OF_LINES_IN_DISPLAY;	
}

char ReadCharacterFromBuffer()
{
	// Check if the buffer is empty
	if (BufferTail == BufferHead) {
		return '\0'; // Return a null character if the buffer is empty
	}

	// Read the character from the buffer
	char receivedChar = DisplayBuffer[BufferTail];

	// Move the tail pointer to the next character in a circular manner
	BufferTail = (BufferTail + 1) % DisplayBufferSize;

	// Return the character read from the buffer
	return receivedChar;
}

char temp[16] = "";   // Buffer for accumulating received characters
int8_t tempIndex = 0; // Index to track the next position in the buffer

void ResetTempBuffer() {
	tempIndex = 0;
	temp[0] = '\0'; // Reset the string
}

bool IsHexString(const char *str) {
	// Check if the string contains only hexadecimal characters
	while (*str) {
		if (!( (*str >= '0' && *str <= '9') ||
		(*str >= 'A' && *str <= 'F') ||
		(*str >= 'a' && *str <= 'f') )) {
			return false;
		}
		str++;
	}
	return true;
}

int main(void)
{
	StatePortHandler tilstand = Modtag_Adresse;
	
	SetupFunctionCallbackPointer(ReceiceCharacterFromUart);
	RS232Init();
	Enable_UART_Receive_Interrupt();
	
	// Enable global interrupt
	sei();

	// init ssd1306
	SSD1306_Init(I2C_Address);

	// clear screen
	SSD1306_ClearScreen();
	
	while (1)
	{
		if (CharacterReceivedFromUART)
		{
			CharacterReceivedFromUART = false;
			
			char receivedChar = ReadCharacterFromBuffer();
			
			if (receivedChar == ':') {
				// Evaluate temp as hexadecimal when ':' is received
				if (IsHexString(temp)) {
					printf("Hex value received: %s\n", temp);
					// Pass the hex value for further processing
					WriteStringWhenReceivedCharacterFromUARTInDisplay(temp);
					} 
				else {
					printf("Invalid hex string: %s\n", temp);
				}
				// Reset the buffer for the next input
				ResetTempBuffer();
			} 
			else {
				// Accumulate characters in temp
				if (tempIndex < sizeof(temp) - 1) { // Ensure no overflow
					temp[tempIndex++] = receivedChar;
					temp[tempIndex] = '\0'; // Null-terminate the string
					} else {
					printf("Temp buffer overflow! Resetting buffer.\n");
					ResetTempBuffer();
				}
			}
		}
		
		if (SkippedCharacterIndicator)
		{
			SkippedCharacterIndicator = false;
			printf("\nSkipped character from Uart %c\n", SkippedCharacter);
			printf("Number of Skipped Characters : %d\n", BufferOverrunCounter);
		}
	}
}

