// ************************************************************************
//
// File: AnimatedObject.h
// Programmer: T.J. Eason
//  Project: Game Engine
// Description: Get scene object to support mesh animation
// Date: 3-31-10
// Revision 1: 4-2-10
//
// *************************************************************************

#ifndef ANIMATED_OBJECT_H
#define ANIMATED_OBJECT_H

#define TYPE_ANIMATED_OBJECT 1

class AnimatedObject : public SceneObject, public ID3DXAnimationCallbackHandler
{
public:
	AnimatedObject( char *meshName, char *meshPath = "./", unsigned long type = TYPE_ANIMATED_OBJECT );
	virtual ~AnimatedObject();

	virtual void Update( float elapsed, bool addVelocity = true );

	void PlayAnimation( unsigned int animation, float transitionTime, bool loop = true );
	ID3DXAnimationController *GetAnimationController();

private:
	virtual HRESULT CALLBACK HandleCallback( THIS_ UINT Track, LPVOID pCallbackData );

private:
	ID3DXAnimationController *m_animationController;		// Mesh animation playback controller
	unsigned int m_currentTrack;												// Current playing animation track
	float m_currentTime;																// Animation timer

};

#endif
