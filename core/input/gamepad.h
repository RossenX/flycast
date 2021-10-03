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

	DC_BTN_GROUP_MASK   = 0xF000000,

	EMU_BUTTONS         = 0x3000000,
	EMU_BTN_MENU,
	EMU_BTN_FFORWARD,
	EMU_BTN_ESCAPE,

	// Real axes
	DC_AXIS_TRIGGERS	= 0x1000000,
	DC_AXIS_LT,
	DC_AXIS_RT,
	DC_AXIS_STICKS		= 0x2000000,
	DC_AXIS_LEFT,
	DC_AXIS_RIGHT,
	DC_AXIS_UP,
	DC_AXIS_DOWN,
	DC_AXIS2_LEFT,
	DC_AXIS2_RIGHT,
	DC_AXIS2_UP,
	DC_AXIS2_DOWN,

	// Macros

	BC_03 = (DC_BTN_A | DC_BTN_X), 							// 1+4 '
	BC_14 = (DC_BTN_B | DC_BTN_Y),							// 2+5 '
	BC_25 = (DC_BTN_C | DC_BTN_Z),							// 3+6 '
	BC_01 = (DC_BTN_A | DC_BTN_B),                          // 1+2 '
	BC_12 = (DC_BTN_B | DC_BTN_C),							// 2+3 '
	BC_34 = (DC_BTN_X | DC_BTN_Y),							// 4+5 '
	BC_45 = (DC_BTN_Y | DC_BTN_Z),							// 5+6 '
	BC_012 = (DC_BTN_A | DC_BTN_B | DC_BTN_C),				// 1+2+3 '
	BC_345 = (DC_BTN_X | DC_BTN_Y | DC_BTN_Z),				// 4+5+6 '

	BC_XA = (DC_BTN_X | DC_BTN_A), 							// X+A
	BC_YB = (DC_BTN_Y | DC_BTN_B),							// Y+B
	BC_LR = (DC_BTN_Z | DC_BTN_C),							// L(Z)+R
	BC_XY = (DC_BTN_X | DC_BTN_Y),                          // X+Y
	BC_YL = (DC_BTN_Y | DC_BTN_Z),							// Y+L(Z)
	BC_AB = (DC_BTN_A | DC_BTN_B),							// A+B
	BC_BR = (DC_BTN_B | DC_BTN_C),							// B+R(C)
	BC_XYL = (DC_BTN_X | DC_BTN_Y | DC_BTN_Z),				// X+Y+L(Z)
	BC_ABR = (DC_BTN_A | DC_BTN_B | DC_BTN_C),				// A+B+R(C)

	// Kept for legacy conversion mostly, not really used <Deprecated Kept for Converting Old input codeo only>
	AXIS_BC_03 = (DC_BTN_A | DC_BTN_X), 						// 1+4 '
	AXIS_BC_14 = (DC_BTN_B | DC_BTN_Y),							// 2+5 '
	AXIS_BC_25 = (DC_BTN_C | DC_BTN_Z),							// 3+6 '
	AXIS_BC_01 = (DC_BTN_A | DC_BTN_B),                         // 1+2 '
	AXIS_BC_12 = (DC_BTN_B | DC_BTN_C),							// 2+3 '
	AXIS_BC_34 = (DC_BTN_X | DC_BTN_Y),							// 4+5 '
	AXIS_BC_45 = (DC_BTN_Y | DC_BTN_Z),							// 5+6 '
	AXIS_BC_012 = (DC_BTN_A | DC_BTN_B | DC_BTN_C),				// 1+2+3 '
	AXIS_BC_345 = (DC_BTN_X | DC_BTN_Y | DC_BTN_Z),				// 4+5+6 '

	AXIS_BC_XA = (DC_BTN_X | DC_BTN_A), 						// X+A
	AXIS_BC_YB = (DC_BTN_Y | DC_BTN_B),							// Y+B
	AXIS_BC_LR = (DC_BTN_Z | DC_BTN_C),							// L(Z)+R
	AXIS_BC_XY = (DC_BTN_X | DC_BTN_Y),                         // X+Y
	AXIS_BC_YL = (DC_BTN_Y | DC_BTN_Z),							// Y+L(Z)
	AXIS_BC_AB = (DC_BTN_A | DC_BTN_B),							// A+B
	AXIS_BC_BR = (DC_BTN_B | DC_BTN_C),							// B+R(C)
	AXIS_BC_XYL = (DC_BTN_X | DC_BTN_Y | DC_BTN_Z),				// X+Y+L(Z)
	AXIS_BC_ABR = (DC_BTN_A | DC_BTN_B | DC_BTN_C),				// A+B+R(C)

	// System axes
	EMU_AXIS_NONE        = 0,
};
