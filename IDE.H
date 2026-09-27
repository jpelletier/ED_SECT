#define PPIDE_DIR_READ	92h
#define PPIDE_DIR_WRITE	80h
#define PPIDE_CTL_RESET	80h

//CTL_IDLE	00h
#define CTL_BYTE	08h
#define CTL_FEAT	0x09
#define CTL_DRHD	0Ah
#define CTL_LBA_LOW	0Bh
#define CTL_LBA_MID	0Ch
#define CTL_LBA_HIG	05h
#define CTL_MAS_LBA	06h
#define CTL_CMD		07h

#define CTL_PRELOAD 00h
#define CTL_ERROR   01h
#define CTL_STATUS	07h

#define PPIDE_CMD_NOP		000h
#define PPIDE_CMD_DEVRES	008h
#define PPIDE_CMD_RECAL		010h
#define PPIDE_CMD_READ		020h
#define PPIDE_CMD_WRITE		030h
#define PPIDE_CMD_DEVDIAG	090h
#define PPIDE_CMD_PACKET	0A0h
#define PPIDE_CMD_IDPKTDEV	0A1h
#define PPIDE_CMD_IDDEV		0ECh
#define PPIDE_CMD_SETFEAT	0EFh
//
// FEATURE BYTES
//
#define PPIDE_FEAT_ENABLE8BIT	001h
#define PPIDE_FEAT_DISABLE8BIT	081h
//
// PPIDE DEVICE TYPES
//
#define PPIDE_TYPEUNK	0
#define PPIDE_TYPEATA	1
#define PPIDE_TYPEATAPI	2
//
// PPIDE DEVICE STATUS CODES
//
#define PPIDE_STOK	0
#define PPIDE_STINVUNIT	-1
#define PPIDE_STNOMEDIA	-2
#define PPIDE_STCMDERR	-3
#define PPIDE_STIOERR	-4
#define PPIDE_STRDYTO	-5
#define PPIDE_STDRQTO	-6
#define PPIDE_STBSYTO	-7
#define PPIDE_STNOTSUP	-8
#define PPIDE_STNOTRDY	-9
//
// DRIVE SELECTION BYTES (FOR USE IN DRIVE/HEAD REGISTER)
//
//#define PPIDE_DRVSEL:
#define PPIDE_DRVMASTER	0xE0	// LBA,MASTER DEVICE
#define PPIDE_DRVSLAVE	0F0h	// LBA,SLAVE DEVICE

// =====================================================================
// PPIDE Port Mapping (Base 0244h)
// =====================================================================
#define PPIDE_DATALO 0244h    // IDE Data Bus (D0-D7)
#define PPIDE_DATAHI 0245h    // IDE Data Bus (D8-D15)
#define PPIDE_CTL    0246h    // IDE Control Signals
#define PPIDE_PPI    0247h    // 82C55 Control Register

// =====================================================================
// Port C Control Bitmask Calculations (Inverted Logic)
// Reset=0,IOR=0,IOW=0,CS1=0,CS0=0 mean pins are 5V (Inactive)
// =====================================================================
#define MSK_CS0 08h      // Pulls /CS0 Active Low (Target Command Block)
#define MSK_IOW 20h      // Pulls /IOW Active Low (Write Strobe)
#define MSK_IOR 40h      // Pulls /IOR Active Low (Read Strobe)

//------------------------------
//## Part 1: Selecting an LBA Sector
//To select a sector,we must write a 28-bit sector address across four 
//internal IDE registers: LBA Low,LBA Mid,LBA High,and Device/Head.
//This routine expects you to pass the 28-bit LBA address via the Z80 
//registers:

//* DE Lower 16 bits of LBA
//* H Upper 12 bits of LBA (Bits 0–7)
//* L Upper 12 bits of LBA (Bits 8–11) — Note: Only bits 0-3 are used
//here,mixed with the Master selection bit 0E0.

// =====================================================================
// ATA Command Set Definitions
// =====================================================================
#define CMD_SET_FEAT 0xEF  // ATA Set Features Command
#define FEAT_8BIT_ON 01h   // Sub-command feature code to enable 8-bit PIO

