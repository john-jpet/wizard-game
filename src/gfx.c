
#include "gfx.h"
#include "game.h"
// Enemy structure


// Sprite data
const unsigned char wizardwalkr2[]={
	  0,  0,0x20,0|OAM_FLIP_H,
	  0,  8,0x30,0|OAM_FLIP_H,
	  8,  0,0x21,0|OAM_FLIP_H,
	  8,  8,0x31,0|OAM_FLIP_H,
	128
};

const unsigned char wizardwalkr1[]={
	  0,  0,0x02,0|OAM_FLIP_H,
	  0,  8,0x12,0|OAM_FLIP_H,
	  8,  0,0x03,0|OAM_FLIP_H,
	  8,  8,0x13,0|OAM_FLIP_H,
	128
};

const unsigned char wizardwalk2[]={
	  0,  0,0x21,0,
	  0,  8,0x31,0,
	  8,  0,0x20,0,
	  8,  8,0x30,0,
	128
};

const unsigned char wizardwalk1[]={
	  0,  0,0x03,0,
	  0,  8,0x13,0,
	  8,  0,0x02,0,
	  8,  8,0x12,0,
	128
};

const unsigned char wizardstand[]={
	  0,  0,0x01,0,
	  0,  8,0x10,0|OAM_FLIP_H,
	  8,  0,0x00,0,
	  8,  8,0x10,0,
	128
};

const unsigned char wizardfire[]={
	  0,  0,0x05,0,
	  0,  8,0x14,0|OAM_FLIP_H,
	  8,  0,0x04,0,
	  8,  8,0x14,0,
	128
};

const unsigned char imp[]={
	  0,  0,0x22,2|OAM_FLIP_H,
	  0,  8,0x32,2|OAM_FLIP_H,
	  8,  0,0x22,2,
	  8,  8,0x32,2,
	128
};

const unsigned char imp1[]={
	  0,  0,0x23,2|OAM_FLIP_H,
	  0,  8,0x33,2|OAM_FLIP_H,
	  8,  0,0x23,2,
	  8,  8,0x33,2,
	128
};
const unsigned char diveimp[]={
	  0,  0,0x22,1|OAM_FLIP_H,
	  0,  8,0x32,1|OAM_FLIP_H,
	  8,  0,0x22,1,
	  8,  8,0x32,1,
	128
};

const unsigned char diveimp1[]={
	  0,  0,0x23,1|OAM_FLIP_H,
	  0,  8,0x33,1|OAM_FLIP_H,
	  8,  0,0x23,1,
	  8,  8,0x33,1,
	128
};

const unsigned char large_bullet[]={
	  0,  0,0x24,1|OAM_FLIP_H,
	  0,  8,0x34,1|OAM_FLIP_H,
	  8,  0,0x24,1,
	  8,  8,0x34,1,
	128
};


// Palette data
const unsigned char palette_bg[]={
0x0f, 0x27, 0x16, 0x30, // black, gray, lt gray, white
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 

const unsigned char palette_sp[]={
0x0f, 0x02, 0x27, 0x36, // black, blue, yellow, apricot
0x0f, 0x06, 0x16, 0x27, // black, dark red, light red, yellow
0x0f,0x03,0x13,0x23,  // black, dark purple, purple, light purple
0,0,0,0
}; 

