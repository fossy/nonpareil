/*
Copyright 1995, 2003-2009, 2022 Eric Smith <spacewar@gmail.com>

Nonpareil is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.  Note that I am not
granting permission to redistribute or modify Nonpareil under the
terms of any later version of the General Public License.

Nonpareil is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (in the file "COPYING"); if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111, USA.
*/


#define WSIZE 14
#define EXPSIZE 3  // two exponent and one exponent sign digit

typedef digit_t reg_t [WSIZE];


#define SSIZE 16
#define STACK_SIZE 2

#define PAGE_SIZE 1024
#define MAX_PAGE 4
#define MAX_BANK 2


typedef uint16_t rom_addr_t;


// Architecture variants:

#define AV_P_WRAP_FUNNY  0x01   // pointer compares funny after wrapping
                                // off for ACT 1820-1396, 1820-1523, 1820-1741
                                // on  for ACT 1820-1596, 1820-1812, 1820-2028,
                                //             1820-2530
                                // unknown for NMOS ACT 5061-0428
                                // unknown for Spice series processors
                                //      1820-2105, 1820-2122, 1820-2162-A

#define AV_BANK_USING_S0 0x02


typedef enum
{
  // External flag outputs
  EXT_FLAG_ACT_F0,          // ACT pin 22, controlled by flag s0

  // External flag inputs
  EXT_FLAG_ACT_F1,          // ACT pin 3, affects flag s5
  EXT_FLAG_ACT_F1_COND_S0,  //   F1, but only while S0 is true
  EXT_FLAG_ACT_F1_PULSE,    //   F1, but pulsed for a single cycle

  EXT_FLAG_ACT_F2,          // ACT pin 4, affects flag s3
  EXT_FLAG_ACT_F2_COND_S0,  //   F2, but only while S0 is true
  EXT_FLAG_ACT_F2_PULSE,    //   F2, but pulsed for a single cycle

  // EXT_FLAG_ACT_KA through _KE should be consecutive
  EXT_FLAG_ACT_KA,          // ACT pin 5
  EXT_FLAG_ACT_KB,          // ACT pin 6
  EXT_FLAG_ACT_KC,          // ACT pin 7
  EXT_FLAG_ACT_KD,          // ACT pin 8
  EXT_FLAG_ACT_KE,          // ACT pin 9

  // EXT_FLAG_ACT_KA_COND_S0 through _KE_ should be consecutive
  EXT_FLAG_ACT_KA_COND_S0,  //   KA, but only while S0 is true
  EXT_FLAG_ACT_KB_COND_S0,  //   KB, but only while S0 is true
  EXT_FLAG_ACT_KC_COND_S0,  //   KC, but only while S0 is true
  EXT_FLAG_ACT_KD_COND_S0,  //   KD, but only while S0 is true
  EXT_FLAG_ACT_KE_COND_S0,  //   KE, but only while S0 is true

  EXT_FLAG_SIZE             // not an actual flag
} ext_flag_num_t;


typedef struct
{
  uint32_t arch_variant;

  reg_t a;
  reg_t b;
  reg_t c;
  reg_t y;
  reg_t z;
  reg_t t;
  reg_t m1;
  reg_t m2;

  digit_t f;

  digit_t p;  // 0 to 13 only
  int p_change [3];  // p inc/dec values for last three cycles
                     // -1, 0, or +1

  bool decimal;

  bool carry, prev_carry;

  bool s [SSIZE];                 // ACT flags (status bits)
  bool ext_flag [EXT_FLAG_SIZE];  // external flags, cause status or CRC
                                     // bits to get set

  uint16_t pc;

  bool del_rom_flag;
  uint8_t del_rom;

  inst_state_t inst_state;

  int sp;  /* stack pointer */
  uint16_t stack [STACK_SIZE];

  int prev_pc;  /* used to store complete five-digit octal address of instruction */

  int crc;

  // ACT keyboard scanner

  bool key_scanner_as_flags;   // true if keyboards scanner is used as flags instead of keyboard (19C, 91, 92, 95C)
  uint8_t key_scanner_inputs;
  uint8_t key_scanner_cond_s0_inputs;

  bool key_flag;      /* true if a key is down */
  int key_buf;        /* most recently pressed key */

  // display

  bool display_enable;
  bool display_14_digit;  // true after RESET TWF instruction

  int left_scan;
  int right_scan;
  int display_scan_position;   /* word index, left_scan down to right_scan */
  int display_digit_position;  /* character index, 0 to MAX_DIGIT_POSITION-1 */

  void (* display_scan_fn) (sim_t *sim);

  void (* op_fcn [1024])(struct sim_t *sim, int opcode);

  // ROM:
  int bank_exists [MAX_PAGE];  // bitmap
  bool bank_using_s0;          // if true, use s0 as bank select rather than bank switch instruction (19C, 95C)
  bool bank;                       // only a single global bank bit
  rom_word_t *rom;
  bool *rom_exists;
  bool *rom_breakpoint;

  // RAM:
  addr_t max_ram;  // highest existing ram address + 1
  addr_t ram_addr;  /* selected RAM address */
  bool *ram_exists;
  reg_t *ram;

  bool (* ram_rd_fcn [256])(struct sim_t *sim);
  bool (* ram_wr_fcn [256])(struct sim_t *sim);

  // chips:
  chip_t *act_chip;
  chip_t *crc_chip;
  chip_t *pick_chip;
} act_reg_t;


// defined in dis_woodstock.c:
bool woodstock_disassemble (sim_t        *sim,
			    uint32_t     flags,
			    // input and output:
			    bank_t       *bank,
			    addr_t       *addr,
			    inst_state_t *inst_state,
			    bool         *carry_known_clear,
			    addr_t       *delayed_select_mask,
			    addr_t       *delayed_select_addr,
			    // output:
			    flow_type_t  *flow_type,
			    bank_t       *target_bank,
			    addr_t       *target_addr,
			    char         *buf,
			    int          len);

enum
{
  event_flag_out_change = first_arch_event,
};
