﻿//========= Copyright Š 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef KEYCODE_H
#define KEYCODE_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/vgui.h>

namespace vgui
{

	//-----------------------------------------------------------------------------
	// Purpose: this is a map for virtual key codes
	//			virtual key codes may exist outside this range for other languages
	//-----------------------------------------------------------------------------
	enum KeyCode
	{
		BUTTON_CODE_INVALID = -1,
		BUTTON_CODE_NONE = 0,

		KEY_FIRST = 0,

		KEY_NONE = KEY_FIRST,
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		KEY_PAD_0,
		KEY_PAD_1,
		KEY_PAD_2,
		KEY_PAD_3,
		KEY_PAD_4,
		KEY_PAD_5,
		KEY_PAD_6,
		KEY_PAD_7,
		KEY_PAD_8,
		KEY_PAD_9,
		KEY_PAD_DIVIDE,
		KEY_PAD_MULTIPLY,
		KEY_PAD_MINUS,
		KEY_PAD_PLUS,
		KEY_PAD_ENTER,
		KEY_PAD_DECIMAL,
		KEY_LBRACKET,
		KEY_RBRACKET,
		KEY_SEMICOLON,
		KEY_APOSTROPHE,
		KEY_BACKQUOTE,
		KEY_COMMA,
		KEY_PERIOD,
		KEY_SLASH,
		KEY_BACKSLASH,
		KEY_MINUS,
		KEY_EQUAL,
		KEY_ENTER,
		KEY_SPACE,
		KEY_BACKSPACE,
		KEY_TAB,
		KEY_CAPSLOCK,
		KEY_NUMLOCK,
		KEY_ESCAPE,
		KEY_SCROLLLOCK,
		KEY_INSERT,
		KEY_DELETE,
		KEY_HOME,
		KEY_END,
		KEY_PAGEUP,
		KEY_PAGEDOWN,
		KEY_BREAK,
		KEY_LSHIFT,
		KEY_RSHIFT,
		KEY_LALT,
		KEY_RALT,
		KEY_LCONTROL,
		KEY_RCONTROL,
		KEY_LWIN,
		KEY_RWIN,
		KEY_APP,
		KEY_UP,
		KEY_LEFT,
		KEY_DOWN,
		KEY_RIGHT,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_CAPSLOCKTOGGLE,
		KEY_NUMLOCKTOGGLE,
		KEY_SCROLLLOCKTOGGLE,

		KEY_XBUTTON_UP,
		KEY_XBUTTON_DOWN,
		KEY_XBUTTON_LEFT,
		KEY_XBUTTON_RIGHT,
		KEY_XBUTTON_START,
		KEY_XBUTTON_BACK,
		KEY_XBUTTON_STICK1,
		KEY_XBUTTON_STICK2,
		KEY_XBUTTON_A,
		KEY_XBUTTON_B,
		KEY_XBUTTON_X,
		KEY_XBUTTON_Y,
		KEY_XBUTTON_BLACK,
		KEY_XBUTTON_WHITE,
		KEY_XBUTTON_LTRIGGER,
		KEY_XBUTTON_RTRIGGER,
		KEY_XSTICK1_UP,
		KEY_XSTICK1_DOWN,
		KEY_XSTICK1_LEFT,
		KEY_XSTICK1_RIGHT,
		KEY_XSTICK2_UP,
		KEY_XSTICK2_DOWN,
		KEY_XSTICK2_LEFT,
		KEY_XSTICK2_RIGHT,

		KEY_LAST,
	};

	inline bool IsAlpha(KeyCode code)
	{
		return (code >= KEY_A) && (code <= KEY_Z);
	}

	inline bool IsAlphaNumeric(KeyCode code)
	{
		return (code >= KEY_0) && (code <= KEY_Z);
	}

	inline bool IsSpace(KeyCode code)
	{
		return (code == KEY_ENTER) || (code == KEY_TAB) || (code == KEY_SPACE);
	}

	inline bool IsKeypad(KeyCode code)
	{
		return (code >= KEY_PAD_0) && (code <= KEY_PAD_DECIMAL);
	}

	inline bool IsPunctuation(KeyCode code)
	{
		return (code >= KEY_0) && (code <= KEY_SPACE) && !IsAlphaNumeric(code) && !IsSpace(code) && !IsKeypad(code);
	}

	inline bool IsKeyCode(KeyCode code)
	{
		return (code >= KEY_FIRST) && (code <= KEY_LAST);
	}

}
#endif // KEYCODE_H