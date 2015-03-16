//****************************************************************************
//
// File: Geometry.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description:   Processing functions and geometrics
// Date: 1-20-10
// Revision 1: 1-21-10
// Revision 2: 3-25-10
// Revision 3: 4-21-10
// References: Microsoft Developer's Network & Cube Engine Manual
//
// **************************************************************************

#ifndef GEOMETRY_H
#define GEOMETRY_H

// Structure for vertex
struct Vertex
{
	D3DXVECTOR3 translation;		// Tranlsate vector in world
	D3DXVECTOR3 normal;				// Normal vector

	float tu, tv;										// Texture UV cordinates

	// Vertex constructor
	Vertex()
	{
		translation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		normal = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		tu = 0.0f;
		tv = 0.0f;

	}

	// Another vertex constructor
	Vertex( D3DXVECTOR3 t, D3DXVECTOR3 n, float u, float v )
	{
		translation = t;
		normal = n;
		tu = u;
		tv = v;

	}
};

#define VERTEX_FVF	( D3DFVF_XYZ		|	D3DFVF_NORMAL		|	D3DFVF_TEX1 ) 
#define VERTEX_FVF_SIZE D3DXGetFVFVertexSize( VERTEX_FVF )

// Lit vertex structure
struct LVertex
{
	D3DXVECTOR3 translation;		// Translate vertex into world
	D3DXCOLOR diffuse;					// Vertex color
	
	float tu, tv;										// Texture UV cordinates

	// Lit vertex constructor
	LVertex()
	{
		translation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		diffuse = 0xFFFFFFFF;
		tu = 0.0f;
		tv = 0.0f;
	}

	// Another lit vertex constructor
	LVertex( D3DXVECTOR3 t, D3DXCOLOR d, float u, float v )
	{
		translation = t;
		diffuse = d;
		tu = u;
		tv = v;

	}
};

#define L_VERTEX_FVF ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define L_VERTEX_FVF_SIZE D3DXGetFVFVertexSize( L_VERTEX_FVF )

// Transformed and Lit vertex structure
struct TLVertex
{
	D3DXVECTOR4 translation;		// Vertex translation for screen resolution
	D3DCOLOR diffuse;						// Vertex color

	float tu, tv;										// Texture UV cordinates

	// Transformed and Lit vertex constructor
	TLVertex()
	{
		translation = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
		diffuse = 0xFFFFFFFF;
		tu = 0.0f;
		tv = 0.0f;

	}

	// Anotther Transformed and Lit vertex constructor
	TLVertex( D3DXVECTOR4 t, D3DCOLOR d, float u, float v )
	{
		translation = t;
		diffuse = d;
		tu = u;
		tv = v;

	}
};

#define TL_VERTEX_FVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define TL_VERTEX_FVF_SIZE D3DXGetFVFVertexSize( TL_VERTEX_FVF )

// Edge structure
struct Edge
{
	Vertex *vertex0;		// First vertex for an edge
	Vertex *vertex1;		// Second vertex for an edge

	// Edge constructor
	Edge( Vertex *v0, Vertex *v1 )
	{
		vertex0 = v0;
		vertex1 = v1;

	}
};

// Indexed edge structure
struct IndexedEdge
{
	unsigned short vertex0;		// First vertex with an edge's index
	unsigned short vertex1;		// Second vertex with an edge's index
};

// Face structure
struct Face
{
	Vertex *vertex0;		// First vertex for face
	Vertex *vertex1;		// Second vertex for face
	Vertex *vertex2;		// Third vertex for face

	// Face constructor
	Face( Vertex *v0, Vertex *v1, Vertex *v2 )
	{
		vertex0 = v0;
		vertex1 = v1;
		vertex2 = v2;

	}
};

// Index face structure
struct IndexedFace
{
	unsigned short vertex0;		// First vertex with face's index
	unsigned short vertex1;		// Second vertex with face's index
	unsigned short vertex2;		// Third vertex with face's index
};

// Returns true if the first given box is inside the second given box.
inline bool IsBoxInBox( D3DXVECTOR3 box1Min, D3DXVECTOR3 box1Max, D3DXVECTOR3 box2Min, D3DXVECTOR3 box2Max )
{
	if( box1Min.x > box2Max.x )
		return false;
	if( box1Min.y > box2Max.y )
		return false;
	if( box1Min.z > box2Max.z )
		return false;
	if( box1Max.x < box2Min.x )
		return false;
	if( box1Max.y < box2Min.y )
		return false;
	if( box1Max.z < box2Min.z )
		return false;

	return true;
}

// Return true if a face is inside the boxed shape
inline bool IsFaceInBox( Vertex *vertex0, Vertex *vertex1, Vertex *vertex2, D3DXVECTOR3 boxMin, D3DXVECTOR3 boxMax )
{
	// Find min and max points along the face's x-axis
	float minX = min( vertex0 -> translation.x, min( vertex1 -> translation.x, vertex2 -> translation.x ) );
	float maxX = max( vertex0 -> translation.x, max( vertex1 -> translation.x, vertex2 -> translation.x ) );

	// Make sure two points are within the box's x-axis
	if( maxX < boxMin.x )
		return false;

	if( minX > boxMax.x )
		return false;

	// Find min and max points along the face's y-axis
	float minY = min( vertex0 -> translation.y, min( vertex1 -> translation.y, vertex2 -> translation.y ) );
	float maxY = max( vertex0 -> translation.y, max( vertex1 -> translation.y, vertex2 -> translation.y ) );

	// Make sure two points are within the box's y-axis
	if( maxY < boxMin.y )
		return false;

	if( minY > boxMax.y )
		return false;

	// Find min and max points along the face's z-axis
	float minZ = min( vertex0 -> translation.z, min( vertex1 -> translation.z, vertex2 -> translation.z ) );
	float maxZ = max( vertex0 -> translation.z, max( vertex1 -> translation.z, vertex2 -> translation.z ) );

	// Make sure two points are within the box's z-axis
	if( maxZ < boxMin.z )
		return false;

	if( minZ > boxMax.z )
		return false;

	return true;

}

// Retrurn true if box entirely enclosed by its volume
inline bool IsBoxEnclosedByVolume( LinkedList < D3DXPLANE > *planes, D3DXVECTOR3 min, D3DXVECTOR3 max )
{
	planes -> Iterate( true );

	// Planes exist within the box with cordinates that need to be checked
	while( planes -> Iterate() )
	{
		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &D3DXVECTOR3( min.x, min.y, min.z ) ) < 0.0f )
			return false;

		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &D3DXVECTOR3( max.x, min.y, min.z ) ) < 0.0f )
			return false;

		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &D3DXVECTOR3( min.x, max.y, min.z ) ) < 0.0f )
			return false;

		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &D3DXVECTOR3( min.x, min.y, max.z ) ) < 0.0f )
			return false;

		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &D3DXVECTOR3( max.x, min.y, max.z ) ) < 0.0f )
			return false;

		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &D3DXVECTOR3( min.x, max.y, max.z ) ) < 0.0f )
			return false;

		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &D3DXVECTOR3( max.x, max.y, max.z ) ) < 0.0f )
			return false;
	}

	return true;

}

// Return true if sphere is overlaping its volume
inline bool IsSphereOverlappingVolume( LinkedList< D3DXPLANE > *planes, D3DXVECTOR3 translation, float radius )
{
	planes -> Iterate( true );

	// Plane exists for a sphere; iterate through list and check plane coordinates 
	while( planes -> Iterate() )
		if( D3DXPlaneDotCoord( planes -> GetCurrent(), &translation ) < -radius )
			return false;

	return true;

}

// Return true if two spheres collide
inline bool IsSphereCollidingWithSphere( float *collisionDistance, D3DXVECTOR3 translation1, D3DXVECTOR3 translation2, 
										                                D3DXVECTOR3 velocitySum, float radiiSum )
{
	// Get distance between two spheres
	float distanceBetween = D3DXVec3Length( &( translation1 - translation2 ) ) - radiiSum;

	// Get length of sum of velocity vectors of two spheres
	float velocityLength = D3DXVec3Length(  &velocitySum );

	// If spheres are not touching and velocity length is less than the distance between them, then no collision
	if( distanceBetween > 0.0f && velocityLength < distanceBetween ) 
		return false;

	// Get velocity vectors' normalized sum
	D3DXVECTOR3 normalizedVelocity;
	D3DXVec3Normalize( &normalizedVelocity, &velocitySum );

	// Get direction vector from second sphere to first sphere
	D3DXVECTOR3 direction = translation1 - translation2;

	// Get angle between normalized velocity and direction vectors
	float angleBetween = D3DXVec3Dot( &normalizedVelocity, &direction );

	// Check if spheres are moving away from each other
	if( angleBetween <= 0.0f )
	{
		// Make sure they are not touching or inside each other so they are not colliding
		if( distanceBetween < 0.0f )
		{
			// Velocity length is greater than distance between spheres moving away from each other
			if( velocityLength > -distanceBetween )
				return false;
		}

		// Velocity and distance are closing in
		else
			return false;
	}

	// Get length for direction vector
	float directionLength = D3DXVec3Length( &direction );

	// Vector between two spheres velocity vector produces two sides of a triangle.  
	// Using Pythagorean theorem to find length of third side.
	float hypotenuse = ( directionLength * directionLength ) - ( angleBetween * angleBetween );

	// Make spheres come closer than the sum of their radii.
	float radiiSumSquared = radiiSum * radiiSum;
	
	if( hypotenuse >= radiiSumSquared )
		return false;

	// Get distance of the velocity vector when the spheres collide. 
	float distance = radiiSumSquared - hypotenuse;
	
	// Calculate collision distance between the two spheres
	*collisionDistance = angleBetween - ( float ) sqrt( distance );

	// Make sure the sphere will not travel more than velocity allows
	if( velocityLength < *collisionDistance )
		return false;

	return true;
}

#endif 
