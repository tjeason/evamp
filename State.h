//****************************************************************************
//
// File: State.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description:   Game derive states from the class to add to the engine.
// Date: 1-25-10
// Revision 1: 3-25-10
// Revision 2: 4-2-10
// References: Cube Engine Manual
//
// ****************************************************************************

#ifndef STATE_H
#define STATE_H

// Viewer setup structure
struct ViewerSetup
{
	SceneObject *viewer;					 // Current object being viewed
	unsigned long viewClearFlags;	 // Flags used for clearing the view.

	// The viewer setup structure constructor.
	ViewerSetup()
	{
		viewer = NULL;
		viewClearFlags = 0;

	};

};

// State class
class State
{
public:
	State( unsigned long id = 0 );

	virtual void Load();
	virtual void Close();

	virtual void RequestViewer( ViewerSetup *viewer );
	virtual void Update( float elapsed );
	virtual void Render();

	unsigned long GetID();

private:
	unsigned long m_id;		// Distinct game id for state switching

};

#endif

