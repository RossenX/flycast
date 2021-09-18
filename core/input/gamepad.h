/*
	This file is part of reicast.

    reicast is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    reicast is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with reicast.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

enum DreamcastKey
{
	// Real buttons
	DC_BTN_C           = 1 << 0,
	DC_BTN_B           = 1 << 1,
	DC_BTN_A           = 1 << 2,
	DC_BTN_START       = 1 << 3,
	DC_DPAD_UP         = 1 << 4,
	DC_DPAD_DOWN       = 1 << 5,
	DC_DPAD_LEFT       = 1 << 6,
	DC_DPAD_RIGHT      = 1 << 7,
	DC_BTN_Z           = 1 << 8,
	DC_BTN_Y           = 1 << 9,
	DC_BTN_X           = 1 << 10,
	DC_BTN_D           = 1 << 11,
	DC_DPAD2_UP        = 1 << 12,
	DC_DPAD2_DOWN      = 1 << 13,
	DC_DPAD2_LEFT      = 1 << 14,
	DC_DPAD2_RIGHT     = 1 << 15,
	DC_BTN_RELOAD      = 1 << 16,	// Not a real button but handled like one

	// System buttons
	EMU_BTN_NONE			= 0,
	EMU_BTN_TRIGGER_LEFT	= 1 << 17,
	EMU_BTN_TRIGGER_RIGHT	= 1 << 18,
	EMU_BTN_MENU			= 1 << 19,
	EMU_BTN_FFORWARD		= 1 << 20,
	EMU_BTN_ANA_UP			= 1 << 21,
	EMU_BTN_ANA_DOWN		= 1 << 22,
	EMU_BTN_ANA_LEFT		= 1 << 23,
	EMU_BTN_ANA_RIGHT		= 1 << 24,
	EMU_BTN_ESCAPE			= 1 << 25,

	// Real axes
	DC_AXIS_LT		 = 0x10000,
	DC_AXIS_RT		 = 0x10001,
	DC_AXIS_X        = 0x20000,
	DC_AXIS_Y        = 0x20001,
	DC_AXIS_X2		 = 0x20002,
	DC_AXIS_Y2		 = 0x20003,

	// System axes
	EMU_AXIS_NONE        = 0x00000,
	EMU_AXIS_DPAD1_X     = DC_DPAD_LEFT,
	EMU_AXIS_DPAD1_Y     = DC_DPAD_UP,
	EMU_AXIS_DPAD2_X     = DC_DPAD2_LEFT,
	EMU_AXIS_DPAD2_Y     = DC_DPAD2_UP,
	EMU_AXIS_BTN_A       = 0x40000 | DC_BTN_A,
	EMU_AXIS_BTN_B       = 0x40000 | DC_BTN_B,
	EMU_AXIS_BTN_C       = 0x40000 | DC_BTN_C,
	EMU_AXIS_BTN_D       = 0x40000 | DC_BTN_D,
	EMU_AXIS_BTN_X       = 0x40000 | DC_BTN_X,
	EMU_AXIS_BTN_Y       = 0x40000 | DC_BTN_Y,
	EMU_AXIS_BTN_Z       = 0x40000 | DC_BTN_Z,
	EMU_AXIS_BTN_START   = 0x40000 | DC_BTN_START,
	EMU_AXIS_DPAD_UP     = 0x40000 | DC_DPAD_UP,
	EMU_AXIS_DPAD_DOWN   = 0x40000 | DC_DPAD_DOWN,
	EMU_AXIS_DPAD_LEFT   = 0x40000 | DC_DPAD_LEFT,
	EMU_AXIS_DPAD_RIGHT  = 0x40000 | DC_DPAD_RIGHT,
	EMU_AXIS_DPAD2_UP    = 0x40000 | DC_DPAD2_UP,
	EMU_AXIS_DPAD2_DOWN  = 0x40000 | DC_DPAD2_DOWN,
	EMU_AXIS_DPAD2_LEFT  = 0x40000 | DC_DPAD2_LEFT,
	EMU_AXIS_DPAD2_RIGHT = 0x40000 | DC_DPAD2_RIGHT,

	// Dreamcast To Naomi Names +1
	// DC_BTN_A = 1
	// DC_BTN_B = 2
	// DC_BTN_X = 3
	// DC_BTN_Y = 4
	// DC_DPAD2_UP = 5
	// DC_DPAD2_DOWN = 6
	// Macros Naomi
	
	BC_03 = (DC_BTN_A | DC_BTN_Y), 							// 1+4
	BC_14 = (DC_BTN_B | DC_DPAD2_UP),						// 2+5
	BC_25 = (DC_BTN_X | DC_DPAD2_DOWN),						// 3+6
	BC_01 = (DC_BTN_A | DC_BTN_B),                          // 1+2
	BC_12 = (DC_BTN_B | DC_BTN_X),							// 2+3
	BC_34 = (DC_BTN_Y | DC_DPAD2_UP),						// 4+5
	BC_45 = (DC_DPAD2_UP | DC_DPAD2_DOWN),					// 5+6
	BC_012 = (DC_BTN_A | DC_BTN_B | DC_BTN_X),				// 1+2+3
	BC_345 = (DC_BTN_Y | DC_DPAD2_UP | DC_DPAD2_DOWN),		// 4+5+6

	BC_XA = (DC_BTN_X | DC_BTN_A), 							// X+A
	BC_YB = (DC_BTN_Y | DC_BTN_B),							// Y+B
	BC_LR = (DC_BTN_Z | DC_BTN_C),							// L(Z)+R
	BC_XY = (DC_BTN_X | DC_BTN_Y),                          // X+Y
	BC_YL = (DC_BTN_Y | DC_BTN_Z),							// Y+L(Z)
	BC_AB = (DC_BTN_A | DC_BTN_B),							// A+B
	BC_BR = (DC_BTN_B | DC_BTN_C),							// B+R(C)
	BC_XYL = (DC_BTN_X | DC_BTN_Y | DC_BTN_Z),				// X+Y+L(Z)
	BC_ABR = (DC_BTN_A | DC_BTN_B | DC_BTN_C),				// A+B+R(C)

	AXIS_BC_03 = 0x40000 | (DC_BTN_A | DC_BTN_Y), 							// 1+4
	AXIS_BC_14 = 0x40000 | (DC_BTN_B | DC_DPAD2_UP),						// 2+5
	AXIS_BC_25 = 0x40000 | (DC_BTN_X | DC_DPAD2_DOWN),						// 3+6
	AXIS_BC_01 = 0x40000 | (DC_BTN_A | DC_BTN_B),                          // 1+2
	AXIS_BC_12 = 0x40000 | (DC_BTN_B | DC_BTN_X),							// 2+3
	AXIS_BC_34 = 0x40000 | (DC_BTN_Y | DC_DPAD2_UP),						// 4+5
	AXIS_BC_45 = 0x40000 | (DC_DPAD2_UP | DC_DPAD2_DOWN),					// 5+6
	AXIS_BC_012 = 0x40000 | (DC_BTN_A | DC_BTN_B | DC_BTN_X),				// 1+2+3
	AXIS_BC_345 = 0x40000 | (DC_BTN_Y | DC_DPAD2_UP | DC_DPAD2_DOWN),		// 4+5+6

	AXIS_BC_XA = 0x40000 | (DC_BTN_X | DC_BTN_A), 							// X+A
	AXIS_BC_YB = 0x40000 | (DC_BTN_Y | DC_BTN_B),							// Y+B
	AXIS_BC_LR = 0x40000 | (DC_BTN_Z | DC_BTN_C),							// L(Z)+R
	AXIS_BC_XY = 0x40000 | (DC_BTN_X | DC_BTN_Y),                          // X+Y
	AXIS_BC_YL = 0x40000 | (DC_BTN_Y | DC_BTN_Z),							// Y+L(Z)
	AXIS_BC_AB = 0x40000 | (DC_BTN_A | DC_BTN_B),							// A+B
	AXIS_BC_BR = 0x40000 | (DC_BTN_B | DC_BTN_C),							// B+R(C)
	AXIS_BC_XYL = 0x40000 | (DC_BTN_X | DC_BTN_Y | DC_BTN_Z),				// X+Y+L(Z)
	AXIS_BC_ABR = 0x40000 | (DC_BTN_A | DC_BTN_B | DC_BTN_C),				// A+B+R(C)

};
