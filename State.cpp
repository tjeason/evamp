//****************************************************************************
//
// File: State.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description:   Game derive states from the class to add to the engine.
// Date: 1-26-10
// Revision 1: 3-25-10
// Revision 2: 4-2-10
//
// ****************************************************************************

#include "Engine.h"

// State class constructor
State::State( unsigned long id )
{
	m_id = id;

}

// Load a state
void State::Load()
{

}

// Close a state
void State::Close()
{

}

// View setup details for a given frame
void State::RequestViewer( ViewerSetup *viewer )
{
	viewer ->viewer = NULL;
	viewer ->viewClearFlags = 0;

}

// Update state
void State::Update( float elapsed )
{

}

// Render state
void State::Render()
{

}

// Get state's ID
unsigned long State::GetID()
{
	return m_id;

}

