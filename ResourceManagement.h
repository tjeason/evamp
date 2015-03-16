//**********************************************************************
//
// File: ResourceManagement.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Manage resource types and ensure no redunancy 
// Date: 1-19-10
// Revision1: 2-8-10
// Revision 2: 3-25-10
//
// **********************************************************************

#ifndef RESOURCE_MANAGEMENT_H
#define RESOURCE_MANAGEMENT_H

template < class Type > class Resource
{
public:
	// Resource class constructor
	Resource( char *name, char *path = "./" )
	{
		// Store name
		if( name != NULL )
		{
			m_name = new char[strlen( name ) + 1];

			memcpy( m_name, name, ( strlen( name ) + 1) * sizeof( char ) );
		}

		// Store path
		if( path != NULL )
		{
			m_path = new char[strlen( path ) + 1];

			memcpy( m_path, path, ( strlen( path ) +1 ) * sizeof( char ) );
		}

		// Create filename
		if( name != NULL && path != NULL )
		{
			m_filename = new char[strlen( name ) + strlen( path ) + 1];

			sprintf( m_filename, "%s%s", path, name );
		}

		// Start reference count
		m_refCount = 1;

	}

	// Resource class destructor
	virtual ~Resource()
	{
		SAFE_DELETE_ARRAY( m_name );
		SAFE_DELETE_ARRAY( m_path );
		SAFE_DELETE_ARRAY( m_filename );

	}

	// Get resource's name
	char *GetName()
	{
		return m_name;

	}

	// Get resource's path
	char *GetPath()
	{
		return m_path;

	}

	// Get resource's filename
	char *GetFilename()
	{
		return m_filename;

	}

	// Increment resouce's reference count
	void IncRef()
	{
		m_refCount++;

	}

	// Decrement resource's reference count
	void DecRef()
	{
		m_refCount--;

	}

	// Get resource's reference count
	unsigned long GetRefCount()
	{
		return m_refCount;

	}

private:
	char *m_name;						// Resource name
	char *m_path;						// Resource path
	char *m_filename;					// Resource filename
	unsigned long m_refCount;	// Reference count
};

// Resource manager class
template < class Type > class ResourceManager
{
public:
	// Resource manager class constructor
	ResourceManager( void( *CreateResourceFunction) ( Type **resource, char *name, char *path ) = NULL )
	{
		m_list = new LinkedList< Type >;

		CreateResource = CreateResourceFunction;
	}

	// Resource manager class destructor
	~ResourceManager()
	{
		SAFE_DELETE( m_list );

	}

	// Add new resource to manager
	Type *Add( char *name, char *path = "./" )
	{
		// Make sure they are valid
		if( m_list == NULL || name == NULL || path == NULL )
			return NULL;

		Type *element = GetElement( name, path );

		// Element exists, so point to it and increment reference
		if( element != NULL )
		{
			element -> IncRef();
			return element;
		}

		Type *resource = NULL;

		// Created resource exists and call it 
		if( CreateResource != NULL )
			CreateResource( &resource, name, path );

		// Create resource for new data type
		else
			resource = new Type( name, path );

		// Add new resource to manager and return a pointer to it.
		return m_list -> Add( resource );

	}

	// Remove the resource from the manager
	void Remove( Type **resource )
	{
		// Make sure they are valid
		if( *resource == NULL || m_list == NULL )
			return;

		// Decrement resource's reference count
		( *resource ) -> DecRef();

		// Destroy resource if it is not being used
		if ( ( *resource ) -> GetRefCount() == 0 )
			m_list -> Remove( resource );

	}

	// Empty resource list
	void EmptyList()
	{
		// Resource list exists and needs to be removed
		if( m_list != NULL )
			m_list -> Empty();

	}

	// Get list of resources
	LinkedList< Type > *GetList()
	{
		return m_list;

	}

	Type *GetElement( char *name, char *path ="./" )
	{
		// Make sure they are valid
		if( name == NULL || path == NULL || m_list == NULL )
			return NULL;

		// List is valid but nothing exists
		if( m_list -> GetFirst() == NULL )
			return NULL;

		m_list -> Iterate( true );

		// Iterate through list and find resource
		while( m_list -> Iterate() )
			if( strcmp( m_list -> GetCurrent() -> GetName(), name ) == 0)
				if( strcmp( m_list -> GetCurrent() -> GetPath(), path ) == 0)
					return m_list -> GetCurrent();

		// Null is retourned if the resource was not found
		return NULL;

	}

private:
	LinkedList< Type > *m_list;		// Linked list resources

	void ( *CreateResource ) ( Type **resource, char *name, char *path ); // Create resource for application

};

#endif

		