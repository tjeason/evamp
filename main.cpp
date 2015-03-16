//****************************************************************************
//
// File: main.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description:   Main source to launch  the game engine
// Date: 1-21-10
// Revision 1: 1-28-10
// Revision 2: 2-19-10
// Revision 3: 2-24-10
// Revision 4: 2-25-10
// Revision 5: 3-1-10
// Revision 6: 3-5-10
// Revision 7: 3-9-10
// Revision 8: 3-11-10
// Revision 9: 3-18-10
// Revision 10: 3-23-10
// Revision 11: 3-24-10
// Revision 12: 3-25-10
// Revision 13: 4-5-10
// Revision 14: 4-6-10
// Revision 15: 4-21-10
// Reference: Microsoft Developer's Network 
//
// ****************************************************************************

// System Includes
#include <windows.h>

// Engine Includes
#include "Engine.h"

class TestState : public State
{
public:
	// Allows the test state to preform any pre-processing construction.
	virtual void Load()
	{
		// Load the text scene.
		g_engine->GetSceneManager()->LoadScene( "Abandoned City.txt", "./Assets/" );

		// Create a scene object that will be used as a camera.
		g_engine->GetSceneManager()->AddObject( m_viewer = new SceneObject );
		m_viewer->SetTranslation( 0.0f, 200.0f, -500.0f );
		m_viewer->SetFriction( 5.0f );

	};

	// Returns the view setup details for the given frame.
	virtual void RequestViewer( ViewerSetup *viewer )
	{
		viewer->viewer = m_viewer;
		viewer->viewClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
	}

	// Updates the test state.
	virtual void Update( float elapsed )
	{
		// Allow the camera (scene object) to be moved around.
		if( g_engine->GetInput()->GetKeyPress( DIK_W, true ) == true )
			m_viewer->Drive( 4000.0f * elapsed, false );

		if( g_engine->GetInput()->GetKeyPress( DIK_S, true ) == true )
			m_viewer->Drive( -4000.0f * elapsed, false );

		if( g_engine->GetInput()->GetKeyPress( DIK_A, true ) == true )
			m_viewer->Strafe( -4000.0f * elapsed, false );

		if( g_engine->GetInput()->GetKeyPress( DIK_D, true ) == true )
			m_viewer->Strafe( 4000.0f * elapsed, false );

		// Adjust the rotation of the camera based on the mouse movement.
		m_viewer->AddRotation( (float)g_engine->GetInput()->GetDeltaY() * elapsed, (float)g_engine->GetInput()->GetDeltaX() * elapsed, 0.0f );

	

	};

private:
	SceneObject *m_viewer;		// Viewer scene object for camera use

};

// Application specific state setup.
void StateSetup()
{
	g_engine->AddState( new TestState, true );

}

// Entry point for the application.
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int cmdShow )
{
	// Create the engine setup structure.
	EngineSetup setup;
	setup.instance = instance;
	setup.name = "Evamp Game Engine Demo";
	setup.scale = 0.01f;
	setup.StateSetup = StateSetup;
	setup.spawnerPath = "./Assets/Spawners/";

	// Create the engine (using the setup structure), then run it.
	new Engine( &setup );
	g_engine->Run();

	return true;
}