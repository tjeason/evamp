//**********************************************************************
//
// File: Scripting.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Scripting system 
// Date: 2-3-10
// Revision 1: 3-23-10
// Revision 2: 3-24-10
// Revision 3: 3-25-10
//
// **********************************************************************

#ifndef SCRIPTING_H
#define SCRIPTING_H

// Variable enumerations
enum { VARIABLE_BOOL, VARIABLE_COLOR, VARIABLE_FLOAT, VARIABLE_NUMBER,  VARIABLE_STRING, 
VARIABLE_VECTOR, VARIABLE_UNKNOWN };

// Variable class
class Variable
{
public:
	Variable( char *name, FILE *file );
	Variable( char *name, char type, void *value );
	virtual ~Variable();

	char GetType();
	char *GetName();
	void *GetData();

private:
	char m_type;		// Type of data stored in a variable
	char *m_name;	// Variable name
	void *m_data;	// Data stored in variable

};

// Script class
class Script : public Resource< Script >
{
public:
	Script( char *name, char *path = "./" );
	
	virtual ~Script();

	void AddVariable( char *name, char type, void *value );
	void SetVariable( char *name, void *value );

	void SaveScript( char *filename = NULL );

	bool *GetBoolData( char *variable );

	D3DCOLORVALUE *GetColorData( char *variable );

	float *GetFloatData( char *variable );

	long *GetNumberData( char *variable );

	char *GetStringData( char *variable );

	D3DXVECTOR3 *GetVectorData( char *variable );

	void *GetUnknownData( char *variable );

private:
	LinkedList< Variable > *m_variables;		// Linked list of variables from a script

};

#endif
