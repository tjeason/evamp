// ***********************************************************************
//
// File: Network.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description: DirectPlay wrapper class to handle system network messages
// Reference: DirectX Manual & GameDev.net
// Date: 3-3-10
// Revision 1: 3-4-10
// Revision: 2: 3-9-10
// Revision 3: 3-10-10
// Revision 4: 3-24-10
// Revision 5: 3-25-10
//
// ***********************************************************************

#include "Engine.h"

// Network class constructor
Network::Network( GUID guid, void ( *HandleNetworkMessageFunction ) ( ReceivedMessage *msg ) )
{
	// Initiallize crtical section
	InitializeCriticalSection( &m_sessionCS );
	InitializeCriticalSection( &m_playerCS );
	InitializeCriticalSection( &m_messageCS );

	// Invalidate DirectPlay peer interface and devce
	m_dpp = NULL;
	m_device = NULL;

	// Store game's GUID
	memcpy( &m_guid, &guid, sizeof( GUID ) );

	// Create new session list
	m_sessions = new LinkedList< SessionInfo >;

	// Create new player list
	m_players = new LinkedList< PlayerInfo >;

	// Create new network message list
	m_messages = new LinkedList<ReceivedMessage >;

	// Load network settings
	Script *settings = new Script( "NetworkSettings.txt" );

	// Check default network settings
	if( settings ->GetNumberData( "processing_time" ) == NULL )
	{
		m_port = 2509;
		m_sendTimeOut = 100;
		m_processingTime = 100;
	}

	// Retrieve local network settings
	else
	{
		m_port = *settings ->GetNumberData( "port" );
		m_sendTimeOut = *settings ->GetNumberData( "send_time_out" );
		m_processingTime = *settings ->GetNumberData( "processing_time" );
	}

	// Delete Network settings
	SAFE_DELETE( settings );

	// Network not allowed to receive messages
	m_receiveAllowed = false;

	// Set network message handler
	HandleNetworkMessage = HandleNetworkMessageFunction;

	// Create DirectPlay peer interface
	CoCreateInstance( CLSID_DirectPlay8Peer, NULL, CLSCTX_INPROC, IID_IDirectPlay8Peer, ( void** ) &m_dpp );

	// Initialize peer interface
	m_dpp ->Initialize( ( PVOID ) this,  NetworkMessageHandler, DPNINITIALIZE_HINT_LANSESSION );

	// Create device address
	CoCreateInstance( CLSID_DirectPlay8Address, NULL, CLSCTX_INPROC, IID_IDirectPlay8Address, ( LPVOID* ) &m_device );

	// Set up the device address
	m_device ->SetSP( &CLSID_DP8SP_TCPIP );
	m_device ->AddComponent( DPNA_KEY_PORT, &m_port, sizeof( DWORD ), DPNA_DATATYPE_DWORD );

}

// Network class destructor
Network::~Network()
{
	// Save network messages
	Script *settings = new Script( "NetworkSettings.txt" );

	// Check if orginal network settings exist
	if( settings ->GetNumberData( "processing_time" ) == NULL )
	{
		settings ->AddVariable( "port", VARIABLE_NUMBER, &m_port );
		settings ->AddVariable( "send_time_out", VARIABLE_NUMBER, &m_sendTimeOut );
		settings ->AddVariable( "processing_time", VARIABLE_NUMBER, &m_processingTime );
	}
	
	// Current network settings already exist
	else
	{
		settings ->SetVariable( "port", &m_port );
		settings ->SetVariable( "send_time_out", &m_sendTimeOut );
		settings ->SetVariable( "processing_time", &m_processingTime );
	}

	settings  ->SaveScript();

	SAFE_DELETE( settings );

	// Release device address
	if( m_device )
	{
		m_device ->Release();
		m_device = NULL;
	}

	// Close DirectPlay peer interface
	if( m_dpp != NULL )
		m_dpp ->Close( DPNCLOSE_IMMEDIATE );

	// Relase DirectPlay peer interface
	if( m_dpp )
	{
		m_dpp ->Release();
		m_dpp = NULL;
	}

	// Destroy session list
	SAFE_DELETE( m_sessions );

	// Destroy player list
	SAFE_DELETE( m_players );

	// Destroy network message list
	SAFE_DELETE( m_messages );

	// Delete critical sections
	DeleteCriticalSection( &m_sessionCS );
	DeleteCriticalSection( &m_playerCS );
	DeleteCriticalSection( &m_messageCS );

}

// Update network to progress messages
void Network::Update()
{
	EnterCriticalSection( &m_messageCS );

	ReceivedMessage *message = m_messages ->GetFirst();

	unsigned long endTime = timeGetTime() + m_processingTime;

	// Handle elapsed time for network messages
	while( endTime > timeGetTime() && message != NULL )
	{
		HandleNetworkMessage( message );
		m_messages ->Remove( &message );
		message = m_messages ->GetFirst();
	}

	LeaveCriticalSection( &m_messageCS );

}

// Enumerate local network sessions
void Network::EnumerateSessions()
{
	// Emply lists
	m_players ->Empty();
	m_messages ->Empty();
	m_sessions ->Empty();

	// Prepare application description
	DPN_APPLICATION_DESC description;
	ZeroMemory( &description, sizeof( DPN_APPLICATION_DESC ) );
	description.dwSize = sizeof( DPN_APPLICATION_DESC );
	description.guidApplication = m_guid;

	// Synchronize enummerated sessions
	m_dpp ->EnumHosts( &description, NULL, m_device, NULL, 0, 1, 0, 0, NULL, NULL, DPNENUMHOSTS_SYNC );

}

// Get a host session
bool Network::Host( char *name, char *session, int players, void *playerData, unsigned long dataSize )
{
	WCHAR wide[MAX_PATH];

	// Prepare player information
	DPN_PLAYER_INFO player;
	ZeroMemory( &player, sizeof( DPN_PLAYER_INFO ) );
	player.dwSize = sizeof( DPN_PLAYER_INFO );
	player.pvData =playerData;
	player.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
	mbstowcs( wide, name, MAX_PATH );
	player.pwszName = wide;

	// Setup player information
	if( FAILED( m_dpp ->SetPeerInfo( &player, NULL, NULL, DPNSETPEERINFO_SYNC ) ) )
		return false;

	// Prepare application description
	DPN_APPLICATION_DESC description;
	ZeroMemory( &description, sizeof( DPN_APPLICATION_DESC ) );
	description.dwSize = sizeof( DPN_APPLICATION_DESC );
	description.guidApplication = m_guid;
	description.dwMaxPlayers = players;
	mbstowcs( wide, session, MAX_PATH );
	description.pwszSessionName = wide;

	// Host session
	if( FAILED( m_dpp ->Host( &description, &m_device, 1, NULL, NULL, NULL, 0 ) ) )
		return false;

	return true;
}

// Join an enumerated session
bool Network::Join( char *name, int session, void *playerData, unsigned long dataSize )
{
	WCHAR wide[MAX_PATH];

	// Empty lists
	m_players ->Empty();
	m_messages ->Empty();

	// Ignore invalid sessions
	if( session < 0 )
		return false;

	// Prepare player information
	DPN_PLAYER_INFO player;
	ZeroMemory( &player, sizeof( DPN_PLAYER_INFO ) );
	player.dwSize = sizeof( DPN_PLAYER_INFO );
	player.pvData =playerData;
	player.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
	mbstowcs( wide, name, MAX_PATH );
	player.pwszName = wide;

	// Setup player information
	if( FAILED( m_dpp ->SetPeerInfo( &player, NULL, NULL, DPNSETPEERINFO_SYNC ) ) )
		return false;

	// Enter sessions linked list critical session
	EnterCriticalSection( &m_sessionCS );

	m_sessions ->Iterate( true );

	// Find the host of the enumerated session
	for( int s = 0; s < session + 1; s++ )
	{
		// Check if an session exists
		if( m_sessions ->Iterate() == NULL )
		{
			LeaveCriticalSection( &m_sessionCS );
			return false;
		}
	}

	// Join session
	if( FAILED( m_dpp ->Connect(  &m_sessions ->GetCurrent() ->description, m_sessions ->GetCurrent() ->address, m_device, NULL, NULL, NULL, 0, NULL, NULL, NULL, DPNCONNECT_SYNC ) ) )
	{
		LeaveCriticalSection( &m_sessionCS );
		return false;
	}

	// Exit session's linked list critical section
	LeaveCriticalSection( &m_sessionCS );

	return true;

}

// Terminate current session
void Network::Terminate()
{
	// Host only has permission to terminate session
	if( m_dpnidHost == m_dpnidLocal )
		m_dpp -> TerminateSession( NULL, 0, 0 );

	// Close connection and uninitialize DirectPlay peer interface
	if( m_dpp != NULL )
		m_dpp ->Close( DPNCLOSE_IMMEDIATE );

	// Initialize DirectPlay peer interface
	m_dpp ->Initialize( ( PVOID ) this, NetworkMessageHandler, DPNINITIALIZE_HINT_LANSESSION );

}

// Set received allowed flag
void Network::SetReceivedAllowed( bool allowed )
{
	m_receiveAllowed = allowed;

}

// Get next iterated session in list
SessionInfo *Network::GetNextSession( bool restart )
{
	EnterCriticalSection( &m_sessionCS );

	m_sessions ->Iterate( restart );

	// Check session status
	if( restart == true )
		m_sessions ->Iterate();

	LeaveCriticalSection( &m_sessionCS );

	return m_sessions ->GetCurrent();

}

// Get player information 
PlayerInfo *Network::GetPlayer( DPNID dpnid )
{
	EnterCriticalSection( &m_playerCS );

	m_players ->Iterate( true );

	// Search for player data through iterated list
	while( m_players ->Iterate() )
	{
		// Retrieve current player's stats
		if( m_players ->GetCurrent() ->dpnid == dpnid )
		{
			LeaveCriticalSection( &m_playerCS );
			return m_players ->GetCurrent();
		}
	}

	LeaveCriticalSection( &m_playerCS );

	return NULL;

}

// Get local player's DirectPlay ID
DPNID Network::GetLocalID()
{
	return m_dpnidLocal;

}

// Get host's DirectPlay ID
DPNID Network::GetHostID()
{
	return m_dpnidHost;

}

// Determines if player is host or not
bool Network::isHost()
{
	// Check if player is host
	if( m_dpnidHost == m_dpnidLocal )
		return true;

	// Player is client
	else
		return false;
}

// Send network message
void Network::Send( void *data, long size, DPNID dpnid, long flags )
{
	DPNHANDLE hAsync;
	DPN_BUFFER_DESC dpbd;

	// Check buffer size
	if ( ( dpbd.dwBufferSize = size ) == 0 )
		return;

	// Buffer message data
	dpbd.pBufferData = ( BYTE* ) data;
	
	// Send message
	m_dpp ->SendTo( dpnid, &dpbd, 1, m_sendTimeOut, NULL, &hAsync, flags | DPNSEND_NOCOMPLETE | DPNSEND_COALESCE ); 

}

// Network message handler
HRESULT WINAPI Network::NetworkMessageHandler( PVOID context, DWORD msgid, PVOID data )
{
	// Get pointer to call next network object
	Network *network = ( Network* ) context;

	// Process incoming message based on type
	switch( msgid )
	{
	case DPN_MSGID_CREATE_PLAYER:
		{
			unsigned long size = 0;
			DPN_PLAYER_INFO *info = NULL;
			HRESULT hr = DPNERR_CONNECTING;
			PDPNMSG_CREATE_PLAYER msgCreatePlayer = ( PDPNMSG_CREATE_PLAYER ) data;

			// Create player information for new player
			PlayerInfo *playerInfo = new PlayerInfo;
			ZeroMemory( playerInfo, sizeof( PlayerInfo ) );
			playerInfo ->dpnid = msgCreatePlayer ->dpnidPlayer;

			// Keep calling GetPeerInfo() to try to connect
			while( hr == DPNERR_CONNECTING )
				hr = network ->m_dpp ->GetPeerInfo( playerInfo ->dpnid, info, &size, 0 );

			// Check if GetPeerInfo() has returned size of DPN_PLAYER_INFO structure
			if( hr == DPNERR_BUFFERTOOSMALL )
			{
				info = ( DPN_PLAYER_INFO* ) new BYTE[size];
				ZeroMemory( info, size );
				info ->dwSize = sizeof ( DPN_PLAYER_INFO );

				// Try again using the correct size
				if( SUCCEEDED( network ->m_dpp ->GetPeerInfo( playerInfo ->dpnid, info, &size, 0 ) ) )
				{
					// Store name of new player 
					playerInfo ->name = new char[wcslen( info ->pwszName ) + 1];
					ZeroMemory( playerInfo ->name, wcslen( info ->pwszName) + 1 );
					wcstombs( playerInfo ->name, info ->pwszName, wcslen( info ->pwszName ) );

					// Store player data
					playerInfo ->data = new BYTE[info ->dwDataSize];
					memcpy( playerInfo ->data, info ->pvData, info  ->dwDataSize );
					playerInfo ->size = info ->dwDataSize;

					// Store client details
					if( info ->dwPlayerFlags & DPNPLAYER_LOCAL )
						network ->m_dpnidLocal = playerInfo ->dpnid;

					// Store host details
					if( info ->dwPlayerFlags & DPNPLAYER_HOST )
						network ->m_dpnidHost = playerInfo ->dpnid;
				}

				SAFE_DELETE_ARRAY( info );
			}

			// Add new player to list 
			EnterCriticalSection( &network ->m_playerCS );
			network ->m_players ->Add( playerInfo );
			LeaveCriticalSection( &network ->m_playerCS );

			// Check if message handler exists
			if( network ->HandleNetworkMessage == NULL )
				break;

			// Create a create player message
			ReceivedMessage *message = new ReceivedMessage;
			message ->msgid = MSGID_CREATE_PLAYER;
			message ->dpnid = playerInfo ->dpnid;

			// Store message to be processed later
			EnterCriticalSection( &network ->m_messageCS );
			network ->m_messages ->Add( message );
			LeaveCriticalSection( &network ->m_messageCS );

			break;
		}

	case DPN_MSGID_DESTROY_PLAYER:
		{
			// Find player to destroy and remove from list
			EnterCriticalSection( &network ->m_playerCS );
			
			network ->m_players ->Iterate( true );

			// Search through list for player to destroy
			while( network ->m_players ->Iterate() )
			{
				// Check status of player
				if( network ->m_players ->GetCurrent() ->dpnid  == ( ( PDPNMSG_DESTROY_PLAYER )  data )  ->dpnidPlayer )
				{
					network ->m_players ->Remove(  ( PlayerInfo** ) network ->m_players  ->GetCurrent() );
					break;
				}
			}

			LeaveCriticalSection( &network ->m_playerCS );

			// Check message handler exists
			if( network ->HandleNetworkMessage == NULL )
				break;

			// Create a destroy player message
			ReceivedMessage *message = new ReceivedMessage;
	
			message->msgid = MSGID_DESTROY_PLAYER;
			message ->dpnid = ( ( PDPNMSG_DESTROY_PLAYER ) data )  ->dpnidPlayer;

			// Store message so it can be processed later
			EnterCriticalSection( &network ->m_messageCS );
			network ->m_messages ->Add( message );
			LeaveCriticalSection( &network ->m_messageCS );

			break;
		}

	case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{
			PDPNMSG_ENUM_HOSTS_RESPONSE response = ( PDPNMSG_ENUM_HOSTS_RESPONSE ) data;

			// Create new session information
			SessionInfo *sessionInfo = new SessionInfo;
			response ->pAddressSender ->Duplicate( &sessionInfo ->address );
			memcpy( &sessionInfo ->description, response ->pApplicationDescription, sizeof( DPN_APPLICATION_DESC ) );

			// Add new session to the list
			EnterCriticalSection( &network ->m_sessionCS );
			network ->m_sessions ->Add( sessionInfo );
			LeaveCriticalSection( &network ->m_sessionCS );

			break;
		}

	case DPN_MSGID_RECEIVE:
		{
			// Check if message handler exists
			if( network ->HandleNetworkMessage == NULL )
				break;

			// Check if network is allowed to receive messages
			if( network ->m_receiveAllowed == false )
				break;

			// Create receive message
			ReceivedMessage *message = new ReceivedMessage;
			memcpy( message, ( ( PDPNMSG_RECEIVE ) data ) ->pReceiveData, ( ( PDPNMSG_RECEIVE ) data ) ->dwReceiveDataSize );

			// Store message to be process later
			EnterCriticalSection( &network ->m_messageCS );
			network ->m_messages ->Add( message );
			LeaveCriticalSection( &network ->m_messageCS );

			break;
		}

	case DPN_MSGID_TERMINATE_SESSION:
		{
			// Check if message handler exists
			if( network ->HandleNetworkMessage == NULL )
				break;

			// Create terminate session message
			ReceivedMessage *message = new ReceivedMessage;
			message ->msgid = MSGID_TERMINATE_SESSION;

			// Store message to be processed later
			EnterCriticalSection( &network ->m_messageCS );
			network ->m_messages ->Add( message );
			LeaveCriticalSection( &network ->m_messageCS );

			break;
		}
	}

	return S_OK;

}


