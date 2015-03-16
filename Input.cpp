//**********************************************************************
//
// File: Input.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Wrapper class for using input using DirectInput 
// Date: 1-28-10
// Revision 1: 3-25-10
// Reference: DirectX Manual & Microsoft Developer's Network
//
// **********************************************************************

#include "Engine.h"

// Input class constructor
Input::Input( HWND window )
{
	// Store parent window handler
	m_window = window;

	// Create DirectInput object
	DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, ( void** )&m_di, NULL );

	// Create keyboard device
	m_di ->CreateDevice( GUID_SysKeyboard, &m_keyboard, NULL );
	
	// Prepare format for keyboard device
	m_keyboard ->SetDataFormat( &c_dfDIKeyboard );

	// Prepare cooperation level for keyboard device
	m_keyboard ->SetCooperativeLevel( m_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );

	// Acquire keyboard device
	m_keyboard ->Acquire();

	// Create mouse device
	m_di ->CreateDevice( GUID_SysMouse, &m_mouse, NULL );

	// Prepare format for mouse device
	m_mouse -> SetDataFormat( &c_dfDIMouse );

	// Prepare cooperation level for mouse device
	m_mouse ->SetCooperativeLevel( m_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );

	// Acquire mouse device
	m_mouse ->Acquire();

	// Start press stamp
	m_pressStamp = 0;

}

// Input class destructor
Input::~Input()
{

	if( m_di )
		m_di ->Release();

	m_di = NULL;

	if( m_keyboard )
		m_keyboard ->Release();

	m_keyboard = NULL;

	if( m_mouse )
		m_mouse ->Release();

	m_mouse = NULL;

/* FIX ME --------------------------------------*/ 
//	SAFE_RELEASE( m_di );
//	SAFE_RELEASE( m_keyboard );
//	SAFE_RELEASE( m_mouse );
// ---------------------------------------------------

}

// Update state for keyboard and mouse
void Input::Update()
{
	static HRESULT result;

	// Poll keyboard until it succeeds or returns an unknown error
	while( true )
	{
		m_keyboard ->Poll();

		// Retreive input data from device and store it
		if( SUCCEEDED( result = m_keyboard ->GetDeviceState( 256, ( LPVOID ) &m_keyState ) ) )
			break;

		// Check for input errors and continue polling 
		if( result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED )
			return;

		// Reacquire device if input was lost
		if( FAILED( m_keyboard ->Acquire() ) )
			return;
	}

	// Poll mouse until it succeeds or returns an unknown error
	while( true)
	{
		m_mouse ->Poll();

		// Retrieve input data from device and store it
		if( SUCCEEDED( result = m_mouse -> GetDeviceState( sizeof( DIMOUSESTATE ), &m_mouseState ) ) )
			break;

		// Check for input errors and continue polling
		if( result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED )
			return; 

		// Reacquire device if input was lost
		if( FAILED( m_mouse ->Acquire() ) )
			return;
	}

	// Get mouse position
	GetCursorPos( &m_position );
	ScreenToClient( m_window, &m_position );

	m_pressStamp++;

}

// Return true when keys are pressed
bool Input::GetKeyPress( char key, bool ignorePressStamp )
{
	// Check to see if the key was pressed twice within the same frame
	if( ( m_keyState[ key ] & 0x80 ) == false )
		return false;

	bool pressed = true;

	// Prevent repeated press stamping
	if( ignorePressStamp == false )
		if( m_keyPressStamp[ key ] == m_pressStamp -1 || m_keyPressStamp[ key ] == m_pressStamp )
			pressed = false;

	m_keyPressStamp[ key ] = m_pressStamp;

	return pressed;

}

// Return true if mouse button was pressed
bool Input::GetButtonPress( char button, bool ignorePressStamp )
{
	// Check to see if the button was pressed twice within the same frame
	if( ( m_mouseState.rgbButtons[ button ] & 0x80 ) == false )
		return false;

	bool pressed = true;

	// Prevent repeated press stamping
	if( ignorePressStamp == false )
		if( m_buttonPressStamp[ button ] == m_pressStamp -1 || m_buttonPressStamp[ button ] == m_pressStamp )
			pressed = false;

	m_buttonPressStamp[ button ] = m_pressStamp;

	return pressed;

}

// Get mouse's x position
long Input::GetPosX()
{
	return m_position.x;

}

// Get mouse's y position
long Input::GetPosY()
{
	return m_position.y;

}

// Get mouse's new x coordinate
long Input::GetDeltaX()
{
	return m_mouseState.lX;

}

// Get mouse's new y coordinate
long Input::GetDeltaY()
{
	return m_mouseState.lY;

}

// Get mouse's new scroll wheel change
long Input::GetDeltaWheel()
{
	return m_mouseState.lZ;

}