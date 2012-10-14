#include <avr/interrupt.h>
#include "rfm12.h"

int main(void)
{
	cli();
	rfm12_init();
	sei();

	while(1) {
		rfm12_tick();
	}
}
