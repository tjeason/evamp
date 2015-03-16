// **********************************************************************
//
// File: Engine.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Engine interface
// Date: 1-15-10
// Revision 1: 1-21-10
// Revision 2: 1-26-10
// Revision 3: 1-28-10
// Revision 4: 2-5-10
// Revision 5: 2-11-10
// Revision 6: 2-24-10
// Revision 7: 3-4-10
// Revision 8: 3-9-10
// Revision 9: 3-11-10
// Revision 10: 3-17-10
// Revision 11: 3-23-10
// Revision 12: 3-24-10
// Revision 13: 3-25-10
// Revisoin 14: 4-21-10
//
// ***********************************************************************

// Engine file
#include "Engine.h"

// Global pointer to access engine's functionality 
Engine *g_engine = NULL;

// Handle Windows messages
LRESULT CALLBACK WindowProc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	switch( msg )
	{
		case WM_ACTIVATEAPP:		// Focus will change to or from the current application
				g_engine -> SetDeactiveFlag ( !wparam );
				return 0;

		case WM_DESTROY:			// Application destroys a window
				PostQuitMessage( 0 );	// Terminate requested thread
				return 0;

		default:
				return DefWindowProc( wnd, msg, wparam, lparam );	
	}
}

// Constructor accepts optional input, pointer to engine setup structure with the option to adjust engine settings
Engine::Engine( EngineSetup *setup )
{
	m_loaded = false;		// Engine has not been loaded
	m_setup = new EngineSetup;		// Engine setup structure is created 

	if( setup != NULL )		// If engine setup structure is passed to constructor 
		memcpy( m_setup, setup, sizeof( EngineSetup ) );		// Details are copied to replace default engine setup structure

	g_engine = this;		// Make engine global for simple access

	// Setup windows application
	WNDCLASSEX wcex;		
	wcex.cbSize						= sizeof( WNDCLASSEX );
	wcex.style							= CS_CLASSDC;
	wcex.lpfnWndProc			= WindowProc;												// Message pipeline
	wcex.cbClsExtra				= 0;
	wcex.cbWndExtra				= 0;
	wcex.hInstance					= m_setup ->instance;
	wcex.hIcon							= LoadIcon( NULL, IDI_APPLICATION );
	wcex.hCursor						= LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground		= NULL;
	wcex.lpszMenuName		= NULL;
	wcex.lpszClassName		= "WindowClass";											// Reference window class
	wcex.hIconSm					= LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx ( &wcex );																		// Register window class to create window

	// Initialize COM using multithreaded concurrency
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	// Create the Direction3D interface
	IDirect3D9 *d3d = Direct3DCreate9( D3D_SDK_VERSION );

	// Enumerate Direct3D device configurations on default adapter
	g_deviceEnumeration = new DeviceEnumeration;
	
	// Release memory for enumerated Direct3D device if the OK button is not pressed
	if( g_deviceEnumeration ->Enumerate( d3d ) != IDOK )
	{
		//  Relase memory
		if( d3d )
		{
			d3d ->Release();
			d3d = NULL;
		}
		return;
	}

	// Create window and handle it
	m_window = CreateWindow( "WindowClass", m_setup ->name, 
														g_deviceEnumeration ->IsWindowed() ? WS_OVERLAPPED : WS_POPUP, 
														0, 0, 800, 600, NULL, NULL, m_setup ->instance, NULL );

	// Prepare device presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory( &d3dpp, sizeof( D3DPRESENT_PARAMETERS ) );

	d3dpp.BackBufferWidth = g_deviceEnumeration ->GetSelectedDisplayMode() ->Width;
	d3dpp.BackBufferHeight = g_deviceEnumeration ->GetSelectedDisplayMode() ->Height;
	d3dpp.BackBufferFormat = g_deviceEnumeration ->GetSelectedDisplayMode() ->Format;
	d3dpp.BackBufferCount = m_setup ->totalBackBuffers;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_window;
	d3dpp.Windowed = g_deviceEnumeration ->IsWindowed();
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.FullScreen_RefreshRateInHz = g_deviceEnumeration ->GetSelectedDisplayMode() ->RefreshRate;

	// Check if v-sycned is enabled
	if( g_deviceEnumeration ->IsVSynced() == true )
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	// V-sync is not enabled by user
	else
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Destroy device enumeration object
	SAFE_DELETE( g_deviceEnumeration );

	// Create Direct3D device with hardware vertex processing
	if( FAILED( d3d ->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_device ) ) )
	{
		// Create Direct3D device with mixed vertex processing
		if( FAILED( d3d ->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &m_device ) ) )
		{
			// Create Direct3D device with software vertex processing
			if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_device ) ) )
				return;
		}
	}

	// Release Direct3D interface 
	if( d3d )
	{
		d3d ->Release();
		d3d = NULL;
	}

	// Switch lighting off by default
	m_device ->SetRenderState( D3DRS_LIGHTING, false );

	// Set anisotropic texture filters
	m_device ->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
	m_device ->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	m_device ->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// Set projection matrix
	D3DXMATRIX projMatrix;
	D3DXMatrixPerspectiveFovLH( &projMatrix, D3DX_PI / 4, (float)d3dpp.BackBufferWidth / (float)d3dpp.BackBufferHeight, 0.1f / m_setup->scale, 1000.0f / m_setup->scale );
	m_device ->SetTransform( D3DTS_PROJECTION, &projMatrix );

	// Store display mode details
	m_displayMode.Width = d3dpp.BackBufferWidth;
	m_displayMode.Height = d3dpp.BackBufferHeight;
	m_displayMode.RefreshRate = d3dpp.FullScreen_RefreshRateInHz;
	m_displayMode.Format = d3dpp.BackBufferFormat;

	// Swap chain always starts on back buffer 1
	m_currentBackBuffer = 0;

	// Create sprite interface
	D3DXCreateSprite( m_device, &m_sprite );

	// Create font to display frame rate
	m_fpsFont = new Font();


	// Create linked list states
	m_states = new LinkedList< State >;
	m_currentState = NULL;

	// Create resource manager
	m_scriptManager = new ResourceManager< Script >;
	m_materialManager = new ResourceManager< Material >( m_setup ->CreateMaterialResource );
	m_meshManager = new ResourceManager< Mesh >;

	 // FOR SCRIPT TESTING
//	Script *myScript = m_scriptManager ->Add( "scriptTest.txt" );

	// Create input object
	m_input = new Input( m_window );

	// Create network object
	m_network = new Network( m_setup ->guid, m_setup ->HandleNetworkMessage );

	// Create sound system
	m_soundSystem = new SoundSystem( m_setup ->scale );

	// Create scene manager
	m_sceneManager = new SceneManager( m_setup ->scale, m_setup ->spawnerPath );

	// Seed random number generator with current time
	srand( timeGetTime( ) );

	// Allow game to perform setup in current state
	if( m_setup ->StateSetup != NULL )
		m_setup ->StateSetup();

	// Engine loaded and ready
	m_loaded = true;

}

// Engine class destructor
Engine::~Engine()
{
	// Confirm engine is loaded
	if( m_loaded == true )
	{
		// Destroy states in linked list
		if( m_currentState != NULL )
			m_currentState ->Close();

		// Destry scene manager
		SAFE_DELETE( m_sceneManager );

		// Destroy all states
		SAFE_DELETE( m_states );

		// Destroy sound system
		SAFE_DELETE( m_soundSystem );

		// Destroy networks
		SAFE_DELETE( m_network );

		// Destroy all inputs
		SAFE_DELETE( m_input );

		// Destroy all meshes
		SAFE_DELETE( m_meshManager );

		// Destroy all materials 
		SAFE_DELETE( m_materialManager );

		// Destroy resources from scripts
		SAFE_DELETE( m_scriptManager );

		// Destroy font used for frame rate
		SAFE_DELETE( m_fpsFont );

		// Release sprite interface
		if( m_sprite )
		{
			m_sprite ->Release();
			m_sprite = NULL;
		}

		// Release device
		if( m_device )
		{
			m_device ->Release();
			m_device = NULL;
		}
	}

	// Uninitialize the COM
	CoUninitialize();

	// Unregister window class
	UnregisterClass( "WindowClass", m_setup ->instance );

	//  Destroy engine setup structure
	SAFE_DELETE( m_setup );

}

void Engine::Run()
{
	// Confirm engine is loaded
	if( m_loaded == true )
	{
		// Show window
		ShowWindow( m_window, SW_NORMAL );

		// Get details from game's viewer
		ViewerSetup viewer;

		// Enter message loop
		MSG msg;

		ZeroMemory( &msg, sizeof( MSG ) );

		// Game loop process messages and application frames
		while( msg.message != WM_QUIT )
		{
			// Check if message is waiting for application's window to process
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );		//  Convert message
				DispatchMessage( &msg );		//  Dispatch message to WindowsProc
			}

			// If no messages are waiting, check if application is active 
			else if( !m_deactive )
			{
				// Calculate elapsed time to process frame
				unsigned long currentTime = timeGetTime();
				static unsigned long lastTime = currentTime;
				float elapsed = ( currentTime - lastTime ) / 1000.0f;

				lastTime = currentTime;

				// Calculate frame rate
				static float frameTime = 1.0f;
				static int frameCount = 0;

				frameTime += elapsed;
				frameCount++;

				// Update fps every 1/2 sec.
				if( frameTime > 1.0f )
				{
					sprintf( m_fpsText, "FPS: %d", frameCount );

					frameTime = 0.0f;
					frameCount = 0;
				}

				// Update network object and process any pending messages
				m_network ->Update();

				// Update input object (keyboard and mouse)
				m_input ->Update();

				// Check if users wants to force exit by pressing 'F1'
				if( m_input ->GetKeyPress( DIK_F1 ) )
					PostQuitMessage( 0 );

				// Request viewer from current state
				if( m_currentState != NULL )
					m_currentState ->RequestViewer( &viewer );

				// Make sure the viewer is valid
				if( viewer.viewer != NULL )
				{

					// Update scene 
					m_sceneManager ->Update( elapsed, viewer.viewer ->GetViewMatrix() );

					// Set view transformation
					m_device ->SetTransform( D3DTS_VIEW, viewer.viewer ->GetViewMatrix() );

					// Update 3D sound
					m_soundSystem ->UpdateListener( viewer.viewer ->GetForwardVector(), viewer.viewer ->GetTranslation(), viewer.viewer ->GetVelocity() );
				}

				m_stateChanged = false;

				// Update current state
				if( m_currentState != NULL )
					m_currentState ->Update( elapsed );

				// Check if state changed
				if( m_stateChanged == true )
					continue;

				// Update current based on elapsed time for each frame
				if( m_currentState != NULL )
					m_currentState ->Update( elapsed );

				// A state change so stop further processing
				if( m_stateChanged == true )
					continue;

				// Begin scene
				m_device ->Clear( 0, NULL, viewer.viewClearFlags, 0, 1.0f, 0 );

				// Check if scene succeeded
				if( SUCCEEDED( m_device ->BeginScene() ) )
				{
					// Check if there is a valid viewer, and render the scene for it.
					if( viewer.viewer != NULL )
						m_sceneManager ->Render( elapsed, viewer.viewer ->GetTranslation() );
					// Render current state
					if( m_currentState != NULL )
						m_currentState ->Render();

					// Render frame rate
					m_fpsFont ->Render( m_fpsText, 740, 550 );

					// End scene and present it
					m_device ->EndScene();
					m_device ->Present( NULL, NULL, NULL, NULL );

					// Keep track of back buffer index
					if( ++m_currentBackBuffer == m_setup ->totalBackBuffers + 1 )
						m_currentBackBuffer = 0;
				}
			}
		}
	}

	// Destroy engine
	SAFE_DELETE( g_engine );

}

// Window handler
HWND Engine::GetWindow()
{
	return m_window;

}

// Set deactive flag
void Engine::SetDeactiveFlag( bool deactive )
{
	m_deactive = deactive;

}

// Return curent scale running in engine
float Engine::GetScale()
{
	return m_setup ->scale;

}

// Get Direct3D device
IDirect3DDevice9 *Engine::GetDevice()
{
	return m_device;

}

// Get display mode for Direct3D device
D3DDISPLAYMODE *Engine::GetDisplayMode()
{
	return &m_displayMode;

}

// Get sprite
ID3DXSprite *Engine::GetSprite()
{
	return m_sprite;

}


// Add state to engine
void Engine::AddState( State *state, bool change )
{
	m_states ->Add( state );

	// no switch in the state so move on
	if( change == false )
		return;

	// State still exists in the list, so close it
	if( m_currentState != NULL )
		m_currentState ->Close();

	m_currentState = m_states ->GetLast();
	m_currentState ->Load();

}

// Remove state from engine
void Engine::RemoveState( State *state )
{
	m_states ->Remove( &state );

}

// Engine processing switches to new state specified by state I.D.
void Engine::ChangeState( unsigned long id )
{
	m_states ->Iterate( true );

	// Iterate through list to find needed state
	while( m_states ->Iterate() != NULL )
	{
		// Find the right state I.D. from the list of states
		if( m_states ->GetCurrent() ->GetID() == id )
		{
			// Close old state
			if( m_currentState != NULL )
				m_currentState ->Close();

			// Sound system performs garbage collection
			m_soundSystem ->GarbageCollection();

			// Start new state
			m_currentState = m_states ->GetCurrent();

			// Load the new state
			m_currentState ->Load();

			// Swap back buffers until first one is front
			while( m_currentBackBuffer != 0)
			{
				m_device ->Present( NULL, NULL, NULL, NULL );

				// Keep track of buffer index
				if( ++m_currentBackBuffer == m_setup ->totalBackBuffers + 1 )
					m_currentBackBuffer = 0;
			}

			// Show that a new state has been loaded
			m_stateChanged = true;

			break;
		}
	}

}

// Return pointer to current state
State *Engine::GetCurrentState()
{
	return m_currentState;

}

// Return pointer to script manager
ResourceManager< Script > *Engine::GetScriptManager()
{
	return m_scriptManager;

}

// Return pointer to material manager
ResourceManager< Material > *Engine::GetMaterialManager()
{
	return m_materialManager;

}

// Return pointer to mesh manager
ResourceManager< Mesh > *Engine::GetMeshManager()
{
	return m_meshManager;

}

// Return pointer to input object
Input *Engine::GetInput()
{
	return m_input;

}

// Return pointer to network object
Network *Engine::GetNetwork()
{
	return m_network;

}

// Return pointer to sound system
SoundSystem *Engine::GetSoundSystem()
{
	return m_soundSystem;

}

// Return pointer to scene manager
SceneManager *Engine::GetSceneManager()
{
	return m_sceneManager;

}

