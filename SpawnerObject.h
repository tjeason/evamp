// ************************************************************************
//
// File: SpawnerObject.h
// Programmer: T.J. Eason
//  Project: Game Engine
// Description: Spawn objects into the scene
// Date: 4-1-10
// Revision 1: 4-2-10
//
// *************************************************************************

#ifndef SPAWNER_OBJECT_H
#define SPAWNER_OBJECT_H

#define TYPE_SPAWNER_OBJECT 2

class SpawnerObject : public SceneObject
{
public:
	SpawnerObject( char *name, char *path = "./", unsigned long type = TYPE_SPAWNER_OBJECT );
	virtual ~SpawnerObject();

	virtual void Update( float elapsed, bool addVelocity = true );

	virtual void CollisionOccurred( SceneObject *object, unsigned long collisionStamp );

	Script *GetObjectScript();

private:
	char *m_name;								// Name of the object that is spawned at this spawner.
	float m_frequency;							 // How often the spawner spawns its object (in seconds).
	float m_spawnTimer;						 // Timer used for spawning the object.
	Sound *m_sound;							 // Sound to play when the object is collected.
	AudioPath3D *m_audioPath;		// Audio path to play the sound on.
	Script *m_objectScript;				// Script for the spawner's object.

};

#endif
