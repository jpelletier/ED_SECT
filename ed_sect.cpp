//TAB=4
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <conio.h>

//#include "ide.h"
#include "keys.h"
#include "ppide.h"
#include "ed_sect.h"

//#define DEBUG_KEYS
#define USE_IDE
#define USE_IDE_INIT

/*-----------------------------------------
;Byte edit
;+      increment
;-      decrement
;!      invert
;~      negate
;R      reverse/swap bits 7:0 -> 0:7
;S		swap nibbles
;bsp    remove previous hex
;Z      00
;
;space
;enter  accept byte and go to next byte
;Q      scrap byte and go to next byte
;
;Cursor movement
;c-A    left
;c-D    right
;c-W    up
;c-X    down
;
;space  store value and goto next byte
;H		value -> L
;L      value -> L
;P      previous 256-byte block
;N      next 256-byte block
;U      word value -> HL
;X      exit binary editor
;W		write current sector
;-----------------------------------------*/
uint8_t		sector_buffer[SECTOR_SIZE];
uint32_t	LBA;
uint32_t	TOTAL_NB_SECT;

uint16_t 	hl;
uint8_t		row,col;

void GetAnyKey(void)
{
	printf("\033[23;40HPress any key to continue\033[0K");
	GetChar();
}

#ifndef USE_IDE_INIT
void CppIdeInit(void)
{
	printf("\033[23;1HCONFIG RESET\033[0K");
	IDERST();
	WRDY();
	
	GetAnyKey();

	printf("\033[23;1HSELECT MASTER\033[0K");
	SELMST();
	WRDY();
	
	printf("\033[23;1HSELECT MASTER: %08lX\033[0K",IDE_GET_LBA());
	//FIXME 20260923: GET LBA returns 02468080

	GetAnyKey();

	printf("\033[23;1HSELECT PIO8\033[0K");
	SELPIO8();
	WRDY();

	GetAnyKey();

	printf("\033[23;1HREADY\033[0K");
}
#endif

int main(void)
{
uint8_t b,c;

	LBA = 0L;
	clearSectBuff();

    //Clear screen and set lightgray on black
    //on reset: white on red
	printf("\033[40;37m\033[2J\033[41;37mSector editor\r");

#ifdef USE_IDE
	// ON RETURN, ZF SET INDICATES HARDWARE FOUND
	//printf("\033[23;1HClearing sector buffer\033[0K");
	//printf("\033[23;1HIDE_DETECT\033[0K");
	if (IDE_DETECT() == 0xFF)
	{
		//blinking black on yellow = wait init
		printf("\033[H\033[43;30m\033[5mSector editor\r");
	}

	printf("\033[25m\033[23;1HIDE_INIT\033[0K");

#ifdef USE_IDE_INIT
	IDE_INIT();
#else
	CppIdeInit();
#endif

#endif
	//printf("\033[23;1HIDE_GET_ERROR: %04X",IDE_GET_ERROR());

    //black on green = ready
	printf("\033[H\033[42;30m\033[25mSector editor\033[m\r");

	//printf("\033[23;1HgetDiskInfo\033[0K");
	getDiskInfo();

#ifdef USE_IDE
	IDE_SET_LBA(LBA);
	IDE_READ_SECTOR(sector_buffer);
	//printf("\033[23;1HReading sector %0x08lX\033[0K",LBA);

#endif
	printf("\033[23;1H\033[0K");
	printf("\033[1;17HLBA: \033[44m%08lX\033[m\033[2;1H",LBA);

	hl = 0;
	redraw_dump();

	//goto row 3 column 6
	printf("\033[3;6H");

	hl = 0;

	do
	{
		printf("\033[44m");

		b = sector_buffer[hl];
		c = in_b_k(&b);

		switch(c)
		{
			case ' ':
				sector_buffer[hl] = b;
				move_right();
				break;

			case 'P':
				//previous 256-byte block
				hl &= 0x00ff;
				//keep color
				update_dump();
				break;

			case 'N':
				//next 256-byte block
				hl |= 0x0100;
				//keep color
				update_dump();
				break;

			case kc_A:
				move_left();
				break;

			case kc_D:
				move_right();
				break;

			case kc_X:
				move_down();
				break;

			case kc_W:
				move_up();
				break;

			case 'L': //set LBA
				printf("\033[1;22H\033[33;45m");
				UIGetAsciiDouble(&LBA);

				if (LBA > TOTAL_NB_SECT)
				{
					LBA = TOTAL_NB_SECT - 1L;
				}

				UpdateLbaSector();
				break;

			case 'W': //write current sector
#ifdef USE_IDE
				IDE_SET_LBA(LBA);
				IDE_WRITE_SECTOR(sector_buffer);
#endif
				put2Backspaces();
				break;

			case 'X':
				printf("\033[40;37m\033[2J");
				return 0;

			case 0:
				switch(GetChar())
				{
					case kUp:
						move_up();
						break;

					case kDn:
						move_down();
						break;

					case kRight:
						move_right();
						break;

					case kLeft:
						move_left();
						break;

					case kPgUp:	//page up
						//increment LBA
						LBA++;
						if (LBA >= TOTAL_NB_SECT)
						{
							LBA = 0;
						}
						UpdateLbaSector();
						break;

					case kPgDn: //page down
						//decrement LBA
						if (LBA == 0)
						{
							LBA = TOTAL_NB_SECT;
						}
						LBA--;
						UpdateLbaSector();
						break;

					default:
#ifdef DEBUG_KEYS
						printf("\033[s\033[23;1H%02X ",c);
						c = GetChar();
						printf("%02X ",c);
						do{
							c = GetChar();
							printf("%02X ",c);
						}while (c != 0x1b);

						printf("\033[u");
#else
						put2Backspaces();
#endif				
				}
				break;

			default:
				put2Backspaces();
		}
	} while (true);
}

void clearSectBuff(void)
{
uint16_t hl = 0;

	do
	{
		sector_buffer[hl] = 0;
	} while (hl++ != SECTOR_SIZE);
}

void getDiskInfo(void)
{
	clearSectBuff();

#ifdef USE_IDE
	IDE_GET_DISK_INFO(sector_buffer);
#endif

// Word Offset		Byte Offset		Field Description	Format
// Words 10–19		0x14 – 0x27		Serial Number		20-character ASCII string
// Words 27–46		0x36 – 0x5D		Model Number/Name	40-character ASCII string
// Words 60–61		0x78 – 0x7B		Total LBA Sectors	32-bit Integer (Capacity)

#ifdef USE_IDE
	TOTAL_NB_SECT = *((uint32_t *)(sector_buffer+0x78));

	printf("\033[20;1HSerial Number: %s\r\n",sector_buffer + 0x14);
	printf("Model Nb/name: %s\r\n",sector_buffer + 0x36);
	printf("Total LBA sec: %08lX",TOTAL_NB_SECT);
#else
	TOTAL_NB_SECT = 0x0BADF00DL;

	printf("\033[20;1HSerial Number: %s\r\n","Dummy serial number");
	printf("Model Nb/name: %s\r\n","dummy mc dumbface");
	printf("Total LBA sec: %08lX",TOTAL_NB_SECT);
#endif
	
	clearSectBuff();
}


//current LBA
//display current LBA and load sector
void UpdateLbaSector(void)
{
	printf("\033[1;22H\033[37;44m%08lX\033[m",LBA);
#ifdef USE_IDE
	IDE_SET_LBA(LBA);
	IDE_READ_SECTOR(sector_buffer);
#endif
	update_dump();
}

void update_dump(void)
{
    //home
    printf("\033[m\033[2;1H");
	//redraw
    redraw_dump();
    move_cursor();
}

void redraw_dump(void)
{
uint16_t tmp;

	tmp = hl & 0xff00;
    mem_dump(tmp);
	//puts(msg_help);
}

void move_left(void)
{
uint16_t tmp;
uint16_t l;

	tmp = hl & 0xfff0;
	l = hl & 0x000f;
	restore_color();
	//roll decrement
	l--;
	l &= 0x000f;
	hl = tmp | l;
	move_cursor();
}

void move_right(void)
{
uint16_t tmp;
uint16_t l;

	tmp = hl & 0xfff0;
	l = hl & 0x000f;
	restore_color();
	//roll increment
	l++;
	l &= 0x000f;
	hl = tmp | l;
	move_cursor();
}

void move_up(void)
{
uint8_t h,l;

	h = hl >> 8;
	l = hl & 0xff;
	restore_color();
	//roll decrement
	l -= 0x10;
	hl = (h << 8) | l;
	move_cursor();
}

void move_down(void)
{
uint8_t h,l;

	h = hl >> 8;
	l = hl & 0xff;
	restore_color();
	//roll increment
	l += 0x10;
	hl = (h << 8) | l;
	move_cursor();
}

void move_cursor(void)
{
uint8_t l;

	l = hl & 0xff;
	row = (l >> 4) + 3;
	col = 3 * (l & 0x0f) + 6;
	printf("\033[%d;%dH",row,col);
}

void restore_color(void)
{
	put2Backspaces();
	printf("\033[m%02X",sector_buffer[hl]);
}

void mem_dump(uint16_t address)
{
	uint8_t a,b;
	uint16_t tmp;

	tmp = address & 0xFF00;

	printf("      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");

    do
    {
		printf("%04X ",tmp);

		b = 16;
		do
		{
			printf("%02X ",sector_buffer[tmp++]);
		} while (--b != 0);

		tmp -= 16;

		b = 16;
		do
		{
			a = sector_buffer[tmp++];

			if ((a < 0x20) || (a > 0x7F)) a = '.';
			putchar(a);
		} while (--b != 0);

		printf("\r\n");

    } while ((tmp & 0x00f0) != 0);
}

uint8_t in_b_k(uint8_t *data)
{
	uint8_t c;
	uint8_t temp;

	temp = *data;

	do
	{
		printf("%02X",temp);

		c = toupper(GetChar());

		switch(c)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				temp = (temp << 4) | (c & 0x0f);
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				temp = (temp << 4) | (c - 'A' + 0x0A);
				break;
			case '+':
				temp++;
				break;
			case '-':
				temp--;
				break;
			case 'Z':
				temp = 0;
				break;
			case '\b':
				temp >>= 4;
				break;
			case 'R': // revert bits 0..7 -> 7..0
				temp = revert_bits(temp);
				break;
			case 'S': // revert bits 0..7 -> 7..0
				temp = swap_nibble(temp);
				break;
			case '~': //neg = !(--B) = (!B)++
				temp--;
				//fallthrough
			case '!':
				temp ^= 0xFF;
				break;
			default:
				*data = temp;
				return c;
		}
		put2Backspaces();
	} while (true);
}

uint8_t UIGetAsciiDouble(uint32_t *value)
{
	uint8_t c;
	uint32_t temp;

	temp = *value;

	do
	{
		printf("%08lX",temp);

		c = toupper(GetChar());

		switch(c)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				temp = (temp << 4) | (c & 0x0f);
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				temp = (temp << 4) | (c - 'A' + 0x0A);
				break;
			case '+':
				temp++;
				break;
			case '-':
				temp--;
				break;
			case 'Z':
				temp = 0;
				break;
			case '\b':
				temp >>= 4;
				break;
			case 0x0d:
				*value = temp;
				return c;
			case 'Q':
				return c;
			default:
			;
		}
		printf("\033[1;22H");
	} while (true);
}

uint8_t swap_nibble(uint8_t in)
{
	uint8_t temp = in;

	return (temp << 4) | (temp >> 4);
}

uint8_t revert_bits(uint8_t in)
{
	uint8_t temp = 0;

	temp |= (in & 0x80) >> 7;
	temp |= (in & 0x40) >> 5;
	temp |= (in & 0x20) >> 3;
	temp |= (in & 0x10) >> 1;
	temp |= (in & 0x08) << 1;
	temp |= (in & 0x04) << 3;
	temp |= (in & 0x02) << 5;
	temp |= (in & 0x01) << 7;

	return temp;
}

void put2Backspaces(void)
{
	printf("\b\b");
}

char GetChar(void)
{
	return getch();
}

