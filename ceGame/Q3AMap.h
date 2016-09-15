#ifndef CRIMSON_Q3AMAP_H
#define CRIMSON_Q3AMAP_H

#pragma once



#include <vector>

#include "SurfaceFlags.h"

#include "Q3MapDefenitions.h"

#include "VertexBuffer_IE.h"
#include "IndexBuffer.h"

#include "Renderable.h"

#include "Light_IE.h"
//#include "ZedTracker.h"

#include "Frustum.h"

#include "PhysicsModel.h"

#include "BSP_PhysicsModel.h"

typedef std::vector<int> LEAFS_INDICES_VECTOR;
typedef std::vector<int>::iterator LEAFS_INDICES_VECTOR_I;

#include "SoundAPI.h"
extern CSoundAPI g_SoundAPI;


#define Q3_MAP_TEXTURES_MAX 2048

#include "Entity.h"
#include "ControllableEntity.h"
#include "Server.h"
#include "SoundManager.h"

#define MAX_ENTITIES_NUM 3000

#define COLLIDED_PLANES_MAX 10

#define PHYSICS_FRAMES_NUM 45
//#define PHYSICS_FRAMES_NUM 75

#define STAIR_MOVE_Y 20.f

#define TRIGGER_TOUCH_NONE      0
#define TRIGGER_TOUCH_ONE_SIDE  1
#define TRIGGER_TOUCH_TWO_SIDES 2






#define WEAPON_TYPE_NOT_INITED -1
#define WEAPON_TYPE_ROCKET      0
#define WEAPON_TYPE_FIREBALL    1
#define WEAPON_TYPE_BLUE        2 
#define WEAPON_TYPE_WHITE       3 //
#define WEAPON_TYPE_LIGHTNING   4 //



#define WEAPON_ROCKET_SPEED 1300.f
#define WEAPON_ROCKET_DAMAGE 20.f

#define WEAPON_FIREBALL_SPEED 1300.f
#define WEAPON_FIREBALL_DAMAGE 50.f


#define WEAPON_BLUE_SPEED 1300.f
#define WEAPON_BLUE_DAMAGE 50.f


#define WEAPON_WHITE_SPEED 1300.f
#define WEAPON_WHITE_DAMAGE 50.f


class CQuake3BSP
{

public:

	//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	// Constructer
	//////////////////////////// CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	CQuake3BSP();

	//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	// Destructer
	//////////////////////////// ~CQuake3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	~CQuake3BSP();

    //////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	// load the q3 bsp map from file
	//////////////////////////// LoadBSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	bool LoadBSP(const char *strFileName);
	

	void UpdateLevelEntitiesPhysics(float fDeltaTime);

	//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// push all the vertexbuffers to the renderer (used mainly for debugging)
	//////////////////////////// PushAllToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void PushAllToRenderer(void);

	//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// push ONLY the bezier patches
	//////////////////////////// PushAllBezierPatchesToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void PushAllBezierPatchesToRenderer(void);


	//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// push a single face to the renderer
	//////////////////////////// PushFaceToRenderer \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void PushFaceToRenderer(UINT uiNum);
	
	//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// returns the leaf index for a position in space
	//////////////////////////// FindLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	int FindLeaf(const CVec3f &vPos);


	//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// fills the recieved leaf indices with the leafs that intersect with the given bounding sphere
	//////////////////////////// FindLeafs \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void FindLeafs(LEAFS_INDICES_VECTOR& fillMe,const CVec3f &vPos,float fRadius)
	{
		FindLeafs_1(fillMe,vPos,fRadius,0);
	}

	void FindLeafs_1(LEAFS_INDICES_VECTOR& fillMe,const CVec3f &vPos,float fRadius,int iNode);

	
	//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// is "test" cluster is potentially visible from "current" cluster?
	//////////////////////////// IsClusterVisible \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	inline int IsClusterVisible(int current, int test);

	/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// render a leaf
	/////////////////////////// RenderLeaf \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void RenderLeaf( int iLeaf, bool CheckFrustum );

	/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// render the BSP tree
	/////////////////////////// RenderTree \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void RenderTree( int Node, bool CheckFrustum );

	////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// render the Q3 BSP level
	////////////////////////// RenderLevel \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void RenderLevel(const CVec3f &vPos);

	// renders only shadow casters
	void RenderLevel_ShadowCasters(const CVec3f &vPos);


	void UpdateCulledEntitiesRenderModels(void);


	ILight_Export** GetLights();
	int GetLightsNum() { return m_iLightsNum;};

	void RenderAllFacesBBox(void);


	void RenderFaceBBox(int iFaceNum);

	void RenderAllEntitiesBBoxes(void);

	/*void RenderMapModels(void);
	void DrawMapModelsBBoxes(void);*/
	void DrawMapLeafsBBoxes(void);

	////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// set the map file name
	////////////////////////// SetMapFileName \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void SetMapFileName(const char* pcSet);


	void SetUpdateEntitiesRenderModels(bool bUpdate) { m_bUpdateEntitiesRenderModels = bUpdate; };

	void SetDepthFirstMode(bool bDepthFirst) { m_bDepthFirstMode = bDepthFirst;};

	void SetPrebatching(bool bPreBatch) { m_bPrebatching = bPreBatch;};
	
	//////////////
	//Debug stuff
	//////////////

	//void OutPutShadersToFile(void);


	float GetDistanceToFloor(const CVec3f& vPos,CEntity* pEntity);

	void TraceEntity(DWORD dwTriggerTouch,const CVec3f& vStart, const CVec3f& vEnd, CEntity* pMe, float& fMoveTillHit_Min, CVec3f& vHitNormal_Min, CEntity*& pFirstHitEntity);

	bool IsFacingStair(const CVec3f& vPos, const CVec3f& vMove,IPhysicsModel* pPM);


	void SetFrustum(CFrustum* pFrustum);

	

	int GetMaxTrisSharingMaterialNum() { return m_iMaxTrisSharingMaterial;};
	CRenderable& GetRenderable(int iNum) { return m_pRenderables[iNum];};


	void LinkEntityToEntitiesMap(CEntity* pEntity);


	void LinkRenderablesToLights(void);
	void LinkLevelEntitiesToLights(bool bForceCheckAll);
	
	
	
	CVec3f ClipVelocity (const CVec3f& in, const CVec3f& normal, float overbounce);

	DWORD CollideAndResponse(CEntity* pEntity);

	DWORD CollideAndResponse_helper(CEntity* pEntity, int iDepth);


	void LinkEntityToLeafs(CEntity* pEntity);

	void UnLinkEntityFromLeafs(CEntity* pEntity);

	void ReLinkMapEntitiesToLeafs(bool bForceReLinkAll);

	

		/*CVec3f* m_pStartPositions;
	int       m_iStartPositionsNum;*/

	////////////////////////////
	// collision detection
	////////////////////////////
	//CTraceResult m_OutputMoveData; // hold the temporary collision data

	//CVec3f m_vInputRayStart;
	//CVec3f m_vInputRayEnd;


	CTraceRequest *m_CurrentTraceInput;
	CTraceResult     *m_CurrentTraceOutput;

	tBSPBrush *m_pBrushThatWasLastHit;

	tBSPLeaf    *m_pLeafs;         // leafs array


	int GetStartPositionsNum(void){return m_iStartPositionsNum;};
	CVec3f* GetStartPositions(void) { return m_pStartPositions;};
	float*  GetStartAngles(void) { return m_fStartAngles;}; 
	

	CEntity* GetEntity(unsigned int uiID);

	void DebugApplyForceOnAllNonStaticEntities(const CVec3f& vVec);

	//void RenderEntity(unsigned int uiID,float fDeltaTime);

	DWORD AddPlayer(tPlayerGameState* pPlayer);

	DWORD SpawnParticleSystem(CParticleSystem* pPS,CEntity* pParent);

	DWORD SpawnProjectile(DWORD dwProjectileType, const CVec3f& vPos,const CVec3f& vVel,DWORD dwTriggeringEntityID);

	void RespawnPlayer(unsigned int uiPlayerEntityID,int iStartPos);

	//void RemoveEntity(DWORD dwEntityID);

	void UnregisterEntity(unsigned int uiEntityID);
	

	void SetRenderHumanPlayer(bool bSet) { m_bRenderHumanModel = bSet;};
	bool GetRenderHumanPlayer(void) const { return m_bRenderHumanModel;};


	//FSOUND_SAMPLE* GetExplotionSound(void) { return m_pRocketExplotionSound_TOUCH;};

private:

	

	//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	///// Fill the VertexBuffer Array
	//////////////////////////// FillVBArray \\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void FillVBArray(DWORD dwGenerate);

	////////////////////////
	//Collision Detection
	////////////////////////

/*
	/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	////// recuresly slides along the planes
	/////////////////////////// TraceAndResponse \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	bool TraceAndResponse(CTraceRequest* input,CTraceResult* output, int iDepth, CEntity* pCausedByEntity);
	
    
	/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	////// send a trace down the bsp tree and check against the brushes
	/////////////////////////// Trace \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	void Trace(CTraceRequest* input,CTraceResult* output,bool bActivateEntities,bool bCheckDynamicEntities,CEntity* pCausedByEntity);

	*/

	bool TestModelAgainstLeaf(int iModelNum,int iLeafNum);

	bool TestBBoxAgainstLeaf(CVec3f& vMin,CVec3f& vMax,int iLeafNum);

	



	/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	////// recurse check the bsp tree by nodes
	/////////////////////////// CheckNode \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	//void CheckMoveByNode(int iNodeIndex, float fStartFraction, float fEndFraction, CVec3f vStart, CVec3f vEnd,bool bActivateEntities,int iRecurseDepth);


	///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
	////// check against the brush
	///////////////////////////// CheckBrush \\\\\\\\\\\\\\\\\\\\\\\\\*
	//bool CheckBrush(tBSPBrush *brush);

	

	int GetModelsNum(void) { return m_iMapModelsNum;};


	

	/*void FillStaticEntities(void);
	void LinkAllStaticEntitiesToLeafs(void);
	void UpdateAllStaticEntities(void);*/

	
	void LoadMapEntities(void);

	
	void ConvertFromQuakeSpaceToD3DSpace(CVec3f& vec);

	int GenerateBrushPlanesFromBezierPatch(int iFaceNum, OUT CPlane* pPlanes);
	
	int GetRequiredBrushesNum(int iFaceNum);

	/*void LinkEntitiesToLeafs(int iEntitiesNum, CEntity** ppEntities);*/

	bool DoPointsOnlyChangeInOneDimention(CVec3f& v1, CVec3f& v2, CVec3f& v3);


	CVec3f FindMin(CVec3f* pVecs, UINT* pIndices, UINT count);

	CVec3f FindMax(CVec3f* pVecs, UINT* pIndices, UINT count);

	
	//void UpdateLinkDynamicEntities(void);

	//void BruteDrawDynamicEntities();

	static const int COLLISION_BEZIER_PATCH_TESS = 3;

	//static const int PLANES_PER_BEZIER_BRUSH = 6*COLLISION_BEZIER_PATCH_TESS;//12;
	static const int PLANES_PER_BEZIER_BRUSH = 6;


	char* m_pcMapFileName;      // map file name


	///////////////////////////////////////////////////////////////////////////////////////
	// RENDERING RELATED

	bool m_bPrebatching;
	bool m_bUpdateEntitiesRenderModels;

	bool m_bRenderHumanModel;
	

	CVec3f m_CameraPosition; // position of the camera

	int m_iVertsNum;			// number of verts in the model
	int m_iFacesNum;			// number of faces in the model
	int m_iTexturesNum;		// number of texture maps
	int m_iLightmapsNum;		// number of light maps

	int m_iLeafsNum;           // number of leafs in the BSP tree 
	int m_iLeafFacesNum;       // number of leaf faces
	

	int m_iIndicesNum;         // num of indices on indices on pIndexArray
	
	int m_iMapModelsNum;
	

	tBSPVertex  *m_pVerts;		// The object's vertices
	tBSPFace    *m_pFaces;		// The faces information of the object

	

	//////// temp arrays used for the creation of the renderables bboxes
	//////// (to avoid reading back from vertex/index buffers...

	UINT** m_pSharedMaterialIndices_TEMP;
	CVec3f* m_pBigVertexBuffer_TEMP;
	


	IIndexBuffer*  m_pIndexBuffer;
	IIndexBuffer*  m_pIndexBuffer_Dynamic;
	IVertexBuffer_Export* m_pVertexBuffer;

	IIndexBuffer*  m_pIndexBuffer_Bezier;
	//IVertexBuffer_Export* m_pVertexBuffer_Bezier;

	UINT*	m_pIndicesData;
	UINT*	m_pIndicesData_Bezier;

	//a vector of visible faces
	typedef std::vector<int>						m_VisibleFaces_Vec;
	typedef m_VisibleFaces_Vec::iterator	m_VisibleFaces_Vec_IT;

	m_VisibleFaces_Vec m_VisibleFaces;


	int			m_iTotalVerticesNum_Simple;

	IIndexBuffer** m_ppIndexBuffersOfSharedMaterials;
	CRenderable*   m_pSharedMaterialRenderables;

	bool	m_bDepthFirstMode;
		

	CRenderable* m_pRenderables;

	int      m_iPlanesNum_Original;          // number of planes
	CPlane   *m_pPlanes;

	int        m_iNodesNum;           // number of nodes in the BSP tree
	tBSPNode*  m_pNodes;         // nodes array  

	int         *m_pLeafFaces;     // leaf faces indices array
	tBSPVisData  m_clusters;	   // the clusters

	tBSPTexture *m_pTextures;      // BSP texture (filename,content,flags)
	bool*        m_pUsedTextures;

	int m_iMaxTrisSharingMaterial;
	
	UINT m_uiShadersIDs[Q3_MAP_TEXTURES_MAX]; // shader IDs
								
	CBitset m_FacesDrawn;		// The bitset for the faces that have/haven't been drawn
	// end of RENDERING RELATED
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
	// Collision 
	
	
	

	/*tBSPBrushORIGINAL *m_pBrushesORIGINAL;         // brushes array	
	tBSPPlane         *m_pPlanesORIGINAL;        // planes array*/
	    

	tBSPModel *m_pMapModels;
	

	CVec3f* m_pStartPositions;
	float*  m_fStartAngles;
	int     m_iStartPositionsNum;


	int    m_iCurrentCollidedPlanes;
	CVec3f m_CollidedPlanesNormals[COLLIDED_PLANES_MAX];

	CFrustum* m_pFrustum;
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// relatd to ENTITIES
	int         m_EntitiesStringLength;
	char        *m_strEntities;            // our Entities String

	int          m_iEntitiesNum;
	//CEntity**    m_ppEntities;
	ENTITIES_MAP m_LevelEntities;

	CEntity*       m_pPhysicsModelEntity; 
	CBSP_PhysicsModel* m_pPhysicsModel;
	////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////
	// related to LIGHTS 
			
	ILight_Export** m_ppLights;
	int         m_iLightsNum;

	bool m_bRenderShadowCastersMode;

	////////////////////////////////////////////////////////////////////
		
	// std map from material id key to list of visible faces
	// it is used in the context of minimizing drawcalls (one per material type)

	std::map<int,int> m_BezierFaceToBezierBrush;
	std::vector<BBox> m_BezierFaceBBoxes;


	/*FSOUND_SAMPLE* m_pRocketLaunchSound_TOUCH;
	FSOUND_SAMPLE* m_pRocketExplotionSound_TOUCH;
	FSOUND_SAMPLE* m_pBlueLaunch_TOUCH;*/
	CSmartPointer<CSound> m_spSoundsTOUCHING[50];
	
};


#endif
