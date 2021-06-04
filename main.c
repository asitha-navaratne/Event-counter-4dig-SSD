#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define NUMERAL_DDR		DDRB					///< DDR for controlling individual segments.
#define CONTROL_DDR 		DDRC					///< DDR for controlling individual digits.
#define NUMERAL_PORT 		PORTB					///< Port for controlling individual segments.
#define CONTROL_PORT 		PORTC					///< Port for controlling individual digits.
#define DIGIT1_PIN		PD0					///< Pin controlling first digit of SSD.
#define DIGIT2_PIN		PD1					///< Pin controlling second digit of SSD.
#define DIGIT3_PIN		PD2					///< Pin controlling third digit of SSD.
#define DIGIT4_PIN		PD3					///< Pin controlling fourth digit of SSD.

void PORT_INIT(void);
void INT_INIT(void);
void DECODE_SSD(uint16_t value);
void DISPLAY_SSD(void);

uint8_t dig[4];

volatile uint16_t event = 0;

/*!
 *	@brief ISR for INT0 external interrupt; increments event at every rising edge at INT0. 
 */

ISR(INT0_vect){
	event++;
	if(event > 9999){
		event = 0;						///< Reset value once it cannot be displayed on the four-digit SSD.
	}
}

int main(void){
	PORT_INIT();
	INT_INIT();
	
	sei();								///< Enable global interrupts.
	
	while(1){
		DECODE_SSD(event);
		DISPLAY_SSD();
	}
}

/*!
 *	@brief Initialize Ports.
 */

void PORT_INIT(void){
	NUMERAL_DDR = 0xFF;
	CONTROL_DDR |= (1<<DIGIT1_PIN)|(1<<DIGIT2_PIN)|(1<<DIGIT3_PIN)|(1<<DIGIT4_PIN);
	DDRD &= ~(1<<PD2);						///< Set INT0 pin to input.
}

/*!
 *	@brief Initialize External Interrupt 0 at INT0.
 */

void INT_INIT(void){
	MCUCR |= (1<<ISC00)|(1<<ISC01);					///< A rising edge at INT0 generates an interrupt request.
	GICR |= (1<<INT0);						///< Enable INT0 external interrupt request.
}

/*!
 *	@brief Decode the value into individual digits.
 *	@param Value to be decoded (uint16_t).
 */

void DECODE_SSD(uint16_t value){
	dig[0] = (value/1000);						///< Save first digit to array.
	dig[1] = (value%1000)/100;					///< Save second digit to array.
	dig[2] = (value%100)/10;					///< Save third digit to array.
	dig[3] = (value%10);						///< Save fourth digit to array.
}

/*!
 *	@brief Display each individual digit on the SSD.
 */

void DISPLAY_SSD(void){
	
	uint8_t ssd[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};		///< Array to hold values of segments corresponding to each digit.

	NUMERAL_PORT = ssd[dig[0]];					///< Display first digit momentarily.
	CONTROL_PORT |= (1<<DIGIT1_PIN);				///< Turn on display of first digit place.
	_delay_ms(1);
	CONTROL_PORT &= ~(1<<DIGIT1_PIN);				///< Turn off display of first digit place.
	
	NUMERAL_PORT = ssd[dig[1]];					///< Display second digit momentarily.
	CONTROL_PORT |= (1<<DIGIT2_PIN);				///< Turn on display of second digit place.
	_delay_ms(1);
	CONTROL_PORT &= ~(1<<DIGIT2_PIN);				///< Turn off display of second digit place.
	
	NUMERAL_PORT = ssd[dig[2]];					///< Display third digit momentarily.
	CONTROL_PORT |= (1<<DIGIT3_PIN);				///< Turn on display of third digit place.
	_delay_ms(1);
	CONTROL_PORT &= ~(1<<DIGIT3_PIN);				///< Turn off display of third digit place.
	
	NUMERAL_PORT = ssd[dig[3]];					///< Display fourth digit momentarily.
	CONTROL_PORT |= (1<<DIGIT4_PIN);				///< Turn on display of fourth digit place.
	_delay_ms(1);
	CONTROL_PORT &= ~(1<<DIGIT4_PIN);				///< Turn off display of fourth digit place.
}
