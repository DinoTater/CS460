#include "vid.h"

int vid_init() // initializes org=0 (row, column)=(0,0)
{
	int i,w;
	org = row = column = 0;
	color = 0x0A;
	set_VDC(CUR_SIZE, CURSOR_SHAPE);
	set_VDC(VID_ORG, 0);
	set_VDC(CURSOR, 0);
	w = 0x0700;							// clear screen with white
	for (i = 0; i<25*80; i++)
		put_word(w, base, 0+2*i);
}

int scroll() //scroll UP one line
{
	u16 i, w, bytes;
	//test offset = org + ONE screen + ONE more line
	offset = org + SCR_BYTES + 2*LINE_WIDTH;
	if (offset <= vid_mask)
		org += 2*LINE_WIDTH;
	else
	{
		//offset exceeds VRAM area ==> reset to VRAM beginning by
		// copy current rows 1-24 to BASE, then reset org to 0
		for (i=0; i<24*80; i++)
		{
			w = get_word(base, org+160+2*i);
			put_word(w, base, 0+2*i);
		}
		org = 0;
	}
	// org has been set up properly
	offset = org + 2*24*80;  // offset = beginning of row 24
	//copy a line of BLANKS to row 24
	w = 0x0C00;	//HRGB=1100 ==> HighLight RED, Null char
	for (i=0; i<80; i++)
	{
		put_word(w, base, offset + 2*i);
	}
	set_VDC(VID_ORG, org >> 1);  // set VID_ORG to org
}

int move_cursor()	// move cursor to current position
{
	int pos = 2*(row*80 + column);
	offset = (org + pos) & vid_mask;
	set_VDC(CURSOR, offset >> 1);
}

// display a char, handle special chars '\n', '\r', '\b'
int putc(char c)
{
	u16 w, pos;
	if (c=='\n')
	{
		while(column < 80)
		{
			pos = 2*(row*80 + column);
			offset = (org + pos) & vid_mask;
			put_word(0x0700, base, offset);
			column++;
		}
		row++;
		if (row>=25)
		{
			row = 24;
			scroll();
		}
		move_cursor();
		return;
	}
	if (c=='\r')
	{
		column = 0;
		move_cursor();
		return;
	}
	if (c=='\b')
	{
		if (column > 0)
		{
			column--;
			move_cursor();
			put_word(0x0700, base, offset);
		}
		return;
	}
	// c is an ordinary char
	pos = 2 * (row*80 + column);
	offset = (org + pos) & vid_mask;
	w = (color << 8) + c;
	put_word(w, base, offset);
	column++;
	if (column >= 80)
	{
		column = 0;
		row++;
		if (row >= 25)
		{
			row = 24;
			scroll();
		}
	}
	move_cursor();
}

int set_VDC(u16 reg, u16 val) // set VDC register reg to val
{
	lock();
	out_byte(VDC_INDEX, reg);			// set index register
	out_byte(VDC_DATA, (val>>8)&0xFF);	// output high byte
	out_byte(VDC_INDEX, reg + 1);		// next index register
	out_byte(VDC_DATA, val&0xFF);		// output low byte
	unlock();
}