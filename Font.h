//****************************************************************************
//
// File: Font.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description:  Font renderer draws characters using text quads
// Date: 2-15-10
// Revision 1: 2-24-10
// Revision 2: 3-25-10
// Reference: Microsoft Developer's Network & Direct X Manual
//
// ****************************************************************************

#ifndef FONT_H
#define FONT_H

// Font class
class Font
{
public:
	Font( char *name = "Arial", short size = 8, unsigned long bold = FW_NORMAL, bool italic = true );

	virtual ~Font();

	void Render( char *text, float x, float y, D3DCOLOR color = D3DCOLOR_COLORVALUE( 1.0f, 1.0f, 1.0f, 1.0f ) );

private:
	bool PrepareFont( HDC hDc, bool measure = false );

private:
	IDirect3DStateBlock9 *m_states;		// State block used for restoring rendering states
	IDirect3DVertexBuffer9 *m_vb;			// Vertex buffer for rendering text
	IDirect3DTexture9 *m_texture;				// Direct3D texture for the font

	unsigned long m_textureWidth;			// Texture width
	unsigned long m_textureHeight;			// Texture height

	float m_textureCoords[96][4];				// Character texture coordinates

	short m_spacing;									// Character pixel spacing per side

};

#endif

	
