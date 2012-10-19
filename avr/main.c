#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>

#include "rfm12.h"
#include "pkt.h"
#include "dbg.h"

static pkt_1_t pkt_counter = {
	.head = {
		.grp = PKT_GRP, //hardcoded for the time being
		.hdr = PKT_NID, //CTL=0, DST=0, ACK=0 -> b'cast noack
		.len = PKT_1_LEN,
		.typ = PKT_1_TYP
	},

	.sid = 0,
	.count = 0,
	.msec = 0
};

static int send(uint8_t *pkt, uint8_t size)
{
	uint8_t i, ret;

	//we cannot busy loop here since this might cause
	//a deadlock with rfm12_tick in the main loop
	//the compiler seems to be quite fond of all these parentheses
	if ((ret = rfm12_tx_occupy()))
		return ret;

	for (i = 0; i < size; i++) {
		rfm12_tx_buffer_add(pkt[i]);
	}

	rfm12_tx_start();

	return 0;
}

static inline void timer0_init(void)
{
	//timer1 prescaler set to 64 giving us a base freq of 125kHz
	TCCR0B |= (1<<CS01) | (1<<CS00);
	//decrease timer freq to 1kHz
	OCR0A = 0xf9;
	//set timer0 to CTC mode
	TCCR0A |= 1<<WGM01;
	//enable output compare match A interrupt
	TIMSK0 |= 1<<OCIE0A;

	DBG_OC0A_TOGGLE();
}

ISR(TIMER0_COMPA_vect)
{
}

int main(void)
{
	cli();
	_delay_ms(10);

	timer0_init();
	rfm12_init();

	// the clk/8 fuse bit is set
	clock_prescale_set(clock_div_1);
	sei();

	send((uint8_t *) &pkt_counter, sizeof(pkt_counter));

	while (1) {
		rfm12_tick();
	}
}
