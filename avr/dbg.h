#if DBG > 0
	//set PD6=OC0A as output pin and toggle compare match
	#define DBG_OC0A_TOGGLE()	DDRD |= 1<<DDB6; \
								TCCR0A |= 1<<COM0A0
#else
	#define DBG_OC0A_TOGGLE()	//nothing
#endif
