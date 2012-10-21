#define PKT_GRP		0xd4
#define PKT_NID		0x01

typedef struct {
	uint8_t grp;
	uint8_t hdr;
	uint8_t len;
	uint8_t typ;
} pkt_head_t;


#define PKT_1_LEN	10
#define PKT_1_TYP	1

typedef struct {
	pkt_head_t head;
	uint8_t sid; //local sensor id
	uint32_t count; //in mWh
	uint32_t msec;
} pkt_1_t;
