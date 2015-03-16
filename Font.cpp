//****************************************************************************
//
// File: Font.cpp
// Programmer: T.J. Eason
// Project: Game Engine
// Description:  Font renderer draws characters using text quads
// Date: 2-15-10
// Revision 1: 2-16-10
// Revision 2: 2-17-10
// Revision 3: 2-19-10
// Revision 4: 3-25-10
// Reference: Microsoft Developer's Network & Direct X Manual
//
// ****************************************************************************

#include "Engine.h"

// Font class contructor
Font::Font( char *name, short size, unsigned long bold, bool italic )
{
	HBITMAP bitmap = NULL;
	HGDIOBJ oldBitmap = NULL;
	HFONT oldFont	= NULL;
	BYTE *dstRow = NULL;

	unsigned long x, y;

	HDC hDC = CreateCompatibleDC( NULL );

	SetMapMode( hDC, MM_TEXT );

	// Create font
	char height = -MulDiv( size, GetDeviceCaps( hDC, LOGPIXELSY ), 72 );
	HFONT font = CreateFont( height, 0, 0, 0, bold, italic, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, name );

	// Check if font is created
	if( font == NULL )
		goto End;

	oldFont = ( HFONT ) SelectObject( hDC, font );

	m_textureWidth = m_textureHeight = 128;

	// Find dimensions of smallest texture to hold characters
	while( !PrepareFont( hDC, true ) )
	{
		m_textureWidth *= 2;
		m_textureHeight *= 2;
	}

	// Create new texture for font
	if( FAILED( g_engine ->GetDevice() ->CreateTexture( m_textureWidth, m_textureHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_texture, NULL ) ) )
		goto End;

	// Prepare bitmap
	unsigned long *bitmapBits;

	BITMAPINFO bmi;

	ZeroMemory( &bmi.bmiHeader, sizeof( BITMAPINFOHEADER ) );

	bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth = ( int )m_textureWidth;
	bmi.bmiHeader.biHeight = -( int ) m_textureHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	// Create bitmap for font
	bitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS, (void** ) &bitmapBits, NULL, 0 );

	oldBitmap = SelectObject( hDC, bitmap );

	// Set text properties
	SetTextColor( hDC, RGB( 255, 255, 255 ) );
	SetBkColor( hDC, 0x00000000 );
	SetTextAlign( hDC, TA_TOP );

	// Prepare font by drawing charachters onto bitmap
	if( !PrepareFont( hDC ) )
		goto End;

	// Lock surface and write alpha values for set pixels
	D3DLOCKED_RECT d3dlr;
	m_texture ->LockRect( 0, &d3dlr, 0, 0 );
	dstRow = ( BYTE* )d3dlr.pBits;
	WORD *dst16;
	BYTE alpha;

	// Move to each y-axis pixel
	for( y = 0;  y < m_textureHeight; y++ )
	{
		dst16 = ( WORD* ) dstRow;

		// Move to each x-axis pixel
		for( x = 0; x < m_textureWidth; x++ )
		{
			alpha = ( BYTE ) ( ( bitmapBits[ m_textureWidth * y + x ] & 0xff ) >> 4 );

			if( alpha > 0 )
				*dst16++  = ( WORD ) ( ( alpha << 12 ) | 0xfff );
			
			else
				*dst16++ = 0x0000;
		}

		dstRow += d3dlr.Pitch;
	}

	// Create vertex buffer for characters
	g_engine ->GetDevice() ->CreateVertexBuffer( 1020 * sizeof( TLVertex ), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_vb, NULL );

	// Prepare alpha testing to render characters
	g_engine ->GetDevice() ->SetRenderState( D3DRS_ALPHAREF, 0x08 );
	g_engine ->GetDevice() ->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	// Create state block to capture volatile render states
	g_engine ->GetDevice() ->BeginStateBlock();
	g_engine ->GetDevice() ->SetRenderState( D3DRS_LIGHTING, false );
	g_engine ->GetDevice() ->SetRenderState( D3DRS_ALPHATESTENABLE, false );
	g_engine ->GetDevice() ->SetRenderState( D3DRS_FOGENABLE, false );
	g_engine ->GetDevice() ->EndStateBlock( &m_states );

	// Clean up and return
End:
	// Unlock texture
	if( m_texture )
		m_texture ->UnlockRect( 0 );

	SelectObject( hDC, oldBitmap );
	SelectObject( hDC, oldFont );

	DeleteObject( bitmap );
	DeleteObject( font );

	DeleteDC( hDC );

}
	
// Font class destructor
Font::~Font()
{
	//	SAFE_RELEASE( m_states );
	if( m_states )
	{
		m_states ->Release();
		m_states = NULL;
	}

	// SAFE_RELEASE( m_vb );
	if( m_vb )
	{
		m_vb ->Release();
		m_vb = NULL;
	}

	// SAFE_RELEASE( m_texture );
	if( m_texture )
	{
		m_texture ->Release();
		m_texture = NULL;
	}

}

// Prepare font drawing all characters
bool Font::PrepareFont( HDC hDC, bool measure )
{
	SIZE size;
	char string[2];

	// Find space between characters based on height
	if( GetTextExtentPoint32( hDC, string, 1, &size ) == 0)
		return false;

	m_spacing = ( short ) ceil( size.cy * 0.1f );

	// Set position to start draw
	unsigned long x = m_spacing;
	unsigned long  y = 0;

	// Draw each character on DC and move to next position
	for( char c = 32; c < 127; c++ )
	{
		string[0] = c;

		// Find space between character based on height
		if( GetTextExtentPoint32( hDC, string, 1, &size ) == 0 )
			return false;

		// Check width and spacing to make sure text does not overlapp
		if(  ( unsigned long ) ( x + size.cx + m_spacing ) > m_textureWidth )
		{
			x = m_spacing;
			y +=size.cy + 1;
		}

		// Make sure there is enough room for character
		if( !measure )
		{
			// Draw character string by selecting current font
			if( ExtTextOut( hDC, x + 0, y + 0, ETO_OPAQUE, NULL, string , 1, NULL ) == 0 )
				return false;

			m_textureCoords[c-32][0] =( float ) ( x - m_spacing ) / m_textureWidth;
			m_textureCoords[c-32][1] = ( float ) ( y ) / m_textureHeight;
			m_textureCoords[c-32][2] = ( float ) ( x + size.cx + m_spacing ) / m_textureWidth;
			m_textureCoords[c-32][3] = ( float ) ( y + size.cy ) / m_textureHeight;
		}

		x += size.cx + ( 2 * m_spacing );
	}

	return true;
}

// Render text using selected font
void Font::Render( char *text , float x , float y, D3DCOLOR color )
{
	// Capture current volatile render states
	m_states ->Capture();

	// Set volatile render states
	g_engine ->GetDevice() ->SetRenderState( D3DRS_LIGHTING, false );
	g_engine ->GetDevice() ->SetRenderState( D3DRS_ALPHATESTENABLE, true );
	g_engine ->GetDevice() ->SetRenderState( D3DRS_FOGENABLE, false );

	//  Set non-volatile render states
	g_engine ->GetDevice() ->SetTexture( 0, m_texture );
	g_engine ->GetDevice() ->SetFVF( TL_VERTEX_FVF );
	g_engine ->GetDevice() ->SetStreamSource( 0, m_vb, 0, TL_VERTEX_FVF_SIZE );

	// Adjust character spacing
	x -= m_spacing;
	float startX = x;

	// Fill vertex buffer
	TLVertex* vertices = NULL;
	unsigned long totalTriangles = 0;

	m_vb ->Lock( 0, 0, ( void** )&vertices, D3DLOCK_DISCARD );

	// For each letter in the text, add texture quad to vertex buffer
	while( *text )
	{
		char c = *text++;

		// Check for newline
		if( c == _T( '\n' ) )
		{
			x = startX;
			y += ( m_textureCoords[0][3] - m_textureCoords[0][1] ) * m_textureHeight;
		}

		// Check for non characters
		if( ( c - 32 ) < 0 || (  c - 32 ) >= 96 )
			continue;

		float tx1 = m_textureCoords[c-32][0];
		float ty1 = m_textureCoords[c-32][1];
		float tx2 = m_textureCoords[c-32][2];
		float ty2 = m_textureCoords[c-32][3];

		float w = ( tx2 - tx1 ) * m_textureWidth;
		float h = ( ty2 - ty1 ) * m_textureHeight;

		// Check for white space
		if( c != _T( ' ' ) )
		{
			*vertices++ = TLVertex( D3DXVECTOR4( x - 0.5f, y + h - 0.5f, 0.0f, 1.0f ), color, tx1, ty2 );
			*vertices++ = TLVertex( D3DXVECTOR4( x - 0.5f, y - 0.5f, 0.0f, 1.0f ), color, tx1, ty1 );
			*vertices++ = TLVertex( D3DXVECTOR4( x + w - 0.5f, y + h - 0.5f, 0.0f, 1.0f ), color, tx2, ty2 );
			*vertices++ = TLVertex( D3DXVECTOR4( x + w - 0.5f, y - 0.5f, 0.0f, 1.0f ), color, tx2, ty1 );
			*vertices++ = TLVertex( D3DXVECTOR4( x + w - 0.5f, y + h - 0.5f, 0.0f, 1.0f ), color, tx2, ty2 );
			*vertices++ = TLVertex( D3DXVECTOR4( x - 0.5f, y - 0.5f, 0.0f, 1.0f ), color, tx1, ty1 );

			totalTriangles += 2;

			// Triangles are completly buffered
			if( totalTriangles == 340 )
			{
				// Unlock, render, and relock vertex buffer
				m_vb ->Unlock();
				g_engine ->GetDevice() ->DrawPrimitive( D3DPT_TRIANGLELIST, 0, totalTriangles );

				vertices = NULL;
				
				m_vb ->Lock( 0, 0, ( void** ) &vertices, D3DLOCK_DISCARD );

				totalTriangles = 0;
			}
		}

		x += w - ( 2 * m_spacing );
	}

		// Unlock vertex buffer
		m_vb ->Unlock();

		// Render vertex buffer
		if( totalTriangles > 0 )
			g_engine ->GetDevice() ->DrawPrimitive( D3DPT_TRIANGLELIST, 0, totalTriangles );

		// Restore volatile render states
		m_states ->Apply();

}
