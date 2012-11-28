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

#define PKT_2_LEN	2
#define PKT_2_TYP	2

typedef struct {
	pkt_head_t head;
	uint8_t state;
} pkt_2_t;

#define PKT_AID0	0x01
#define PKT_AID1	0x02
#define PKT_AID2	0x04
#define PKT_AID3	0x08
#define PKT_AID4	0x10
#define PKT_AID5	0x20
#define PKT_AID6	0x40
#define PKT_AID7	0x80
