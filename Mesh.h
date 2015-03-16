// ************************************************************************
//
// File: Mesh.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Load and manage animated and static meshes
// Date: 3-16-10
// Revision 1: 3-17-10
// Revision 2: 3-24-10
// Revision 3: 3-25-10
// Revision 4: 4-6-10
//
// *************************************************************************

#ifndef MESH_H
#define MESH_H

struct Frame: public D3DXFRAME
{
	D3DXMATRIX finalTransformationMatrix;		// End transformation once combined with parent

	// Get frame's translation
	D3DXVECTOR3 GetTranslation()
	{
		return D3DXVECTOR3( finalTransformationMatrix._41, finalTransformationMatrix._42, finalTransformationMatrix._43 );
	}

};

struct MeshContainer : public D3DXMESHCONTAINER
{
	char **materialNames;											// Temporary array for mesh names
	Material **materials;												// Array of meterials used for mesh
	ID3DXMesh *originalMesh;									// Actual mesh
	D3DXATTRIBUTERANGE *attributeTable;		// Attribute table
	unsigned long totalAttributeGroups;					// Total number of attribute groups
	D3DXMATRIX **boneMatrixPointers;				// Array of pointers to bone transformation matrices

};

// Copied from DirectX Reference to handle Mesh hierarchy
class AllocateHierarchy : public ID3DXAllocateHierarchy
{
		STDMETHOD( CreateFrame )( THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame );
        STDMETHOD( CreateMeshContainer )( THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer );
        STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME pFrameToFree );
        STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree );
};

class Mesh : public BoundVolume, public Resource< Mesh >
{
public:
	Mesh( char *name, char *path = "./" );
	virtual ~Mesh();

	void Update();
	void Render();

	void CloneAnimationController( ID3DXAnimationController **animationController );

	MeshContainer *GetStaticMesh();

	Vertex *GetVertices();

	unsigned short *GetIndices();

	LinkedList< Frame > *GetFrameList();
	Frame *GetFrame( char *name );
	Frame *GetReferencePoint( char *name );

private:
	void PrepareFrame( Frame *frame );
	void UpdateFrame( Frame *frame, D3DXMATRIX *parentTransformationMatrix = NULL );
	void RenderFrame( Frame *frame );



private:
	Frame *m_firstFrame;															// First frame in mesh hierarchy
	ID3DXAnimationController *m_animationController;		// Animation controller

	D3DXMATRIX *m_boneMatrices;										// Transformation bone matrices
	unsigned long m_totalBoneMatrices;									// Number of bones in array

	MeshContainer *m_staticMesh;											// Static mesh
	Vertex *m_vertices;																// Vertex array for static mesh
	unsigned short *m_indices;													// Index array for static mesh

	LinkedList< Frame > *m_frames;										// Linked list of pointers to all frames to mesh
	LinkedList< Frame > *m_refPoints;									// Linked list of pointers to all reference pointers to mesh

};

#endif