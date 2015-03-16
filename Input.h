//**********************************************************************
//
// File: Input.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Wrapper class for using input using DirectInput 
// Date: 1-28-10
// Revision 1: 3-25-10
// Reference: DirectX Manual & Microsoft Developer's Network
//
// **********************************************************************

#ifndef INPUT_H
#define INPUT_H

// Input class
class Input
{
public:
	Input( HWND window );

	virtual ~Input();

	void Update();

	bool GetKeyPress( char key, bool ignorePressStamp = false );
	bool GetButtonPress( char button, bool ignorePressStamp = false );

	long GetPosX();
	long GetPosY();
	long GetDeltaX();
	long GetDeltaY();
	long GetDeltaWheel();

private:
	HWND m_window;										// Handle parent window
	IDirectInput8 *m_di;										// DirectInput object
	unsigned long m_pressStamp;					// Current press stamp

	IDirectInputDevice8 *m_keyboard;				// DirectInput for keyboard
	char m_keyState[256];									// Stores states for all key board keys
	unsigned long m_keyPressStamp[256];	// Stamp from key previously pressed for each frame

	IDirectInputDevice8 *m_mouse;					// DirectInput for mouse
	DIMOUSESTATE m_mouseState;				// Store states for mouse buttons
	unsigned long m_buttonPressStamp[3];	// Stamp from button previously pressed for each frame
	POINT m_position;										// Store position of cursor on the screen

};

#endif
