// ____________________________
// ██▀▀█▀▀██▀▀▀▀▀▀▀█▀▀█        │    ▄▄                 ▄▄
// ██  ▀  █▄  ▀██▄ ▀ ▄█ ▄▀▀ █  │   ██ ▀ ██▄▀ ▄▀██ █ ██ ██  ▄███ ██▄▀  ██▀
// █  █ █  ▀▀  ▄█  █  █ ▀▄█ █▄ │   ▀█▄▀ ██   ▀▄██ █▀█▀ ▀█▄ ▀█▄▄ ██   ▄██
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀────────┘
//  Multiplayer snake game
//─────────────────────────────────────────────────────────────────────────────

//=============================================================================
// BIOS MODULE
//=============================================================================

#include "bios.h"

//-----------------------------------------------------------------------------
// RDSLT
// Address  : #000C
// Function : Reads the value of an address in another slot
// Input    : A  - ExxxSSPP  Slot-ID
//                 │   ││└┴─ Primary slot number (00-11)
//                 │   └┴─── Secondary slot number (00-11)
//                 └──────── Expanded slot (0 = no, 1 = yes)
//            HL - Address to read
// Output   : A  - Contains the value of the read address
// Registers: AF, C, DE
// Remark   : Can be call directly from MSX-DOS
//            This routine turns off the interupt, but won't turn it on again
u8 Bios_InterSlotRead(u8 slot, u16 addr)
{
	slot; // A
	addr; // DE

	__asm
		ld		l, e
		ld		h, d
		call	R_RDSLT
	__endasm;
}

//-----------------------------------------------------------------------------
// WRSLT
// Address  : #0014 
// Function : Writes a value to an address in another slot
// Input    : A  - ExxxSSPP  Slot-ID
//                 │   ││└┴─ Primary slot number (00-11)
//                 │   └┴─── Secondary slot number (00-11)
//                 └──────── Expanded slot (0 = no, 1 = yes)
//            HL - Address
//            E  - Value
// Registers: AF, BC, D
// Remark   : Can be call directly from MSX-DOS
//            This routine turns off the interupt, but won't turn it on again
void Bios_InterSlotWrite(u8 slot, u16 addr, u8 value)
{
	slot;  // A
	addr;  // DE
	value; // (SP+4)

	__asm
		push	iy
		ld		iy, #4
		add		iy, sp

		ld		l, e
		ld		h, d
		ld		e, 0(iy)	
		call	R_WRSLT
		pop		iy
	__endasm;
}

//=============================================================================
// SYSTEM MODULE
//=============================================================================

#include "system.h"

//-----------------------------------------------------------------------------
// Get the slot ID of a given page
u8 Sys_GetPageSlot(u8 page)
{
	// Get page's primary slot
	u8 slot = (g_PortPrimarySlot >> (page * 2)) & 0x03;
	
	// Check if slot is expended
	if(g_EXPTBL[slot] & SLOT_EXP)
	{
		u8 prevSlot = g_PortPrimarySlot; // Backup current primary slots register
		DisableInterrupt();
		g_PortPrimarySlot = (prevSlot & 0x3F) | (slot << 6); // Select primary slot in page 3
		slot |= SLOT_EXP;
		slot |= (((~g_SLTSL) >> (page * 2)) & 0x03) << 2;
		g_PortPrimarySlot = prevSlot; // Restore primary slots register
		EnableInterrupt();
	}
	return slot;
}

//-----------------------------------------------------------------------------
// Set a given slot in page 0
void Sys_SetPage0Slot(u8 slotId)
{
	slotId; // A: ExxxSSPP
			//    │   ││└┴─ Primary slot number (00-11)
			//    │   └┴─── Secondary slot number (00-11)
			//    └──────── Expanded slot (0 = no, 1 = yes)
			// C: SlotID backup
			// B: Previous Primary Slot Configuration
	__asm
		
		ld		c, a				// C=[X000SSPP]	Store input slotId 
		and		#0b10000000			// Check slotId's 'expended' flag
		jp		z, SetNonExpendedSlot

	SetExpendedSlot:
	
		// SET PRIMARY SLOT
		ld		a, c				// A=[X000SSPP]	Retreive input slotId 
		and		#0b00000011			// A=[000000PP]	Keep input primary slot
		ld		d, a				// D=[000000PP]	Store input primary slot (page 0)
		LShift(6)					// A=[PP000000] Shift primary slot (page 3)
		or		d					// A=[PP0000PP] Merge (page 0 & 3)
		ld		d, a				// D=[PP0000PP]	Store input primary slot 
		
		in		a, (P_PPI_A)		// A=[********]	Read primary slots register
		ld		b, a				// B=[********]	Store current slots configuration
		and		#0b00111100			// A=[00****00]	Resets the bits of pages 0 & 3
		or		d					// A=[PP****PP]	Merge slots register for page 1 & 2 and input primary slot for page 0 & 3
		di
		out		(P_PPI_A), a		// Select primary slot register

		// SET SECONDARY SLOT
		ld		a, c				// A=[X000SSPP]	Retreive input slotId 
		RShift(2)					// A=[00X000SS] Shift input secondary slot
		and		#0b00000011			// A=[000000SS]	Keep input secondary slot
		ld		d, a				// D=[000000SS]	Store input secondary slot (page 0)
		
		ld		a,(0xFFFF)			// A=[^^^^^^^^]	Read secondary slots register of page 3's primary slot
		cpl							// A=[********]	Reverses the bits
		and		#0b11111100			// A=[******00]	Resets the bits of page 0
		or		d					// A=[******SS]	Merge slots register with input secondary slot in page 0
		ld		(0xFFFF), a			// Select secondary slot

		// RESTORE RAM PRIMARY SLOT
		ld		a, b				// A=[********]	Retreive initial primary slots configuration
		and		#0b11000000			// A=[**000000]	Keep only the bits of page 0, 1 & 2
		ld		b, a				// B=[**000000]	Store the bits in B
		in		a, (P_PPI_A)		// A=[PP****PP]	Read primary slots register
		and		#0b00111111			// A=[00****PP]	Resets the bits of page 2
		or		b					// A=[******PP] Sets the bits of page 2 to initial value
		out		(P_PPI_A), a		// Select final primary slot register value
		
		ret
		
	SetNonExpendedSlot:

		// SET PRIMARY SLOT
		ld		a, c				// A=[X000SSPP]	Retreive input slotId 
		and		#0b00000011			// A=[000000PP]	Keep input primary slot
		ld		d, a				// D=[000000PP]	Store input primary slot 
		
		in		a, (P_PPI_A)		// A=[********]	Read primary slots register
		ld		b, a				// B=[********]	Store current slots configuration
		and		#0b11111100			// A=[******00]	Resets the bits of page 0
		or		d					// A=[******PP]	Sets the bits of page 0 to the input slot
		di
		out		(P_PPI_A), a		// Select input primary slot for the page 0

		ret
		
	__endasm;

}