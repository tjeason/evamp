// ***********************************************************************
//
// File: Network.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: DirectPlay wrapper class to handle system network messages
// Date: 3-3-10
// Revision 1: 3-9-10
// Revision 2: 3-11-10
// Revision 3: 3-24-10
// Revision 4: 3-25-10
//
// ***********************************************************************

#ifndef NETWORK_H
#define NETWORK_H

// Standard network message
#define MSGID_CREATE_PLAYER 0x12001
#define MSGID_DESTROY_PLAYER 0x12002
#define MSGID_TERMINATE_SESSION 0x12003

// Network message structure
struct NetworkMessage
{
	unsigned long msgid;		// Message ID
	DPNID dpnid;					// DirectPlay ID

};

// Receive message structure
struct ReceivedMessage : public NetworkMessage
{
	char data[32];		// Message data

};

// Session information structure
struct SessionInfo
{
	IDirectPlay8Address *address;						// Session network address
	DPN_APPLICATION_DESC description;		// Application description

};

// Player information structure
struct PlayerInfo
{
	DPNID dpnid;				// DirectPlay player ID
	char *name;					// Player's name
	void *data;						// Player's data
	unsigned long size;		// Data size

	// Player info constructor 
	PlayerInfo()
	{
		dpnid = 0;
		name = NULL;
		data = NULL;
		size = 0;

	}

	// Player infro destructor
	~PlayerInfo()
	{
		SAFE_DELETE( name );
		SAFE_DELETE( data );

	}

};

// Network class
class Network
{
public:
	Network( GUID guid, void ( *HandleNetworkMessageFunction ) ( ReceivedMessage *msg ) );
	virtual ~Network();

	void Update();

	void EnumerateSessions();

	bool Host( char *name, char *session, int playes = 0, void *playerData = NULL, unsigned long datasizie = 0 );
	bool Join( char *name, int session = 0, void *playerData = NULL, unsigned long dataSize = 0 );

	void Terminate();

	void SetReceivedAllowed( bool allowed );

	SessionInfo *GetNextSession( bool restart = false );

	PlayerInfo *GetPlayer( DPNID dpnid );

	DPNID GetLocalID();
	DPNID GetHostID();

	bool isHost();

	void Send( void *data, long size, DPNID dpnid = DPNID_ALL_PLAYERS_GROUP, long flags = 0 );

private:
	static HRESULT WINAPI NetworkMessageHandler( PVOID context, DWORD msgid, PVOID data );

private:
	GUID m_guid;																						// Game specific GUID
	IDirectPlay8Peer *m_dpp;																	// DirectPlay peer interface
	IDirectPlay8Address *m_device;														// DirectPlay device address

	unsigned long m_port;																			// Network communication port
	unsigned long m_sendTimeOut;															// Timeout messaging
	unsigned long m_processingTime;														// Processing time for received network messages

	DPNID m_dpnidLocal;																			// DirectPlay ID for local player
	DPNID m_dpnidHost;																			// DirectPay ID for host player

	CRITICAL_SECTION m_sessionCS;													// Session list for critical section
	
	LinkedList< SessionInfo > *m_sessions;											// Linked list of enumerated sessions

	CRITICAL_SECTION m_playerCS;													// Player list for critical session

	LinkedList< PlayerInfo > *m_players;													// Linked list of players

	bool m_receiveAllowed;																		// Checks if network is available to receive messages

	CRITICAL_SECTION m_messageCS;												// Network messages through critical session

	LinkedList< ReceivedMessage > *m_messages;								// Linked list of network messages

	void ( *HandleNetworkMessage ) (ReceivedMessage *msg );		// Pointer to network message handler

};

#endif