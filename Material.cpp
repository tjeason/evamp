// ***********************************************************************
//
// File: Material.cpp
// Programmer: T.J. Eason
// Project: Game Engine 
// Description: Lighting and texture properties
// Date: 3-11-10
// Revision 1: 3-24-10
// Revision 2: 3-25-10
//
// ************************************************************************

#include "Engine.h"

// Material class constructor
Material::Material(): Resource < Material >( "noname", "nopath" )
{
	// Empty texture
	m_texture = NULL;
	m_width = 0;
	m_height = 0;

	// Set lighting properites
	m_lighting.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_lighting.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_lighting.Specular = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	m_lighting.Emissive = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f );
	m_lighting.Power = 0;

	// Ignore face flag
	m_ignoreFace = false;

	// Ignore fog flag
	m_ignoreFog = false;

	// Ignore ray flag
	m_ignoreRay = false;

}

// Material constructor
Material::Material( char *name, char *path ) : Resource< Material >( name, path )
{
	D3DXIMAGE_INFO info;

	// Load script
	Script *script = new Script( name, path );

	// Check if texture is transparent
	if( script ->GetColorData( "transparency" ) ->a == 0.0f )
	{
		// Load texture without transparency
		D3DXCreateTextureFromFileEx( g_engine ->GetDevice(), script ->GetStringData( "texture" ), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, 
																 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE, 0, &info, NULL, &m_texture );
	}

	else
	{
		// Load texture with transparency based on color value
		D3DCOLORVALUE *color = script ->GetColorData( "transparency" );
		D3DCOLOR transparency = D3DCOLOR_COLORVALUE( color ->r, color ->g, color ->b, color ->a );
		D3DXCreateTextureFromFileEx( g_engine ->GetDevice(), script ->GetStringData( "texture"), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
																 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE, transparency, &info, NULL, &m_texture );
	}

	// Store texture's width and height
	m_width = info.Width;
	m_height = info.Height;

	// Set lighting properties
	m_lighting.Diffuse =*script ->GetColorData( "diffuse" );
	m_lighting.Ambient = *script ->GetColorData( "ambient" );
	m_lighting.Specular = *script ->GetColorData( "specular" );
	m_lighting.Emissive = *script ->GetColorData( "emissive" );
	m_lighting.Power = *script ->GetFloatData( "power" );

	// Set ignore  flags
	m_ignoreFace = *script ->GetBoolData( "ignore_face" );
	m_ignoreFog = *script ->GetBoolData( "ignore_fog" );
	m_ignoreRay = *script ->GetBoolData( "ignore_ray" );

	// Delete script
	SAFE_DELETE( script );

}

// Material class destructor
Material::~Material()
{
	if( m_texture )
	{
		m_texture ->Release();
		m_texture = NULL;
	}

}

// Get texture
IDirect3DTexture9 *Material::GetTexture()
{
	return m_texture;

}

// Get lighting properties
D3DMATERIAL9 *Material::GetLighting()
{
	return &m_lighting;

}

// Get texture width
unsigned long Material::GetWidth()
{
	return m_width;

}

// Get texture height
unsigned long Material::GetHeight()
{
	return m_height;

}

// Get ignore face flag
bool Material::GetIgnoreFace()
{
	return m_ignoreFace;

}

// Get ignore fog flag
bool Material::GetIgnoreFog()
{
	return m_ignoreFog;

}

// Get ignore ray flag
bool Material::GetIgnoreRay()
{
	return m_ignoreRay;

}

