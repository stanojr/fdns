/*
 * Copyright (C) 2019-2021 FDNS Authors
 *
 * This file is part of fdns project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "../fdns/fdns.h"
#include <arpa/inet.h>

typedef struct htable_t {
	uint8_t avalue;	// ascii value
	uint32_t hvalue;	// huffman value
	uint8_t bits;		// number of valid bits in hvalue (LSB)
} HTable;

static HTable htable[] = {
	{0x30, 0x00000000,  5},  /* '0' */
	{0x31, 0x00000001,  5},  /* '1' */
	{0x32, 0x00000002,  5},  /* '2' */
	{0x61, 0x00000003,  5},  /* 'a' */
	{0x63, 0x00000004,  5},  /* 'c' */
	{0x65, 0x00000005,  5},  /* 'e' */
	{0x69, 0x00000006,  5},  /* 'i' */
	{0x6f, 0x00000007,  5},  /* 'o' */
	{0x73, 0x00000008,  5},  /* 's' */
	{0x74, 0x00000009,  5},  /* 't' */
	{0x20, 0x00000014,  6},  /* ' ' */
	{0x25, 0x00000015,  6},  /* '%' */
	{0x2d, 0x00000016,  6},  /* '-' */
	{0x2e, 0x00000017,  6},  /* '.' */
	{0x2f, 0x00000018,  6},  /* '/' */
	{0x33, 0x00000019,  6},  /* '3' */
	{0x34, 0x0000001a,  6},  /* '4' */
	{0x35, 0x0000001b,  6},  /* '5' */
	{0x36, 0x0000001c,  6},  /* '6' */
	{0x37, 0x0000001d,  6},  /* '7' */
	{0x38, 0x0000001e,  6},  /* '8' */
	{0x39, 0x0000001f,  6},  /* '9' */
	{0x3d, 0x00000020,  6},  /* '=' */
	{0x41, 0x00000021,  6},  /* 'A' */
	{0x5f, 0x00000022,  6},  /* '_' */
	{0x62, 0x00000023,  6},  /* 'b' */
	{0x64, 0x00000024,  6},  /* 'd' */
	{0x66, 0x00000025,  6},  /* 'f' */
	{0x67, 0x00000026,  6},  /* 'g' */
	{0x68, 0x00000027,  6},  /* 'h' */
	{0x6c, 0x00000028,  6},  /* 'l' */
	{0x6d, 0x00000029,  6},  /* 'm' */
	{0x6e, 0x0000002a,  6},  /* 'n' */
	{0x70, 0x0000002b,  6},  /* 'p' */
	{0x72, 0x0000002c,  6},  /* 'r' */
	{0x75, 0x0000002d,  6},  /* 'u' */
	{0x3a, 0x0000005c,  7},  /* ':' */
	{0x42, 0x0000005d,  7},  /* 'B' */
	{0x43, 0x0000005e,  7},  /* 'C' */
	{0x44, 0x0000005f,  7},  /* 'D' */
	{0x45, 0x00000060,  7},  /* 'E' */
	{0x46, 0x00000061,  7},  /* 'F' */
	{0x47, 0x00000062,  7},  /* 'G' */
	{0x48, 0x00000063,  7},  /* 'H' */
	{0x49, 0x00000064,  7},  /* 'I' */
	{0x4a, 0x00000065,  7},  /* 'J' */
	{0x4b, 0x00000066,  7},  /* 'K' */
	{0x4c, 0x00000067,  7},  /* 'L' */
	{0x4d, 0x00000068,  7},  /* 'M' */
	{0x4e, 0x00000069,  7},  /* 'N' */
	{0x4f, 0x0000006a,  7},  /* 'O' */
	{0x50, 0x0000006b,  7},  /* 'P' */
	{0x51, 0x0000006c,  7},  /* 'Q' */
	{0x52, 0x0000006d,  7},  /* 'R' */
	{0x53, 0x0000006e,  7},  /* 'S' */
	{0x54, 0x0000006f,  7},  /* 'T' */
	{0x55, 0x00000070,  7},  /* 'U' */
	{0x56, 0x00000071,  7},  /* 'V' */
	{0x57, 0x00000072,  7},  /* 'W' */
	{0x59, 0x00000073,  7},  /* 'Y' */
	{0x6a, 0x00000074,  7},  /* 'j' */
	{0x6b, 0x00000075,  7},  /* 'k' */
	{0x71, 0x00000076,  7},  /* 'q' */
	{0x76, 0x00000077,  7},  /* 'v' */
	{0x77, 0x00000078,  7},  /* 'w' */
	{0x78, 0x00000079,  7},  /* 'x' */
	{0x79, 0x0000007a,  7},  /* 'y' */
	{0x7a, 0x0000007b,  7},  /* 'z' */
	{0x26, 0x000000f8,  8},  /* '&' */
	{0x2a, 0x000000f9,  8},  /* '*' */
	{0x2c, 0x000000fa,  8},  /* ',' */
	{0x3b, 0x000000fb,  8},  /* ';' */
	{0x58, 0x000000fc,  8},  /* 'X' */
	{0x5a, 0x000000fd,  8},  /* 'Z' */
	{0x21, 0x000003f8, 10},  /* '!' */
	{0x22, 0x000003f9, 10},  /* '"' */
	{0x28, 0x000003fa, 10},  /* '(' */
	{0x29, 0x000003fb, 10},  /* ')' */
	{0x3f, 0x000003fc, 10},  /* '?' */
	{0x27, 0x000007fa, 11},  /* ''' */
	{0x2b, 0x000007fb, 11},  /* '+' */
	{0x7c, 0x000007fc, 11},  /* '|' */
	{0x23, 0x00000ffa, 12},  /* '#' */
	{0x3e, 0x00000ffb, 12},  /* '>' */
	{0x00, 0x00001ff8, 13},
	{0x24, 0x00001ff9, 13},  /* '$' */
	{0x40, 0x00001ffa, 13},  /* '@' */
	{0x5b, 0x00001ffb, 13},  /* '[' */
	{0x5d, 0x00001ffc, 13},  /* ']' */
	{0x7e, 0x00001ffd, 13},  /* '~' */
	{0x5e, 0x00003ffc, 14},  /* '^' */
	{0x7d, 0x00003ffd, 14},  /* '}' */
	{0x3c, 0x00007ffc, 15},  /* '<' */
	{0x60, 0x00007ffd, 15},  /* '`' */
	{0x7b, 0x00007ffe, 15},  /* '{' */
	{0x5c, 0x0007fff0, 19},  /* '\' */
	{0xc3, 0x0007fff1, 19},
	{0xd0, 0x0007fff2, 19},
	{0x80, 0x000fffe6, 20},
	{0x82, 0x000fffe7, 20},
	{0x83, 0x000fffe8, 20},
	{0xa2, 0x000fffe9, 20},
	{0xb8, 0x000fffea, 20},
	{0xc2, 0x000fffeb, 20},
	{0xe0, 0x000fffec, 20},
	{0xe2, 0x000fffed, 20},
	{0x99, 0x001fffdc, 21},
	{0xa1, 0x001fffdd, 21},
	{0xa7, 0x001fffde, 21},
	{0xac, 0x001fffdf, 21},
	{0xb0, 0x001fffe0, 21},
	{0xb1, 0x001fffe1, 21},
	{0xb3, 0x001fffe2, 21},
	{0xd1, 0x001fffe3, 21},
	{0xd8, 0x001fffe4, 21},
	{0xd9, 0x001fffe5, 21},
	{0xe3, 0x001fffe6, 21},
	{0xe5, 0x001fffe7, 21},
	{0xe6, 0x001fffe8, 21},
	{0x81, 0x003fffd2, 22},
	{0x84, 0x003fffd3, 22},
	{0x85, 0x003fffd4, 22},
	{0x86, 0x003fffd5, 22},
	{0x88, 0x003fffd6, 22},
	{0x92, 0x003fffd7, 22},
	{0x9a, 0x003fffd8, 22},
	{0x9c, 0x003fffd9, 22},
	{0xa0, 0x003fffda, 22},
	{0xa3, 0x003fffdb, 22},
	{0xa4, 0x003fffdc, 22},
	{0xa9, 0x003fffdd, 22},
	{0xaa, 0x003fffde, 22},
	{0xad, 0x003fffdf, 22},
	{0xb2, 0x003fffe0, 22},
	{0xb5, 0x003fffe1, 22},
	{0xb9, 0x003fffe2, 22},
	{0xba, 0x003fffe3, 22},
	{0xbb, 0x003fffe4, 22},
	{0xbd, 0x003fffe5, 22},
	{0xbe, 0x003fffe6, 22},
	{0xc4, 0x003fffe7, 22},
	{0xc6, 0x003fffe8, 22},
	{0xe4, 0x003fffe9, 22},
	{0xe8, 0x003fffea, 22},
	{0xe9, 0x003fffeb, 22},
	{0x01, 0x007fffd8, 23},
	{0x87, 0x007fffd9, 23},
	{0x89, 0x007fffda, 23},
	{0x8a, 0x007fffdb, 23},
	{0x8b, 0x007fffdc, 23},
	{0x8c, 0x007fffdd, 23},
	{0x8d, 0x007fffde, 23},
	{0x8f, 0x007fffdf, 23},
	{0x93, 0x007fffe0, 23},
	{0x95, 0x007fffe1, 23},
	{0x96, 0x007fffe2, 23},
	{0x97, 0x007fffe3, 23},
	{0x98, 0x007fffe4, 23},
	{0x9b, 0x007fffe5, 23},
	{0x9d, 0x007fffe6, 23},
	{0x9e, 0x007fffe7, 23},
	{0xa5, 0x007fffe8, 23},
	{0xa6, 0x007fffe9, 23},
	{0xa8, 0x007fffea, 23},
	{0xae, 0x007fffeb, 23},
	{0xaf, 0x007fffec, 23},
	{0xb4, 0x007fffed, 23},
	{0xb6, 0x007fffee, 23},
	{0xb7, 0x007fffef, 23},
	{0xbc, 0x007ffff0, 23},
	{0xbf, 0x007ffff1, 23},
	{0xc5, 0x007ffff2, 23},
	{0xe7, 0x007ffff3, 23},
	{0xef, 0x007ffff4, 23},
	{0x09, 0x00ffffea, 24},
	{0x8e, 0x00ffffeb, 24},
	{0x90, 0x00ffffec, 24},
	{0x91, 0x00ffffed, 24},
	{0x94, 0x00ffffee, 24},
	{0x9f, 0x00ffffef, 24},
	{0xab, 0x00fffff0, 24},
	{0xce, 0x00fffff1, 24},
	{0xd7, 0x00fffff2, 24},
	{0xe1, 0x00fffff3, 24},
	{0xec, 0x00fffff4, 24},
	{0xed, 0x00fffff5, 24},
	{0xc7, 0x01ffffec, 25},
	{0xcf, 0x01ffffed, 25},
	{0xea, 0x01ffffee, 25},
	{0xeb, 0x01ffffef, 25},
	{0xc0, 0x03ffffe0, 26},
	{0xc1, 0x03ffffe1, 26},
	{0xc8, 0x03ffffe2, 26},
	{0xc9, 0x03ffffe3, 26},
	{0xca, 0x03ffffe4, 26},
	{0xcd, 0x03ffffe5, 26},
	{0xd2, 0x03ffffe6, 26},
	{0xd5, 0x03ffffe7, 26},
	{0xda, 0x03ffffe8, 26},
	{0xdb, 0x03ffffe9, 26},
	{0xee, 0x03ffffea, 26},
	{0xf0, 0x03ffffeb, 26},
	{0xf2, 0x03ffffec, 26},
	{0xf3, 0x03ffffed, 26},
	{0xff, 0x03ffffee, 26},
	{0xcb, 0x07ffffde, 27},
	{0xcc, 0x07ffffdf, 27},
	{0xd3, 0x07ffffe0, 27},
	{0xd4, 0x07ffffe1, 27},
	{0xd6, 0x07ffffe2, 27},
	{0xdd, 0x07ffffe3, 27},
	{0xde, 0x07ffffe4, 27},
	{0xdf, 0x07ffffe5, 27},
	{0xf1, 0x07ffffe6, 27},
	{0xf4, 0x07ffffe7, 27},
	{0xf5, 0x07ffffe8, 27},
	{0xf6, 0x07ffffe9, 27},
	{0xf7, 0x07ffffea, 27},
	{0xf8, 0x07ffffeb, 27},
	{0xfa, 0x07ffffec, 27},
	{0xfb, 0x07ffffed, 27},
	{0xfc, 0x07ffffee, 27},
	{0xfd, 0x07ffffef, 27},
	{0xfe, 0x07fffff0, 27},
	{0x02, 0x0fffffe2, 28},
	{0x03, 0x0fffffe3, 28},
	{0x04, 0x0fffffe4, 28},
	{0x05, 0x0fffffe5, 28},
	{0x06, 0x0fffffe6, 28},
	{0x07, 0x0fffffe7, 28},
	{0x08, 0x0fffffe8, 28},
	{0x0b, 0x0fffffe9, 28},
	{0x0c, 0x0fffffea, 28},
	{0x0e, 0x0fffffeb, 28},
	{0x0f, 0x0fffffec, 28},
	{0x10, 0x0fffffed, 28},
	{0x11, 0x0fffffee, 28},
	{0x12, 0x0fffffef, 28},
	{0x13, 0x0ffffff0, 28},
	{0x14, 0x0ffffff1, 28},
	{0x15, 0x0ffffff2, 28},
	{0x17, 0x0ffffff3, 28},
	{0x18, 0x0ffffff4, 28},
	{0x19, 0x0ffffff5, 28},
	{0x1a, 0x0ffffff6, 28},
	{0x1b, 0x0ffffff7, 28},
	{0x1c, 0x0ffffff8, 28},
	{0x1d, 0x0ffffff9, 28},
	{0x1e, 0x0ffffffa, 28},
	{0x1f, 0x0ffffffb, 28},
	{0x7f, 0x0ffffffc, 28},
	{0xdc, 0x0ffffffd, 28},
	{0xf9, 0x0ffffffe, 28},
	{0x0a, 0x3ffffffc, 30},
	{0x0d, 0x3ffffffd, 30},
	{0x16, 0x3ffffffe, 30},
	{0, 0x3fffffff, 30}, // end of string
};

typedef struct htree_t {
	struct htree_t *next[2];
	HTable *htable;
} HTree;

static HTree *htree = NULL;

static inline HTree *ht_new(void) {
	HTree *ht = malloc(sizeof(HTree));
	if (!ht)
		errExit("malloc");
	memset(ht, 0, sizeof(HTree));
	return ht;
}

static void ht_add(HTable *tbl) {
	assert(tbl);

	uint32_t mask = 1;
	int i;
	for (i = 0; i < tbl->bits - 1; i++, mask <<= 1);

	HTree *ptr = htree;
	assert(ptr);
	for (i = tbl->bits; i > 0; i--, mask >>= 1) {
		int bitval = (mask & tbl->hvalue)? 1: 0;
		if (ptr->next[bitval] == NULL) {
//			printf("adding %d\n", bitval);
			ptr->next[bitval] = ht_new();
		}
		ptr = ptr->next[bitval];
	}
	ptr->htable = tbl;
}
#if 0
result 30
result 31
result 32
result 61
result 25
result 68
#endif
// return the table structure for the character char at specified offset
static HTable *ht_search_char(uint8_t *input, int offset) {
	assert(offset < 8);
	if (htree == NULL) { // not initialized?
		// build huffman tree
		htree = ht_new(); // top of the tree

		unsigned i;
		for (i = 0; i < sizeof(htable) / sizeof(HTable); i++)
			ht_add(htable + i);
	}

	uint32_t in;
	memcpy(&in, input, 4);
	in = ntohl(in);
	in <<= offset;

	uint32_t mask = 0x80000000;
	HTree *ptr = htree;
	int i;
	for (i = 0; i < 19; i++, mask >>= 1) {	// 19 = maximum number of bits in our table
		assert(ptr);
		int bitval = (mask & in)? 1: 0;
		if (ptr->next[bitval] == NULL) {
			assert(ptr->htable);
			return ptr->htable;
		}
		ptr = ptr->next[bitval];
	}
	return ptr->htable;
}


// decode cnt chars from hstr
// returns decoded string and the number of bytes consumed in hstr
#define MAXOUTPUT 1024
static uint8_t search_out[MAXOUTPUT + 1];
uint8_t all_f[] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
char *huffman_search(uint8_t *hstr, int len) {
	int outpos = 0;
	int bcnt = 0; // bit counting
	int bpos = 0; // position in input string

	while (bpos < (len - 1) && outpos < MAXOUTPUT) {
		int boffset = bcnt % 8;
		bpos = bcnt / 8;
		if (bpos == (len - 1)) {
			// check incomplete EOS
			if ((*(hstr + bpos) & all_f[boffset]) == all_f[boffset])
				break;
		}
//printf("\n*ptr 0x%02x - len %d, bcnt %d: bpos %d, boffset %d - ", *(hstr + bpos), len, bcnt, bpos, boffset);

		HTable *res = ht_search_char(hstr + bpos, boffset);
		assert(res);
//printf("add %c\n", res->avalue);
		search_out[outpos++] = res->avalue;
		bcnt += res->bits;
	}
	search_out[outpos] = '\0';
	return  (char *) search_out;
}


#if 0
// test value 0123
// 0000 0|000 01|00 010|0 0011 = 00443

int main(void) {
	uint8_t str[] = {0, 0x44, 0x35, 0x67, 0, 0, 0, 0, 0};
	int consumed;
	char *retval =huffman_search(str, 5, &consumed);
printf("retval #%s#, consumed %d\n", retval, consumed);

	return 0;
}
#endif
