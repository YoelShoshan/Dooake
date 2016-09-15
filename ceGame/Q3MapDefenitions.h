#ifndef CRIMSON_QUAKE3_MAP_DEFENITIONS
#define CRIMSON_QUAKE3_MAP_DEFENITIONS


#pragma once

#define Q3FACE_SIMPLE_POLYGON  1
#define Q3FACE_BEZIER_PATCH    2
#define Q3FACE_MESH_POLYGON    3
#define Q3FACE_BILLBOARD       4

#include "Math3d.h"
#include "ce.h"

struct tVector3i
{
	int x, y, z;
};

// the bsp file header
struct tBSPHeader
{
    char strID[4];				// should be "IBSP"
    int version;				// This should be 0x2e for Quake 3 files
}; 

// This is our BSP lump structure
struct tBSPLump
{
	int offset;					// The offset into the file for the start of this lump
	int length;					// The length in bytes for this lump
};


// This is our BSP vertex structure
struct tBSPVertex
{
    CVec3f vPosition;			// (x, y, z) position. 
    CVec2f vTextureCoord;		// (u, v) texture coordinate
    CVec2f vLightmapCoord;	// (u, v) lightmap coordinate
    CVec3f vNormal;			// (x, y, z) normal vector
    unsigned char color[4];				// RGBA color for the vertex 
};


// This is our BSP face structure
struct tBSPFace
{
    int textureID;				// The index into the texture array 
    int effect;					// The index for the effects (or -1 = n/a) 
    int type;					// 1=polygon, 2=patch, 3=mesh, 4=billboard 
    int startVertIndex;			// The starting index into this face's first vertex 
    int numOfVerts;				// The number of vertices for this face 
    int meshVertIndex;			// The index into the first meshvertex 
    int numMeshVerts;			// The number of mesh vertices 
    int lightmapID;				// The texture index for the lightmap 
    int lMapCorner[2];			// The face's lightmap corner in the image 
    int lMapSize[2];			// The size of the lightmap section 
    CVec3f lMapPos;			// The 3D origin of lightmap. 
    CVec3f lMapVecs[2];		// The 3D space for s and t unit vectors. 
    CVec3f vNormal;			// The face normal. 
    int size[2];				// The bezier patch dimensions. 
};


// This is our BSP texture structure
struct tBSPTexture
{
    char strName[64];			// The name of the texture w/o the extension 
    int flags;					// The surface flags
    int contents;				// The content flags
};

// This is our BSP lightmap structure which stores the 128x128 RGB values
struct tBSPLightmap
{
    unsigned char imageBits[128][128][3];   // The RGB data in a 128x128 image
};


// This stores a node in the BSP tree
struct tBSPNode
{
    int plane;					// The index into the planes array 
    int front;					// The child index for the front node 
    int back;					// The child index for the back node 
    tVector3i min;				// The bounding box min position. 
    tVector3i max;				// The bounding box max position. 
}; 

// This stores a leaf (end node) in the BSP tree
struct tBSPLeafORIGINAL
{
    int cluster;				// The visibility cluster 
    int area;					// The area portal 
    tVector3i min;				// The bounding box min position 
    tVector3i max;				// The bounding box max position 
    int leafface;				// The first index into the face array 
    int numOfLeafFaces;			// The number of faces for this leaf 
    int leafBrush;				// The first index for into the brushes 
    int numOfLeafBrushes;		// The number of brushes for this leaf 
}; 

#include <map>

class CEntity;
typedef std::map<unsigned int,CEntity*>           ENTITIES_MAP;
typedef std::map<unsigned int,CEntity*>::iterator ENTITIES_MAP_I;
typedef std::map<unsigned int,CEntity*>::reverse_iterator ENTITIES_MAP_IR;

// extended version of the original leafs, with added entities vector
struct tBSPLeaf
{
    int cluster;				// The visibility cluster 
    int area;					// The area portal 
    tVector3i min;				// The bounding box min position 
    tVector3i max;				// The bounding box max position 
    int leafface;				// The first index into the face array 
    int numOfLeafFaces;			// The number of faces for this leaf 
    
	int leafBrush;				// The first index into the brushes 
    int numOfLeafBrushes;		// The number of brushes for this leaf 

	int leafBezierBrush;        // The first index into the Bezier Patches "Artificial" brushes
	int numOfBezierLeafBrushes;	// The number of bezier brushes for this leaf 

	ENTITIES_MAP m_Entities;
}; 


// This stores a splitter plane in the BSP tree
struct tBSPPlane
{
    CVec3f vNormal;			// Plane normal. 
    float d;					// The plane distance from origin 
};

// This stores the cluster data for the PVS's
struct tBSPVisData
{
	int numOfClusters;			// The number of clusters
	int bytesPerCluster;		// The amount of bytes (8 bits) in the cluster's bitset
	unsigned char *pBitsets;				// The array of bytes that holds the cluster bitsets				
};


//	The brushes store information about a convex volume, which are defined by the brush sides.
//	Brushes are used for collision detection.  This allows the level editor to decide what is
//	collidable and what can be walked through, such as trees, bushes or certain corners.

struct tBSPBrushORIGINAL
{
        int brushSide;           // The starting brush side for the brush
        int numOfBrushSides;     // Number of brush sides for the brush
        int textureID;           // The texture index for the brush
};




struct tBSPBrush
{

	// relevant for both

	int numOfBrushSides;     // Number of brush sides for the brush (or number of planes in the dynamic brush)


	// a brush inside the map

        int brushSide;           // The starting brush side for the brush        
        int textureID;           // The texture index for the brush   - if textureID = -1 then dynamic entity

	// some other "dynamic" brush

		CPlane* m_pPlanes;

		int     m_iAbsoluteEntityIndex;
};



//The brush sides lump stores information about the brush bounding surface.
//To calculate the number of brush sides, just divide the lumps
//length by sizeof(tBSPBrushORIGINALSides).
struct tBSPBrushORIGINALSide
{
        int plane;		// The plane index
        int textureID;	// The texture index
};

/*struct tBSPShaderFile  // THIS IS ONLY FOR VOLUMETRIC FOG OR STH LIKE THIS!!!!!!!!!
{
    char strName[64];     // The name of the shader file 
    int brushIndex;       // The brush index for this shader 
    int unknown;          // This is 99% of the time 5
};*/ 

struct tBSPShaderNaP
{
	char strName[64];
	char path[160];
};



struct tBSPModel 
{
	CVec3f min;			// The min position for the bounding box
	CVec3f max;			// The max position for the bounding box. */
    int faceIndex;          // The first face index in the model 
    int numOfFaces;         // The number of faces in the model 
    int brushIndex;         // The first brush index in the model 
    int numOfBrushes;       // The number brushes for the model
};



/*struct tEntityData
	{
		tEntityData():pchClassName(NULL),pchTargetName(NULL) {};
		~tEntityData() {SafeDeleteArray(pchClassName); SafeDeleteArray(pchTargetName;);
	
		CVec3f vOrigin;
		char* pchClassName;
		char* pchTargetName;
	};*/


// This is our lumps enumeration
enum eLumps
{
    kEntities = 0,				// Stores player/object positions, etc...
    kTextures,					// Stores texture information
    kPlanes,				    // Stores the splitting planes
    kNodes,						// Stores the BSP nodes
    kLeafs,						// Stores the leafs of the nodes
    kLeafFaces,					// Stores the leaf's indices into the faces
    kLeafBrushes,				// Stores the leaf's indices into the brushes
    kModels,					// Stores the info of world models
    kBrushes,					// Stores the brushes info (for collision)
    kBrushSides,				// Stores the brush surfaces info
    kVertices,					// Stores the level vertices
    kMeshVerts,					// Stores the model vertices offsets
    kShaders,					// Stores the shader files (blending, anims..)
    kFaces,						// Stores the faces for the level
    kLightmaps,					// Stores the lightmaps for the level
    kLightVolumes,				// Stores extra world lighting information
    kVisData,					// Stores PVS and cluster info (visibility)
    kMaxLumps					// A constant to store the number of lumps
};




// This is our bitset class for storing which face has already been drawn.
// The bitset functionality isn't really taken advantage of in this version
// since we aren't rendering by leafs and nodes.
class CBitset 
{

public:

	// Initialize all the data members
    CBitset() : m_bits(0), m_size(0) {}

	// This is our deconstructor
	~CBitset() 
	{
		// If we have valid memory, get rid of it
		if(m_bits) 
		{
			delete m_bits;
			m_bits = NULL;
		}
	}

	// This resizes our bitset to a size so each face has a bit associated with it
	void Resize(int count) 
	{ 
		// Get the size of integers we need
		m_size = count/32 + 1;

		// Make sure we haven't already allocated memory for the bits
        if(m_bits) 
		{
			delete m_bits;
			m_bits = 0;
		}

		// Allocate the bits and initialize them
		m_bits = new unsigned int[m_size];
		ClearAll();
	}

	// This does the binary math to set the desired bit
	void Set(int i) 
	{
		m_bits[i >> 5] |= (1 << (i & 31));
	}

	// This returns if the desired bit slot is a 1 or a 0
	int On(int i) 
	{
		return m_bits[i >> 5] & (1 << (i & 31 ));
	}

	// This clears a bit to 0
	void Clear(int i) 
	{
		m_bits[i >> 5] &= ~(1 << (i & 31));
	}

	// This initializes the bits to 0
	void ClearAll() 
	{
		memset(m_bits, 0, sizeof(unsigned int) * m_size);
	}

private:

	//holds the bits and size
	unsigned int *m_bits;
	int m_size;
};




#define TRACE_TYPE_LINE   0
#define TRACE_TYPE_SPHERE 1
#define TRACE_TYPE_BB     2

#define TRACE_CAUSED_BY_PLAYER      0
#define TRACE_CAUSED_BY_BOT         1
#define TRACE_CAUSED_BY_SHOOT       2
#define TRACE_CAUSED_BY_PROJECTILE  4
#define TRACE_CAUSED_BY_TEST        5

struct CTraceRequest
{
public:

	CTraceRequest() {iAbsoluteEntityIndexCause = -1; };

	DWORD dwTraceType;
	DWORD dwTraceCausedBy;

	// all types
	CVec3f vRayStart;
	CVec3f vRayEnd;


	int     iAbsoluteEntityIndexCause;

	// sphere

	float fRadius;
};

struct CTraceResult
{
public:

	CTraceResult() { HitFlags = 0; HitContents=0;iAbsoluteEntityIndexHit=-1;};

	float Fraction;
	CVec3f EndPoint;

	CPlane  CollisionPlane;
	int     HitFlags;
	int     HitContents;
	int     iAbsoluteEntityIndexHit;

	bool StartOut;
	bool AllSolid;
};


#endif // YOEL_QUAKE3_MAP_DEFENITIONS