// ***********************************************************************
//
// File: BoundVolume.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Storing bounding volume of 3D shapes
// Date: 3-16-10
// Revision 1: 3-25-10
// 
// ***********************************************************************

#include "Engine.h"

// Bounding volume class constructor
BoundVolume::BoundVolume()
{
	m_box = new BoundingBox();
	m_sphere = new BoundingSphere();
	m_ellipsoidRadius = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

}

// Bounding volume class destructor
BoundVolume::~BoundVolume()
{
	SAFE_DELETE( m_box );
	SAFE_DELETE( m_sphere );

}

// Create bounding volume using given mesh
void BoundVolume::BoundVolumeFromMesh( ID3DXMesh *mesh, D3DXVECTOR3 ellipsoidRadius )
{
	D3DXVECTOR3 *vertices;

	// Check if it locks a vertex buffer and obtains a pointer to the vertex buffer memory
	if( SUCCEEDED( mesh ->LockVertexBuffer( D3DLOCK_READONLY, ( void** ) &vertices )  ) )
	{
		D3DXComputeBoundingBox( vertices, mesh ->GetNumVertices(), D3DXGetFVFVertexSize( mesh ->GetFVF() ), &m_box ->min, &m_box ->max );
		D3DXComputeBoundingSphere( vertices, mesh ->GetNumVertices(), D3DXGetFVFVertexSize( mesh ->GetFVF() ),  &m_sphere ->center, &m_sphere ->radius );
	
		mesh ->UnlockVertexBuffer();
	}

	m_sphere ->center.x = m_box ->min.x + ( ( m_box ->max.x - m_box ->min.x ) / 2.0f );
	m_sphere ->center.y = m_box ->min.y + ( ( m_box ->max.y - m_box ->min.y ) / 2.0f );
	m_sphere ->center.z = m_box ->min.z + ( ( m_box ->max.z - m_box ->min.z ) / 2.0f );

	m_box ->halfSize = ( float ) max( fabs( m_box ->max.x ) , max( fabs( m_box ->max.y ), fabs( m_box ->max.z ) ) );
	m_box ->halfSize = ( float ) max( m_box ->halfSize, max( fabs( m_box ->min.x ), max( fabs( m_box ->min.y ), fabs( m_box ->min.z ) ) ) );

	m_originalMin = m_box ->min;
	m_originalMax = m_box ->max;
	m_originalCenter = m_sphere ->center;

	SetEllipsoidRadius( ellipsoidRadius );

}

// Create bounding volume to enclose given vertices
void BoundVolume::BoundVolumeFromVertices( D3DXVECTOR3 *vertices, unsigned long totalVertices, unsigned long vertexStride, D3DXVECTOR3 ellipsoidRadius )
{
	D3DXComputeBoundingBox( vertices, totalVertices, vertexStride, &m_box ->min, &m_box ->max );
	D3DXComputeBoundingSphere( vertices, totalVertices, vertexStride, &m_sphere ->center, &m_sphere ->radius );

	m_sphere ->center.x = m_box ->min.x + ( ( m_box ->max.x - m_box ->min.x ) / 2.0f );
	m_sphere ->center.y = m_box ->min.y + ( ( m_box ->max.y - m_box ->min.y ) / 2.0f );
	m_sphere ->center.z = m_box ->min.z + ( ( m_box ->max.z - m_box ->min.z ) / 2.0f );

	m_box ->halfSize = ( float ) max( fabs( m_box ->max.x ) , max( fabs( m_box ->max.y ), fabs( m_box ->max.z ) ) );
	m_box ->halfSize = ( float ) max( m_box ->halfSize, max( fabs( m_box ->min.x ), max( fabs( m_box ->min.y ), fabs( m_box ->min.z ) ) ) );

	m_originalMin = m_box ->min;
	m_originalMax = m_box ->max;
	m_originalCenter = m_sphere ->center;

	SetEllipsoidRadius( ellipsoidRadius );

}

// Create bounding volume based on volume details
void BoundVolume::CloneBoundVolume( BoundingBox *box, BoundingSphere *sphere, D3DXVECTOR3 ellipsoidRadius )
{
	m_box ->min = box ->min;
	m_box ->max = box ->max;

	m_sphere ->center = sphere ->center;
	m_sphere ->radius = sphere ->radius;

	m_box ->halfSize = ( float ) max( fabs( m_box ->max.x ) , max( fabs( m_box ->max.y ), fabs( m_box ->max.z ) ) );
	m_box ->halfSize = ( float ) max( m_box ->halfSize, max( fabs( m_box ->min.x ), max( fabs( m_box ->min.y ), fabs( m_box ->min.z ) ) ) );

	m_originalMin = m_box ->min;
	m_originalMax = m_box ->max;
	m_originalCenter = m_sphere ->center;

	SetEllipsoidRadius( ellipsoidRadius );

}

// Reposition bounding volume based on given matrix
void BoundVolume::RepositionBoundVolume( D3DXMATRIX *location )
{
	D3DXVec3TransformCoord( &m_box ->min, &m_originalMin, location );
	D3DXVec3TransformCoord( &m_box ->max, &m_originalMax, location );
	D3DXVec3TransformCoord( &m_sphere ->center, &m_originalCenter, location );

}

// Set bounding box's properties
void BoundVolume::SetBoundingBox( D3DXVECTOR3 min, D3DXVECTOR3 max )
{
	m_originalMin = m_box ->min = min;
	m_originalMax = m_box ->max = max;

	m_box ->halfSize = ( float ) max( fabs( m_box ->max.x ) , max( fabs( m_box ->max.y ), fabs( m_box ->max.z ) ) );
	m_box ->halfSize = ( float ) max( m_box ->halfSize, max( fabs( m_box ->min.x ), max( fabs( m_box ->min.y ), fabs( m_box ->min.z ) ) ) );

}

// Get bounding box
BoundingBox *BoundVolume::GetBoundingBox()
{
	return m_box;

}

// Set bounding sphere's properties
void BoundVolume::SetBoundingSphere( D3DXVECTOR3 center, float radius, D3DXVECTOR3 ellipsoidRadius )
{
	m_originalCenter = m_sphere ->center = center;
	m_sphere ->radius = radius;

	SetEllipsoidRadius( ellipsoidRadius );

}

// Get bounding sphere
BoundingSphere *BoundVolume::GetBoundingSphere()
{
	return m_sphere;

}

// Set ellipsoid radius and compere percentage to sphere
void BoundVolume::SetEllipsoidRadius( D3DXVECTOR3 ellipsoidRadius )
{
	m_ellipsoidRadius = D3DXVECTOR3( m_sphere ->radius * ellipsoidRadius.x, m_sphere ->radius * ellipsoidRadius.y, m_sphere ->radius  * ellipsoidRadius.z );

}

// Get ellipsoidRadius
D3DXVECTOR3 BoundVolume::GetEllipsoidRadius()
{
	return m_ellipsoidRadius;

}