//TAB=4
extern "C" {
    #include <stdint.h>
}

#define SECTOR_SIZE 512

void clearSectBuff(void);
void getDiskInfo(void);
void UpdateLbaSector(void);
void update_dump(void);
void redraw_dump(void);
void move_left(void);
void move_right(void);
void move_up(void);
void move_down(void);
void move_cursor(void);
void restore_color(void);
void mem_dump(uint16_t address);
uint8_t in_b_k(uint8_t *data);
uint8_t UIGetAsciiDouble(uint32_t *value);
uint8_t swap_nibble(uint8_t in);
uint8_t revert_bits(uint8_t in);
void putBackSpace(void);
void put2Backspaces(void);
char GetChar(void);

