#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>

#include "rfm12.h"
#include "pkt.h"
#include "dbg.h"

#define METERCONST 80 //mWh/pulse
#define SECOND 1000 //msec

static uint32_t msec;
static bool pkt_counter_send = false;

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
	msec++;
}

static inline void s0_init(void)
{
	//s0 maps to PC2
	//set as input pin with internal pull-up
	PORTC |= (1<<PC2);
	//enable pin change interrupt on PC2 = PCINT10
	PCMSK1 |= (1<<PCINT10);
	//enable pin change interrupt 1
	PCICR |= (1<<PCIE1);
}

static inline uint32_t diff(uint32_t new, uint32_t old)
{
	return new < old ? (0xFFFFFFFF - old) + new + 1 : new - old;
}
 
ISR(PCINT1_vect)
{
	//we're only counting when the pin is pulled low
	if (PINC & (1<<PINC2))
		return;

	if (diff(msec, pkt_counter.msec) >= SECOND) {
		pkt_counter_send = true;
	}

	pkt_counter.count += METERCONST;
	pkt_counter.msec = msec;
}

int main(void)
{
	cli();
	_delay_ms(10);

	rfm12_init();
	timer0_init();
	s0_init();

	// the clk/8 fuse bit is set
	clock_prescale_set(clock_div_1);
	sei();

	while (1) {
		if (pkt_counter_send && !send((uint8_t *) &pkt_counter, sizeof(pkt_counter))) {
			//clear when pkt is in tx buffer
			//if not, the tx buffer is still occupied, so try again later
			pkt_counter_send = false;
		}

		rfm12_tick();
	}
}
