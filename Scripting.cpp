//**********************************************************************
//
// File: Scripting.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Scripting system 
// Reference: cplusplus.com, Microsoft Developer's Network
// Date: 2-3-10
// Revision 1: 2-4-10
// Revision 2: 2-5-10
// Revision 3: 3-23-10
// Revision 4: 3-24-10
// Revision 5: 3-25-10
// Revision 6: 4-5-10
//
// **********************************************************************

#include "Engine.h"

// Variable class constructor
Variable::Variable( char *name, FILE *file )
{
	// Store variable name
	m_name = new char[ strlen( name ) + 1 ];
	strcpy( m_name, name );

	// Ensure the file pointer is valid
	if( file == NULL )
		return;

	// Read variable's type
	char buffer[ MAX_PATH ];
	fscanf( file, "%s", buffer );

	// Compare the string types to determine if it is a boolean
	if( strcmp( buffer, "bool" ) == 0 )
	{
		// Variable is a boolean
		m_type = VARIABLE_BOOL;

		// Read and set boolean for variable
		bool value;
		fscanf( file, "%s", buffer );

		// Compare the strings to determine if the boolean is true or false
		if( strcmp( buffer, "true" ) == 0 )
			value = true;

		else 
			value = false;

		m_data = new bool;

		memcpy( m_data, &value, sizeof( bool ) );
	}

	// Determine if the variable is color
	else if( strcmp( buffer, "color" ) == 0 )
	{
		// Variable is a color
		m_type = VARIABLE_COLOR;

		// Read and set color for variable
		D3DCOLORVALUE color;

		fscanf( file, "%s", buffer );
		color.r = ( float ) atof( buffer );

		fscanf( file, "%s", buffer );
		color.g = ( float ) atof( buffer );

		fscanf( file, "%s", buffer );
		color.b = ( float ) atof( buffer );

		fscanf( file, "%s", buffer );
		color.a = ( float ) atof( buffer );

		m_data = new D3DCOLORVALUE;

		memcpy( m_data, &color, sizeof( D3DCOLORVALUE ) );
	}

	// Determine if the variable contains  a floating-point value
	else if( strcmp( buffer, "float" ) == 0 )
	{
		// Variable is a float
		m_type = VARIABLE_FLOAT;

		// Read and set the float for variable
		float value;
		fscanf( file, "%s", buffer );

		value = (float) atof( buffer );
		m_data = new float;

		memcpy( m_data, &value, sizeof( float ) );
	}
	
	// Determine if the variable is a number
	else if( strcmp( buffer, "number" ) == 0 )
	{
		// Variable is a number
		m_type = VARIABLE_NUMBER;

		// Read and set the number for variable
		long value;

		fscanf( file, "%s", buffer );

		value = atol( buffer );

		m_data = new long;

		memcpy( m_data, &value, sizeof( long ) );
	}

	// Determine if the variable is a string
	else if( strcmp( buffer, "string" ) == 0 )
	{
		// Variable is a string
		m_type = VARIABLE_STRING;

		// Search for the open quote within a path
		bool quoteFound = false;
		ZeroMemory( buffer, MAX_PATH * sizeof( char ) );
		fscanf( file, "%c", buffer );

		// String is found and now search for the open quote
		while( true )
		{
			// Open comma is found
			if( strcmp( buffer, "\"" ) == 0)
			{
				quoteFound = true;
				break;
			}

			// Getting position of the string
			if( strcmp( buffer, " ") != 0)
			{
				fpos_t pos;
				fgetpos( file, &pos );
				fsetpos( file, &--pos );

				break;
			}

			fscanf( file, "%c", buffer );

		}

		// Read and set the string  for variable
		char completeString[ MAX_PATH ];

		ZeroMemory( completeString, MAX_PATH * sizeof( char ) );

		bool addSpacing = false;

		// While the string still exists
		do
		{
			fscanf( file, "%s", buffer );

			// Open quotation is found
			if( strcmp( &buffer[ strlen( buffer ) -1 ], "\"" ) == 0 )
			{
				buffer[ strlen( buffer ) - 1 ] = 0;
				quoteFound = false;
			}

			// Need to add spacing
			if( addSpacing == false )
				addSpacing = true;

			// Empty string is found
			else
				strcat( completeString, " ");

			strcat( completeString, buffer );
		} while( quoteFound == true );

		m_data = new char[ strlen( completeString) + 1 ];

		strcpy( (char*) m_data,  completeString );
	}

	// Determine if the variable is a vector
	else if ( strcmp( buffer, "vector" ) == 0 )
	{
		// Variable is a vector
		m_type = VARIABLE_VECTOR;

		// Read and set vector to variable 
		D3DXVECTOR3 vector;

		fscanf( file, "%s", buffer );
		vector.x = (float) atof( buffer );

		fscanf( file, "%s", buffer );
		vector.y = (float) atof( buffer );

		fscanf( file, "%s", buffer );
		vector.z = (float) atof( buffer );

		m_data = new D3DXVECTOR3;

		memcpy( m_data, &vector, sizeof( D3DXVECTOR3 ) );
	}

	// Variable is unknown
	else
	{
		// Variable has an unknown type
		m_type = VARIABLE_UNKNOWN;

		// Read and set data to variable
		fscanf( file, "%s", buffer );
		
		m_data = new char[ strlen( buffer ) + 1 ];

		strcpy( (char *) m_data, buffer );
	}
}

// Variable class constructor
Variable::Variable( char *name, char type, void *value )
{
	// Store name of variable
	m_name = new char[ strlen( name ) + 1 ];
	
	strcpy( m_name, name );

	// Store variable type
	m_type = type;

	// Set the variable's data to its type
	switch( type )
	{
	case VARIABLE_BOOL:
		m_data = new bool;
		memcpy( m_data, ( bool* ) value, sizeof( bool ) );
		return;

	case VARIABLE_COLOR:
		m_data = new D3DCOLORVALUE;
		memcpy( m_data, ( D3DCOLORVALUE*)value, sizeof( D3DCOLORVALUE ) );
		return;

	case VARIABLE_FLOAT:
		m_data = new float;
		memcpy( m_data, ( float* )value, sizeof( float ) );
		return;

	case VARIABLE_NUMBER:
		m_data = new long;
		memcpy( m_data, ( long* )value, sizeof( long ) );
		return;

	case VARIABLE_STRING:
		m_data = new char[ strlen( (char* )value ) + 1 ];
		strcpy( (char* )m_data, ( char* )value );
		return;

	case VARIABLE_VECTOR:
		m_data = new D3DXVECTOR3;
		memcpy( m_data, (D3DXVECTOR3* )value, sizeof( D3DXVECTOR3 ) );
		return;

	default:
		m_data = new char[ strlen( ( char* ) value ) + 1 ];
		strcpy( ( char* ) m_data, ( char* ) value );
		return;
	}
}

// Variable class destructor
Variable::~Variable()
{
	SAFE_DELETE_ARRAY( m_name );

	// Destroy any data that is a string
	if( m_type == VARIABLE_STRING || m_type == VARIABLE_UNKNOWN )
	{
		SAFE_DELETE_ARRAY( m_data );
	}

	//  Destroy nonstrings
	else
		SAFE_DELETE( m_data );

}

// Get variable type
char Variable::GetType()
{
	return m_type;

}

// Get variable name
char *Variable::GetName()
{
	return m_name;

}

// Get data in variable
void *Variable::GetData()
{
	// Use the correct data type
	switch( m_type )
	{
	case VARIABLE_BOOL:
		return (bool*)m_data;

	case VARIABLE_COLOR:
		return (D3DCOLOR*)m_data;

	case VARIABLE_FLOAT:
		return (float*)m_data;

	case VARIABLE_NUMBER:
		return (long*)m_data;

	case VARIABLE_STRING:
		return (char*)m_data;

	case VARIABLE_VECTOR:
		return (D3DXVECTOR3*)m_data;

	default:
		return m_data;
	}

}

// Script class constructor
Script::Script( char *name, char *path ) : Resource< Script > ( name, path )
{
	// Create linked list to store all script's variables
	m_variables = new LinkedList< Variable >;

	FILE *file =NULL;

	// Open script using its filename
	if( ( file = fopen( GetFilename(), "r" ) ) == NULL )
		return;

	// Read script 
	bool read = false;
	char buffer[ MAX_PATH ];

	fscanf( file, "%s", buffer );

	// Read script until EOF is reached
	while( feof( file ) == 0 )
	{
		// Check file position indicator is between #begin and #end. 
		// Read data into Variable linked list
		if( read == true )
		{
			// Stop reading data if #end is read
			if( strcmp( buffer, "#end" ) == 0 )
				read = false;

			// Variables still need to be read
			else
				m_variables ->Add( new Variable( buffer, file ) );
		}

		// Reading #begin, which indicates a new script needs to be read
		else if ( strcmp( buffer, "#begin" ) == 0 )
			read = true;

		// Read next string
		fscanf( file, "%s", buffer );
	}
	
	// Close file
	fclose( file );

}

// Script class destructor
Script::~Script( )
{
	SAFE_DELETE( m_variables );

}

// Add new variables to script
void Script::AddVariable( char *name, char type, void *value )
{
	 m_variables ->Add( new Variable( name, type, value ) );

}

// Set the value of an existing variable in the script
void Script::SetVariable( char *name, void *value )
{
	// Find variable
	Variable *variable = NULL;

	m_variables ->Iterate( true );

	// Iterate through linked list
	while( m_variables ->Iterate() != NULL )
	{
		// Get the current existing variables
		if( strcmp( m_variables ->GetCurrent() ->GetName() , name ) == 0 )
		{
			variable =  m_variables ->GetCurrent();
			break;
		}
	}

	// Make sure variable was found
	if( variable == NULL )
		return;

	// Get variable's type
	char type = variable ->GetType();

	// Destroy variable
	m_variables ->Remove( &variable );

	// Add variable back with new value
	AddVariable( name, type, value );

}

// Save script to file
void Script::SaveScript( char *filename )
{
	FILE *file = NULL;
	char output[ MAX_PATH ];

	// Open file based on filename if it is available
	if( filename != NULL )
	{
		// Write to file
		if( ( file = fopen( filename, "w" ) ) == NULL )
			return;
	}

	// Open the internal file
	else
	{
		// Get the internal filename and set to write mode
		if( ( file = fopen( GetFilename(), "w" ) ) == NULL )
			return;
	}

	// Write #begin state to file
	fputs( "#begin\n", file );

	// Write each variable to file
	m_variables ->Iterate( true );

	// Iterate through linked list and write each available variable
	while( m_variables ->Iterate() != NULL )
	{
		// Determine a current variable's type
		switch( m_variables ->GetCurrent() ->GetType() )
		{
		case VARIABLE_BOOL:
			if( *( (bool*)m_variables ->GetCurrent() ->GetData() ) == true )
				sprintf( output, "%s bool true", m_variables ->GetCurrent() ->GetName() );

			else
				sprintf( output, "%s bool false", m_variables ->GetCurrent() ->GetName() );

			fputs( output, file );
			fputs( "\n", file );

			continue;

		case VARIABLE_COLOR:
			sprintf( output, "%s color %f %f %f %f", m_variables ->GetCurrent() ->GetName(), 
					     ( ( D3DCOLORVALUE* ) m_variables ->GetCurrent() ->GetData() ) ->r,
						 ( ( D3DCOLORVALUE* ) m_variables ->GetCurrent() ->GetData() ) ->g,
						 ( ( D3DCOLORVALUE* ) m_variables ->GetCurrent() ->GetData() ) ->b,
						 ( ( D3DCOLORVALUE* ) m_variables ->GetCurrent() ->GetData() ) ->a );

			fputs( output, file );
			fputs( "\n", file );

			continue;

		case VARIABLE_FLOAT:
			sprintf( output, "%s float %f",  m_variables ->GetCurrent() ->GetName(), *( float* ) m_variables ->GetCurrent() ->GetData() );

			fputs( output, file );
			fputs( "\n", file );

			continue;

		case VARIABLE_NUMBER:
			sprintf( output, "%s number %d", m_variables ->GetCurrent() ->GetName(), *( long* ) m_variables ->GetCurrent() ->GetData() );

			fputs( output, file );
			fputs( "\n", file );

			continue;

		case VARIABLE_STRING:
			sprintf( output, "%s string \"%s\"", m_variables ->GetCurrent() ->GetName(), ( char* ) m_variables ->GetCurrent() ->GetData() );

			fputs( output, file );
			fputs( "\n", file );

			continue;

		case VARIABLE_VECTOR:
			sprintf( output, "%s vector %f %f %f", m_variables ->GetCurrent() ->GetName(), 
					   ( ( D3DXVECTOR3* ) m_variables ->GetCurrent() ->GetData() ) -> x,
					   ( ( D3DXVECTOR3* ) m_variables ->GetCurrent() ->GetData() ) -> y,
					   ( ( D3DXVECTOR3* ) m_variables ->GetCurrent() ->GetData() ) -> z );

			fputs( output, file );
			fputs( "\n", file );

			continue;

		default:
			sprintf( output, "%s unknown %s", m_variables ->GetCurrent() ->GetName(), ( char* ) m_variables ->GetCurrent() ->GetData() );
	
			fputs( output, file );
			fputs( "\n", file );

			continue;
		}
	}

	// Write #end statement to file
	fputs( "#end", file );

	// Close the file
	fclose( file );

}

// Return boolean data from variable
bool *Script:: GetBoolData( char *variable )
{
	m_variables ->Iterate( true );

	// Iterate through linked list to get boolean data
	while( m_variables ->Iterate() != NULL )
		if( strcmp( m_variables ->GetCurrent() ->GetName(), variable ) == 0)
			return (bool*)m_variables ->GetCurrent() ->GetData();

	return NULL;

}

// Return color data from variable
D3DCOLORVALUE *Script::GetColorData( char *variable )
{
	m_variables ->Iterate( true );

	// Iterate through linked list to get color data
	while( m_variables ->Iterate() != NULL )
		if( strcmp( m_variables ->GetCurrent() ->GetName(), variable ) == 0)
			return ( D3DCOLORVALUE*)m_variables ->GetCurrent() ->GetData();

	return NULL;

}

// Return float data from variable
float *Script::GetFloatData( char *variable )
{
	m_variables ->Iterate( true );

	// Iterate through linked list to get float data
	while( m_variables ->Iterate() != NULL )
		if( strcmp( m_variables ->GetCurrent() ->GetName(), variable ) == 0)
			return ( float*)m_variables ->GetCurrent() ->GetData();

	return NULL;

}

// Return number from variable
long *Script::GetNumberData( char *variable )
{
	m_variables ->Iterate( true );

	// Iterate through linked list to get number data
	while( m_variables ->Iterate() != NULL )
		if( strcmp( m_variables ->GetCurrent() ->GetName(), variable ) == 0)
			return (long*)m_variables ->GetCurrent() ->GetData();

	return NULL;

}

// Return string from variable
char *Script::GetStringData( char *variable )
{
	m_variables ->Iterate( true );

	// Iterate through linked list to get string data
	while( m_variables ->Iterate() != NULL )
		if( strcmp( m_variables ->GetCurrent() ->GetName(), variable ) == 0)
			return (char*) m_variables ->GetCurrent() ->GetData();

	return NULL;

}

// Return vector data from variable
D3DXVECTOR3 *Script::GetVectorData( char *variable )
{
	m_variables ->Iterate( true );

	// Iterate through linked list to get vector data
	while( m_variables ->Iterate() != NULL )
		if( strcmp( m_variables ->GetCurrent() ->GetName(), variable ) == 0)
			return (D3DXVECTOR3*) m_variables ->GetCurrent() ->GetData();

	return NULL;

}

void *Script::GetUnknownData( char *variable )
{
	m_variables ->Iterate( true );

	// Iterate through linked list to get unknown data
	while( m_variables ->Iterate() != NULL )
		if( strcmp( m_variables ->GetCurrent() ->GetName(), variable ) == 0)
			return m_variables ->GetCurrent() ->GetData();

	return NULL;

}





