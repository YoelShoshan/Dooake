#ifndef CRIMSON_MD5_MESH_H
#define CRIMSON_MD5_MESH_H

#pragma once

#include "Math3d.h"
#include "Quaternions.h"
#include "Renderable_IE.h"
#include "SmartPointer.h"
#include "Material_IE.h"
#include "ce.h"

#include <set>

// REMEMBER TO CHECK ABOUT THE HEIGHTMAP !

/*diffusemap	    models/monsters/hellknight/hellknight.tga
bumpmap		addnormals(models/monsters/hellknight/hellknight_local.tga, heightmap(models/monsters/hellknight/hellknight_h.tga, 10 ) )
specularmap		models/monsters/hellknight/hellknight_s.tga*/

__declspec(align(16)) struct tMD5Vertex
{
	CVec2f tc;
	int    iWeightsIndex;
	int    iWeightsCount;
};

__declspec(align(16)) struct tMD5Weight
{
	int    iJointIndex;
	float  fBias;
	CVec3f vPosOffest;
};

__declspec(align(16)) struct tMD5BindJoint
{
	tMD5BindJoint() { };

	CQuat bindRot;
	CVec3f bindPos;
	int iParentIndex;

};


//struct tMD5Joint
__declspec(align(16)) struct tMD5Joint
{
	tMD5Joint() { 
		dwAnimFlags=0;
		iStartIndex=0;
		pchName=NULL;
		pParent=NULL;
	};

	CQuat  rot;
	CVec3f pos;



	char* pchName;
	tMD5Joint* pParent;
	int iParentIndex;
	
	DWORD dwAnimFlags;
	int   iStartIndex; // the start index into the float data array

	/*// it is possible to compute this at run time
	char chAnimatedElements;*/
};

struct tMD5SubMesh
{
	tMD5SubMesh() { ZeroMemory(this,sizeof(tMD5SubMesh));};
	~tMD5SubMesh() { SafeDeleteArray(pVerts);SafeDeleteArray(puiIndices);SafeDeleteArray(pWeights);};

	int iVertsNum;
	tMD5Vertex* pVerts;

	CVec2f* pTexCoords;

	int iIndicesNum;
	unsigned int* puiIndices;

	int iWeightsNum;
	tMD5Weight* pWeights;

	CSmartPointer<IMaterial_Export> spMaterial;
};

#define MD5_MESH_CHANNEL_LEGS  0
#define MD5_MESH_CHANNEL_TORSO 1
#define MD5_MESH_CHANNELS_NUM  2

class CMD5Mesh
{
public:
	CMD5Mesh();
	~CMD5Mesh();

	bool Load(const char* pchFileName/*,const CVec3f& vScale*/);

	int GetSubMeshesNum(void) const { return m_iSubMeshesNum; };
	int GetJointsNum(void) const { return m_iJointsNum;};

	const tMD5SubMesh* GetSubMeshes(void) const { return m_pSubMeshes;};

	const tMD5Joint*   GetJoints(void) const { return m_pJoints;};


	std::set<int> GetJointDescendents(int iIndexNum);
	int           FindJointIndex(const char* pchName);
	
	bool AddJointsToChannel(const char* pchName,int iChannelID,bool bRecurseDescendents);

	bool RemoveJointsFromChannel(const char* pchName,int iChannelID,bool bRecurseDescendents);

	void ConvertTempChannelsSetsIntoIndexArrays(void);

	const int* GetChannelJointsIndices(int iChannelNum) const;
	int        GetChannelJointsIndicesNum(int iChannelNum) const;

private:
	int     m_iJointsNum;
	tMD5Joint* m_pJoints;

	tMD5SubMesh*  m_pSubMeshes;
	int           m_iSubMeshesNum;

	std::set<int> m_TempChannels[MD5_MESH_CHANNELS_NUM];
	int*          m_ppChannels[MD5_MESH_CHANNELS_NUM];
	int           m_ChannelsIndicesNum[MD5_MESH_CHANNELS_NUM];
	

	//CRenderable* m_pRenderables;
	
};

#endif