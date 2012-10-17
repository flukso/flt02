#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>

#include "rfm12.h"

int main(void)
{
	cli();
	_delay_ms(10);

	rfm12_init();
	// the clk/8 fuse bit is set
	clock_prescale_set(clock_div_1);
	sei();

	while(1) {
		rfm12_tick();
	}
}
