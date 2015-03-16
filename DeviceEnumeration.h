//****************************************************************************
//
// File: DeviceEnumeration.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description:   Enumerates Direct3D devices. Allows users to 
// select supported device configurations
// Date: 2-9-10
// Revision 1: 2-10-20
// Revision 2: 3-24-10
// Revision 3: 3-25-10
// Reference: Microsoft Developer's Network & Direct X Manual
//
// ****************************************************************************

#ifndef DEVICE_ENUMERATION_H
#define DEVICE_ENUMERATION_H


// Display Mode Structure
struct DisplayMode
{
	D3DDISPLAYMODE mode; // Direct3D display mode.
	char bpp[6]; // Color depth expressed as a character string for display.
};


// Device Enumeration Class
class DeviceEnumeration
{
public:
	INT_PTR Enumerate( IDirect3D9 *d3d );

	INT_PTR SettingsDialogProc( HWND dialog, UINT uiMsg, WPARAM wParam, LPARAM lParam );

	D3DDISPLAYMODE *GetSelectedDisplayMode();
	bool IsWindowed();
	bool IsVSynced();

private:
	void ComboBoxAdd( HWND dialog, int id, void *data, char *desc );
	void ComboBoxSelect( HWND dialog, int id, int index );
	void ComboBoxSelect( HWND dialog, int id, void *data );
	void *ComboBoxSelected( HWND dialog, int id );
	bool ComboBoxSomethingSelected( HWND dialog, int id );
	int ComboBoxCount( HWND dialog, int id );
	bool ComboBoxContainsText( HWND dialog, int id, char *text );

private:
	Script *m_settingsScript;											// Script which stores the device configuration.

	D3DADAPTER_IDENTIFIER9 m_adapter;			// Direct3D adapter identifier.
	LinkedList< DisplayMode > *m_displayModes;	// Linked list of enumerated display modes.
	D3DDISPLAYMODE m_selectedDisplayMode;	// User selected display mode.
	bool m_windowed;													// Indicates if the application should run in windowed mode.
	bool m_vsync;															// Inidicates if v-sync should be enabled.
};


// Externals
extern DeviceEnumeration *g_deviceEnumeration;

#endif