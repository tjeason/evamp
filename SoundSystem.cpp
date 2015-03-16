//****************************************************************************
//
// File: SoundSystem.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description:   Wrapper class for playing audio files using DirectMusic
// Date: 2-24-10
// Revision 1: 3-25-10
// Reference: DirectX Manual
//
// ****************************************************************************

#include "Engine.h"

// Sound system class constructor
SoundSystem::SoundSystem( float scale )
{
	// Create DirectMusic loader
	CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8, ( void**) &m_loader );

	// Create and initialize DirectMusic performance
	CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, ( void** ) &m_performance );

	// Store interface pointer for new performance object
	m_performance ->InitAudio( NULL, NULL, NULL, DMUS_APATH_SHARED_STEREOPLUSREVERB, 8, DMUS_AUDIOF_ALL, NULL );

	// Get 3D listener by creating a 3D audio path then retrieving the listener from audio path.  Audio path can be released.
	IDirectMusicAudioPath8 *audioPath3D = NULL;

	// Create and initialize audio path for 3D sounds
	m_performance ->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_3D, 1, true, &audioPath3D );

	// Check if audio system is work
	if( audioPath3D != NULL )
	{
		// Create listener
		audioPath3D ->GetObjectInPath( 0, DMUS_PATH_PRIMARY_BUFFER, 0, GUID_All_Objects, 0, IID_IDirectSound3DListener, ( void** )&m_listener );

		// Release listern's memory
		if( audioPath3D )
		{
			audioPath3D ->Release();
			audioPath3D = NULL;
		}

		// Set scale 
		m_scale  = scale;

		// Set distance factor
		m_listener ->SetDistanceFactor( m_scale, DS3D_IMMEDIATE );

	}

	// Audio system does not work so do nothing
	else
		m_listener = NULL;
}

// Sound system class destructor
SoundSystem::~SoundSystem()
{
	// Close down music performance
	m_performance ->CloseDown();

	// Release memory for performance
	if( m_performance )
	{
		m_performance ->Release();
		m_performance = NULL;
	}

	// Release memory for loader
	if( m_loader )
	{
		m_loader ->Release();
		m_loader = NULL;
	}

}

// Udpate sound system's 3D listener
void SoundSystem::UpdateListener( D3DXVECTOR3 forward, D3DXVECTOR3 position, D3DXVECTOR3 velocity )
{
	// Make sure listener is valid
	if( m_listener == NULL )
		return;

	// Set listener's orientation
	m_listener ->SetOrientation( forward.x, forward.y, forward.z, 0.0f, 1.0f, 0.0f, DS3D_DEFERRED );

	// Scale listener's position
	position *= m_scale;

	// Set listener's position
	m_listener ->SetPosition( position.x, position.y, position.z, DS3D_DEFERRED );

	// Scale listener's velocity
	velocity *= m_scale;

	// Set listener's velocity
	m_listener ->SetVelocity( velocity.x, velocity.y, velocity.z, DS3D_DEFERRED );

	// Commit to settings
	m_listener ->CommitDeferredSettings();

}

// Allow sound system to remove unused sound objects
void SoundSystem::GarbageCollection()
{
	m_loader ->CollectGarbage();

}

// Set master volume for all sounds played through sound system
void SoundSystem::SetVolume( long volume )
{
	m_performance ->SetGlobalParam( GUID_PerfMasterVolume, &volume, sizeof( long ) );

}

// Return pointer to sound system's loader
IDirectMusicLoader8 *SoundSystem::GetLoader()
{
	return m_loader;

}

// Return sound system's performance object
IDirectMusicPerformance8 *SoundSystem::GetPerformance()
{
	return m_performance;

}

// Sound class contructor
Sound::Sound( char *filename )
{
	// Convert filename into wide character string
	WCHAR *wideFilename = new WCHAR[strlen( filename ) + 1];
	MultiByteToWideChar( CP_ACP, 0, filename, -1, wideFilename, strlen( filename ) + 1 );
	wideFilename[strlen( filename ) ] = 0;

	// Load sound file
	g_engine ->GetSoundSystem() ->GetLoader() ->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, wideFilename, ( void** ) &m_segment );

	// Delete filename
	SAFE_DELETE_ARRAY( wideFilename );
	
	// Download segment's data to performance
	m_segment ->Download( g_engine ->GetSoundSystem() ->GetPerformance() );

}

// Sound class destructor
Sound::~Sound()
{
	// Unload segment's data from performance
	m_segment ->Unload( g_engine ->GetSoundSystem() ->GetPerformance() );

	// Remove segment from DirectMusic loader
	g_engine ->GetSoundSystem() ->GetLoader() ->ReleaseObjectByUnknown( m_segment );

	// Release segment's memory
	if( m_segment )
	{
		m_segment ->Release();
		m_segment = NULL;
	}

}

// Play sound on standard audiopath with or without looping
void Sound::Play( bool loop, unsigned long flags )
{
	// Sound is looping
	if( loop == true )
		m_segment ->SetRepeats( DMUS_SEG_REPEAT_INFINITE );

	// Play sound only once
	else
		m_segment ->SetRepeats( 0 );

	// Engine plays sound segment
	g_engine ->GetSoundSystem() ->GetPerformance() ->PlaySegment( m_segment, flags, 0, NULL );

}

// Return sound segment
IDirectMusicSegment8 *Sound::GetSegment()
{
	return m_segment;

}

// Audo path 3D class constructor
AudioPath3D::AudioPath3D()
{
	// Clear sound buffer 
	m_soundBuffer = NULL;

	// Clear audio path pointers
	m_audioPath = NULL;

	// Create audio path for 3D parameters to be set individually
	g_engine ->GetSoundSystem() ->GetPerformance() ->CreateStandardAudioPath( DMUS_APATH_DYNAMIC_3D, 1, true, &m_audioPath );

	// Get audio path's sound buffer
	if( m_audioPath != NULL )
		m_audioPath ->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, GUID_NULL, 0, IID_IDirectSound3DBuffer, ( void** )&m_soundBuffer );

}

// Audio path 3D class destructor
AudioPath3D::~AudioPath3D()
{
	// Release memory for sound buffer
	if( m_soundBuffer )
	{
		m_soundBuffer ->Release();
		m_soundBuffer = NULL;
	}

	// Relase memory for audio path
	if( m_audioPath )
	{
		m_audioPath ->Release();
		m_audioPath = NULL;
	}

}

// Set position of sound in 3D space
void AudioPath3D::SetPosition( D3DXVECTOR3 position )
{
	// Make sure sound buffer is valid
	if( m_soundBuffer == NULL )
		return;

	// Scale position
	position *= g_engine ->GetScale();

	// Set postion within sound buffer
	m_soundBuffer ->SetPosition( position.x, position.y, position.z, DS3D_IMMEDIATE );

}

// Set velocity of sound in 3D space
void AudioPath3D::SetVelocity( D3DXVECTOR3 velocity )
{
	// Make sure sound buffer is valid
	if( m_soundBuffer == NULL )
		return;

	// Scale velocity
	velocity *= g_engine ->GetScale();

	// Set velocity within sound buffer
	m_soundBuffer ->SetVelocity( velocity.x, velocity.y, velocity.z, DS3D_IMMEDIATE );

}

// Set mode for audio paths that is playing sound
void AudioPath3D::SetMode( unsigned long mode )
{
	// Make sure sound buffer is valid
	if( m_soundBuffer == NULL )
		return;

	// Set mode within sound buffer
	m_soundBuffer ->SetMode( mode, DS3D_IMMEDIATE );

}

// Play sound on 3D audio path with our without looping
void AudioPath3D::Play( IDirectMusicSegment8 *segment, bool loop, unsigned long flags )
{
	// Make sure audio path is valid
	if( m_audioPath == NULL )
		return;

	// Sound segment is looping
	if( loop == true )
		segment ->SetRepeats( DMUS_SEG_REPEAT_INFINITE );

	// Sound segment is only playing once
	else
		segment ->SetRepeats( 0);

	// Play sound segment
	g_engine ->GetSoundSystem() ->GetPerformance() ->PlaySegmentEx( segment, NULL, NULL, flags, 0, NULL, NULL, m_audioPath );

}