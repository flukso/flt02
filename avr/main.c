#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>

#include "rfm12.h"
#include "pkt.h"

static pkt_1_t pkt_counter = {
	.head = {
		.grp = PKT_GRP, //hardcoded for the time being
		.hdr = PKT_NID, //CTL=0, DST=0, ACK=0 -> b'cast noack
		.len = PKT_1_LEN,
		.typ = PKT_1_TYP
	},

	.sid = 1,
	.count = 2,
	.msec = 3
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

int main(void)
{
	cli();
	_delay_ms(10);

	rfm12_init();
	// the clk/8 fuse bit is set
	clock_prescale_set(clock_div_1);
	sei();

	send((uint8_t *) &pkt_counter, sizeof(pkt_counter));

	while (1) {
		rfm12_tick();
	}
}
