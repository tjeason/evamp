// ************************************************************************
//
// File: RenderCache.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Render index faces from a set of vertices
// Date: 3-17-10
// Revision 1: 3-25-10
//
// ************************************************************************

#ifndef RENDER_CACHE_H
#define RENDER_CACHE_H

class RenderCache
{
public:
	RenderCache( IDirect3DDevice9 *device, Material *material );
	virtual ~RenderCache();

	void AddFace();

	void Prepare(	 unsigned long totalVertices );

	void Begin();

	void RenderFace( unsigned short vertex0, unsigned short vertex1, unsigned short vertex2 );

	void End();

	Material *GetMaterial();

private:
	IDirect3DDevice9 *m_device;						// Direct3D device pointer
	Material *m_material;									// Material pointer

	IDirect3DIndexBuffer9 *m_indexBuffer;		// Index buffer pointer to vertices to render
	
	unsigned short *m_indexPointer;				// Index pointer
	unsigned long m_totalIndices;						// Total number of indices to rendered

	unsigned long m_faces;								// Total number of faces to be rendered

	unsigned long m_totalVertices;					// Total number of vertices

};

#endif 
