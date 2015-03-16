// ***********************************************************************
//
// File: Material.h
// Programmer: T.J. Eason
// Project: Game Engine 
// Description: Lighting and texture properties
// Date: 3-11-10
// Revision 1: 3-24-10
// Revision 2: 3-25-10
//
// ************************************************************************

#ifndef MATERIAL_H
#define MATERIAL_H

// Material class
class Material: public Resource< Material >
{
public:
	Material();
	Material( char *name, char *path = "./" );

	virtual ~Material();

	IDirect3DTexture9 *GetTexture();
	D3DMATERIAL9 *GetLighting();

	unsigned long GetWidth();
	unsigned long GetHeight();

	bool GetIgnoreFace();
	bool GetIgnoreFog();
	bool GetIgnoreRay();

private:
	IDirect3DTexture9  *m_texture;		// Direct3D texture
	D3DMATERIAL9 m_lighting;			// Direct3D lighting

	IDirect3DTexture9 *m_normalMap; 
	bool m_normalMapped; 
	bool m_ignoreLight; 


	unsigned long m_width;					// Texture width
	unsigned long m_height;					// Texture height

	bool m_ignoreFace;							// Determine if face is ignored
	bool m_ignoreFog;							// Determine if fog is ignored
	bool m_ignoreRay;							// Determine if ray is ignored

};

#endif