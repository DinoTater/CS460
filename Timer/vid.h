#define VDC_INDEX		0x3D4
#define VDC_DATA		0x3D5
#define CUR_SIZE		10
#define VID_ORG			12
#define CURSOR 			14
#define LINE_WIDTH		80
#define SCR_LINES		25
#define SCR_BYTES		4000
#define CURSOR_SHAPE	15

u16 base = 0xB800;
u16 vid_mask = 0x3FFF;
u16 offset;
int color;
int org;
int row, column;
