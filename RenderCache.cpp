// ************************************************************************
//
// File: RenderCache.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Render index faces from a set of vertices
// Date: 3-17-10
// Revision 1: 3-18-10
// Revision 2: 3-25-10
//
// ************************************************************************

#include "Engine.h"

// Render cache class constructor
RenderCache::RenderCache( IDirect3DDevice9  *device, Material *material )
{
	m_device = device;
	m_material = material;
	m_indexBuffer = NULL;
	m_totalIndices = 0;

}

// Render cache class destructor
RenderCache::~RenderCache()
{
	// Release index buffer
	if( m_indexBuffer )
	{
		m_indexBuffer ->Release();
		m_indexBuffer = NULL;
	}

}

// Incease the size of the render cache to manage another face
void RenderCache::AddFace()
{
	m_totalIndices += 3;

}

// Prepare render cache
void RenderCache::Prepare( unsigned long totalVertices )
{
	// Set total vertices to be rendered
	m_totalVertices = totalVertices;

	// Create render cache's index buffer
	m_device ->CreateIndexBuffer( m_totalIndices * sizeof( unsigned short ), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_indexBuffer, NULL );

}

// Inform render cache to rendering is about to begin
void RenderCache::Begin()
{
	m_indexBuffer ->Lock( 0, 0, ( void** ) &m_indexPointer, 0 );
	m_faces = 0;

}

// Provide face indices to be rendered
void RenderCache::RenderFace( unsigned short vertex0, unsigned short vertex1, unsigned short vertex2 )
{
	*m_indexPointer++ = vertex0;
	*m_indexPointer++ = vertex1;
	*m_indexPointer++ = vertex2;

	m_faces++;

}

// Inform render cache that rendering has completed
void RenderCache::End()
{
	// Unlock index buffer
	m_indexBuffer ->Unlock();

	// Check if there are any faces to render
	if( m_faces == 0 )
		return;

	// Check if material should ignore fog
	if( m_material ->GetIgnoreFog() == true )
		m_device ->SetRenderState( D3DRS_FOGENABLE, false );

	// Set material and textures
	m_device ->SetMaterial( m_material ->GetLighting() );;
	m_device ->SetTexture( 0, m_material ->GetTexture() );

	// Set indices to render correct faces
	m_device ->SetIndices( m_indexBuffer );

	// Render faces
	m_device ->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_totalVertices, 0, m_faces );

	// Restore fog setting if it needs adjustments
	if( m_material ->GetIgnoreFog() == true )
		m_device ->SetRenderState( D3DRS_FOGENABLE, true );

}

// Get material being used by render cache
Material *RenderCache::GetMaterial()
{
	return m_material;

}

