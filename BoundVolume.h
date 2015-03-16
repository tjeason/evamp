// ***********************************************************************
//
// File: BoundVolume.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Storing bounding volume of 3D shapes
// Date: 3-16-10
// Revision 1: 3-25-10
// 
// ***********************************************************************

#ifndef BOUND_VOLUME_H
#define BOUND_VOLUME_H

struct  BoundingBox
{
	D3DXVECTOR3 min;		// Minimun length of bounding box
	D3DXVECTOR3 max;		// Maximum length of bounding box

	float halfSize;						// Distance from volume's center to furthest point of any axis.

};

struct BoundingSphere
{
	D3DXVECTOR3 center;		// Sphere's center point

	float radius;							// Sphere's radius

};

class BoundVolume
{
public :
	BoundVolume();
	
	virtual ~BoundVolume();

	void BoundVolumeFromMesh( ID3DXMesh *mesh, D3DXVECTOR3 ellipsoidRadius = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
	void BoundVolumeFromVertices( D3DXVECTOR3 *vertices, unsigned long totalVertices, unsigned long vertexStride, D3DXVECTOR3 ellipsoidRadius = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );

	void CloneBoundVolume( BoundingBox *box, BoundingSphere *sphere, D3DXVECTOR3 ellipsoidRadius = D3DXVECTOR3( 1.0f, 1.0f, 1.0f)  );

	void RepositionBoundVolume( D3DXMATRIX *location );

	void SetBoundingBox( D3DXVECTOR3 min, D3DXVECTOR3 max );
	BoundingBox *GetBoundingBox();

	void SetBoundingSphere(  D3DXVECTOR3 centre, float radius, D3DXVECTOR3 ellipsoidRadius = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
	BoundingSphere *GetBoundingSphere();

	 void SetEllipsoidRadius( D3DXVECTOR3 ellipsoidRadius );
	D3DXVECTOR3 GetEllipsoidRadius();

private:
	BoundingBox *m_box;									// Bounding volume of box
	BoundingSphere *m_sphere;						// Bounding volume of sphere

	D3DXVECTOR3 m_originalMin;				// Bounding box's original min point
	D3DXVECTOR3 m_originalMax;				// Bounding box's original max point
	D3DXVECTOR3 m_originalCenter;			// Bounding sphere's original center point
	D3DXVECTOR3 m_ellipsoidRadius;		// Ellipsoid radius

};

#endif


