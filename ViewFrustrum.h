// ****************************************************
// 
// File: ViewFrustrum.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Manage view frustrum for view matrix
// Date: 4-19-10
//
// *****************************************************

#ifndef VIEW_FRUSTUM_H
#define VIEW_FRUSTUM_H

// View Frustum Class
class ViewFrustum
{
public:
	void Update( D3DXMATRIX *view );

	void SetProjectionMatrix( D3DXMATRIX projection );

	bool IsBoxInside( D3DXVECTOR3 min, D3DXVECTOR3 max );
	bool IsBoxInside( D3DXVECTOR3 translation, D3DXVECTOR3 min, D3DXVECTOR3 max );
	bool IsSphereInside( D3DXVECTOR3 translation, float radius );

private:
	D3DXMATRIX m_projection;		// Pointer to a projection matrix.
	D3DXPLANE m_planes[5];			// Five planes of the view frustum (near plane is ignored).
};

#endif