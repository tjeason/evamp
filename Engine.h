// ***********************************************************************
//
// File: Engine.h
// Programmer: T.J. Eason
// Project: Game Engine 
// Description: Links entire engine together
// Date: 1-15-10
// Revision 1: 1-21-10
// Revision 2: 1-26-10
// Revision 3: 1-28-10
// Revision 4: 2-5-10
// Revision 5: 2-8-10
// Revision 6: 2-11-10
// Revision 7: 2-24-10
// Revision 8: 3-4-10
// Revision 9: 3-9-10
// Revision 10: 3-11-10
// Revision 11: 3-16-10
// Revision 12: 3-17-10
// Revision 13: 3-18-10
// Revision 14: 3-23-10
// Revision 15: 3-24-10
// Revision 16: 3-25-10
// Revision 17: 4-2-10
// Revision 18: 4-6-10
// Revision 19: 4-20-10
// Revision 20: 4-21-10
//
// ***********************************************************************

#ifndef ENGINE_H	
#define ENGINE_H

// DirectInput version
#define DIRECTINPUT_VERSION 0x0800

// System files
#include <stdio.h>					// Standard input/output
#include <string.h>				// Handle strings
#include <tchar.h>					// Text string datatype
#include <windowsx.h>        // Generate window

// Direct X files
#include <d3dx9.h>				// D3DX Libraray
#include <dinput.h>				// DirectInput interfaces used by keyboard/mouse
#include <dplay8.h>				// DirectPlay interfaces for networking
#include <dmusici.h>			// DirectMusic interfaces for sound playback

// Macros to free memory
#define SAFE_DELETE( p )					{ if( p )		{ delete ( p );			( p ) = NULL; }}	// For non-arrays
#define SAFE_DELETE_ARRAY( p )	{ if( p )		{ delete[ ] ( p );		( p ) = NULL; }}	// For arrays

/* FIX ME */
//#define SAFE_RELEASE ( p )				{ if( p )     { ( p) ->Release (); ( p ) = NULL; }}  // For DirectX COM/objects 

// Engine files
#include "Resource.h"
#include "LinkedList.h"
#include "ResourceManagement.h"
#include "Geometry.h"
#include "Font.h"
#include "Scripting.h"
#include "DeviceEnumeration.h"
#include "Input.h"
#include "Network.h"
#include "SoundSystem.h"
#include "BoundVolume.h"
#include "Material.h"
#include "Mesh.h"
#include "SceneObject.h"
#include "AnimatedObject.h"
#include "SpawnerObject.h"
#include "ViewFrustrum.h"
#include "RenderCache.h"
#include "SceneManager.h"
#include "CollisionDetection.h"
#include "State.h"

// Engine setup structure
struct EngineSetup
{
	HINSTANCE instance;																											// Handle game's instance
	GUID guid;																																// Game's guid
	char *name;																															// Game's name
	float scale;																																// Unit scale in meters/unit
	unsigned char totalBackBuffers;																							// Number of back buffers used
	void ( *HandleNetworkMessage ) ( ReceivedMessage *msg );										// Network messege handler
	void ( *StateSetup ) ();																											// State setup function
	void ( *CreateMaterialResource ) ( Material **resource, char *name, char *path );		// Material resource creation
	char *spawnerPath;																												// Locates the path for spawner object scripts

	// Engine setup constructor
	EngineSetup()
	{
		GUID defaultGUID = { 0x24215591, 0x24c0, 0x4316, { 0xb5, 0xb2, 0x67, 0x98, 0x2c, 0xb3, 0x82, 0x54 } };

		instance = NULL;
		guid = defaultGUID;
		name = "Application";
		scale = 1.0f;
		totalBackBuffers = 1;
		HandleNetworkMessage = NULL;
		StateSetup = NULL;
		CreateMaterialResource = NULL;
		spawnerPath ="./";
	}

};

// Engine class
class Engine
{
	public:
		Engine( EngineSetup *setup = NULL );
		virtual ~Engine( );

		void Run( );

		HWND GetWindow( );
		void SetDeactiveFlag( bool deactive );

		float GetScale();
		IDirect3DDevice9 *GetDevice();
		D3DDISPLAYMODE *GetDisplayMode();
		ID3DXSprite *GetSprite();

		void AddState( State *state, bool change = true );
		void RemoveState( State *state );
		void ChangeState( unsigned long id );

		State *GetCurrentState();

		ResourceManager< Script > *GetScriptManager();
		ResourceManager< Material > *GetMaterialManager();
		ResourceManager< Mesh > *GetMeshManager();

		Input *GetInput();
		Network *GetNetwork();
		SoundSystem *GetSoundSystem();
		SceneManager *GetSceneManager();

	private:

		char m_fpsText[16];																// Frame rate character string
		Font *m_fpsFont;																	// Font for rendering frame rate

		bool m_loaded;																		// Checks if engine is loading
		HWND m_window;																// Handle main window
		bool m_deactive;																	// Checks if applicaiton is active

		EngineSetup *m_setup;														// Copy of engine setup structure

		IDirect3DDevice9 *m_device;												// Direct3D device interface
		D3DDISPLAYMODE m_displayMode;								// Current display mode for Direct3D device
		ID3DXSprite *m_sprite;														// Sprite interface for rendering 2D texture primitives
		unsigned char m_currentBackBuffer;									// Keeps track of which back buffer is in front of swap chain

		LinkedList< State > *m_states;											// Linked list of states
		State *m_currentState;															// Pointer to current state
		bool m_stateChanged;															// Determine if state changed in current frame

		ResourceManager< Script > *m_scriptManager;				// Script manager
		ResourceManager< Material > *m_materialManager;     // Material manager
		ResourceManager< Mesh > *m_meshManager;				// Mesh manager

		Input *m_input;																		// Input object
		Network *m_network;															// Network object
		SoundSystem *m_soundSystem;										// Sound system object
		SceneManager *m_sceneManager;									// Scene manager object
};

// Have global access to other header and source files
extern Engine *g_engine;

#endif
