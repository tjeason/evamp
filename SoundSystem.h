//****************************************************************************
//
// File: SoundSystem.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description:   Wrapper class for playing audio files using DirectMusic
// Date: 2-24-10
// Revision 1: 3-25-10
// Reference: DirectX Manual
//
// ****************************************************************************

#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

// Sound system class
class SoundSystem
{
public:
	SoundSystem( float scale = 1.0f );
	virtual ~SoundSystem();

	void UpdateListener( D3DXVECTOR3 forward, D3DXVECTOR3 position, D3DXVECTOR3 velocity );
	void GarbageCollection();
	void SetVolume( long volume );

	IDirectMusicLoader8 *GetLoader();
	IDirectMusicPerformance8 *GetPerformance();

private:
	float m_scale;														// Unit scale in meters/unit

	IDirectMusicLoader8 *m_loader;						// DirectMusic loader
	IDirectMusicPerformance8 *m_performance;	// DirectMusic performance
	IDirectSound3DListener8 *m_listener;				// DirectSound 3D listener

};

// Sound class
class Sound
{
public:
	Sound( char *filename );
	virtual ~Sound();

	void Play( bool loop = false, unsigned long flags = DMUS_SEGF_AUTOTRANSITION );

	IDirectMusicSegment8 *GetSegment();

private:
	IDirectMusicSegment8 *m_segment;

};

// Audio path 3D class
class AudioPath3D
{
public:
	AudioPath3D();
	virtual ~AudioPath3D();

	void SetPosition( D3DXVECTOR3 position );
	void SetVelocity( D3DXVECTOR3 velocity );
	void SetMode( unsigned long mode );

	void Play( IDirectMusicSegment8 *segment, bool loop =false, unsigned long flags = DMUS_SEGF_SECONDARY );

private:
	IDirectMusicAudioPath8 *m_audioPath;		// DirectMusic audo path for 3D playback
	IDirectSound3DBuffer8 *m_soundBuffer;		// Pointer to audio path's sound buffer

};

#endif

