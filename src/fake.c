#include <stdlib.h>
#include "driver.h"


static struct GfxLayout centipede_charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(1,2), 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static struct GfxLayout centipede_spritelayout =
{
	8,16,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(1,2), 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	16*8
};

static struct GfxDecodeInfo centiped_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &centipede_charlayout,     0, 1 },
	{ REGION_GFX1, 0, &centipede_spritelayout,   4, 4*4*4 },
	{ -1 }
};

static unsigned gfx_total_size(struct GfxDecodeInfo* info) {
	return memory_region_length(info[0].memory_region); // TODO: doesn't work for multiregion
}

static unsigned get32LE(unsigned char* data) {
	return data[0] | ((unsigned)data[1]<<8) | ((unsigned)data[2]<<16) || ((unsigned)data[3]<<24);
}

static unsigned get_from_bmp(unsigned char* bmp, unsigned x, unsigned y) {
	unsigned off_bits = get32LE(bmp+10);
	unsigned width = get32LE(bmp+14+4);
	unsigned height = get32LE(bmp+14+8);
	return bmp[off_bits+width*(height-1-y)+x];
}

static void encode_layout(unsigned char* out, unsigned char* bmp, unsigned regionSize, struct GfxLayout* layout, unsigned start) {
}


static void* encode_gfx(struct GfxDecodeInfo* info, FILE* bmpFile, unsigned minSize) {
	unsigned len = gfx_total_size(info);
	if (len<minSize)
		len = minSize;
	unsigned char* buf = malloc(len);

	if (buf == NULL)
		return buf;

	for(int i=0;i<len;i++)
		buf[i]=0;

	fseek(bmpFile, 0, SEEK_END);
	unsigned bmpSize = ftell(bmpFile);
	fseek(bmpFile, 0, SEEK_SET);
	unsigned char* bmp = malloc(bmpSize);
	if (bmp == NULL) 
		return buf;
	fread(bmp, bmpSize, 1, bmpFile);
	puts("read bmp");

	unsigned rlen = memory_region_length(info[0].memory_region); // TODO: multiregion

	while(info->memory_region != -1) {
		encode_layout(buf, bmp, rlen, info->gfxlayout, info->start);
		info++;
	}

	return buf;
}


static struct GfxDecodeInfo milliped_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &centipede_charlayout,     0, 4 },
	{ REGION_GFX1, 0, &centipede_spritelayout, 4*4, 4*4*4*4 },
	{ -1 }
};



/*************************************
 *
 *	Graphics layouts: Warlords
 *
 *************************************/

static struct GfxLayout warlords_charlayout =
{
	8,8,
	RGN_FRAC(1,4),
	2,
	{ RGN_FRAC(1,2), 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static struct GfxDecodeInfo warlords_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0x000, &warlords_charlayout, 0,   8 },
	{ REGION_GFX1, 0x200, &warlords_charlayout, 8*4, 8*4 },
	{ -1 }
};

struct fake_piece {
    const char* name;
    unsigned size;
    const char* originalFile;
    unsigned originalOffset;
    unsigned originalSize; // if smaller than size, will read more than once
    struct GfxDecodeInfo* gfx;
};

struct fake_whole {
    const char* zipname;
    struct fake_piece pieces[];
};    

struct fake_whole bwidow = {
    "bwidow.zip", 
    { 
    { "136017.107", 2048, "Black Widow.bin", 0 },
    { "136017.108", 4096 },
    { "136017.109" },
    { "136017.110" },
    { "136017.101" },
    { "136017.102" },
    { "136017.103" },
    { "136017.104" },
    { "136017.105" },
    { "136017.106" },
    { NULL }
    }
};

struct fake_whole centiped3 = {
    "centiped.zip",
    { 
     {"centiped.307", 2048, "Centipede.bin", 0 },
     {"centiped.308" },
     {"centiped.309" },
     {"centiped.310" },
     {"centiped.211", 2048, "Centipede.bmp", 0, 2048, &centiped_gfxdecodeinfo },
     {"centiped.212", 2048, "Centipede.bmp", 2048, 2048, &centiped_gfxdecodeinfo },
     {NULL} //TODO:gfx:http://adb.arcadeitalia.net/dettaglio_mame.php?game_name=centiped3&search_id=
    }
};

struct fake_whole asteroid = {
    "asteroid.zip",
    {
        { "035145.02", 2048, "Asteroids.bin" },
        { "035144.02" },
        { "035143.02" },
        { "035127.02" },
        { NULL }
    }
};

struct fake_whole astdelux = {
    "astdelux.zip",
    {
        {"036430.02", 2048, "Asteroids Deluxe.bin"},
        {"036431.02"},
        {"036432.02"},
        {"036433.03"},
        {"036800.02"},
        {"036799.01"},
        {NULL}
    }
};

struct fake_whole gravitar = {
    "gravitar.zip",
    {
        {"136010.210", 0x800, "Gravitar.bin" },
        {"136010.207", 0x1000 },
        {"136010.208"},
        {"136010.309"},
        {"136010.301"},
        {"136010.302"},
        {"136010.303"}, 
        {"136010.304"},
        {"136010.305"},
        {"136010.306"},
        {NULL}
    }
};

struct fake_whole llander = {
    "llander.zip",
    {
        {"034599.01", 2048, "Lunar Lander.bin"},
        {"034598.01"},{"034597.01"}, {"034572.02"}, {"034571.02"}, {"034570.01"}, {"034569.02"}, 
        {NULL}
    }
};

struct fake_whole mhavoc = {
    "mhavoc.zip",
    {
        {"136025.210", 0x2000, "Major Havoc.bin", 0, 0x1000 },
        {"", 0x2000, "Major Havoc.bin", 0, 0x1000 },
        {"136025.216", 0x4000, "Major Havoc.bin", 0x1000, 0x4000 },
        {"136025.217"},
        {"136025.215", 0x4000, "Major Havoc alpha banks.bin" },
        {"136025.318"},
        {"136025.106", 0x4000, "Major Havoc vector banks.bin" },
        {"136025.107"},
        {"136025.108", 0x4000, "Major Havoc gamma.bin" },        
    }
};

struct fake_whole missile = {
    "missile.zip",
    {
        {"035820.02", 2048, "Missile Command.bin" },
            {"035821.02"}, {"035822.02"}, {"035823.02"}, {"035824.02"}, {"035825.02"}
    }
};

struct fake_whole redbaron = {
    "redbaron.zip",
    {
        {"037587.01", 4096, "Red Baron.bin", 0, 2048 },
        {"", 4096, "Red Baron.bin", 2*2048, 2048 },
        {"037000.01e", 2048, "Red Baron.bin", 2048 },
        {"036998.01e", 2048, "Red Baron.bin", 3*2048 },
        {"036997.01e"}, {"036996.01e"}, {"036995.01e"}, {"037006.01e"}, {"037007.01e"}
    }
};

struct fake_whole spacduel = {
    "spacduel.zip",
    {
        {"136006.106", 0x800, "Space Duel.bin" },
        {"136006.107", 0x1000 }, {"136006.201"}, {"136006.102"}, {"136006.103"}, {"136006.104"}, {"136006.105"}
    }
};

struct fake_whole tempest3 = {
    "tempest3.zip",
    {
        {"237.002", 4096, "Tempest.bin"},
            {"136.002"}, {"235.002"}, {"134.002"}, {"133.002"}, {"138.002"}
    }
};

/*
struct fake_whole centiped = {
    "centiped.zip",
    {
        {"136001.407", 2048, "Centipede.bin" },
            {"136001.408"},{"136001.409"},{"136001.410"},{"136001.211"},{"136001.212"},{"136001.213"}
    }
};
*/

struct fake_whole* substitutions[] = {
    &bwidow,
    &asteroid,
    &astdelux,
    &gravitar,
    &llander,
    &mhavoc,
    &missile,
    &redbaron,
    &spacduel,
    &tempest3,
    &centiped3,
    NULL
};

static void fix(struct fake_whole* f) {
    const char* lastOriginalFile = NULL;  
    unsigned lastSize = 0;
    unsigned lastOffset = 0;
    
    struct fake_piece* piece = f->pieces;
    
    while (piece->name != NULL) {
        if (piece->size == 0)
            piece->size = lastSize;
        else
            lastSize = piece->size;
        
        if (piece->originalFile == NULL)
            piece->originalFile = lastOriginalFile;
        else {
            lastOriginalFile = piece->originalFile;
            lastOffset = 0;
        }
        
        if (piece->originalOffset == 0)
            piece->originalOffset = lastOffset;
        else
            lastOffset = piece->originalOffset;
        
        if (piece->originalSize == 0)
            piece->originalSize = piece->size;
        
        lastOffset += piece->originalSize;
        piece++;
    }
}


