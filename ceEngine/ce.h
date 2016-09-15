#ifndef CRIMSON_ENGINE_DEFINES_H
#define CRIMSON_ENGINE_DEFINES_H

#pragma once


#define CR_FLOAT1     0
#define CR_FLOAT2     1
#define CR_FLOAT3     2
#define CR_FLOAT4     3
#define CR_BYTE4      4
#define CR_FLOAT4X4   5



#define VB_MEM_GPU (1<<1)
#define VB_MEM_SYS (1<<2)
#define VB_MERGED  (1<<3)
#define VB_SINGLE  (1<<4)

#define CR_STREAM_STATIC              (1<<5)
#define CR_STREAM_DYNAMIC             (1<<6)
#define CR_STREAM_DISCARD             (1<<7)




#define CR_UNLOCKED						0
#define CR_LOCK_READ_ONLY				1
#define CR_LOCK_WRITE_ONLY				2
#define CR_LOCK_READ_WRITE				3
#define CR_LOCK_WRITE_ONLY_NO_OVERWRITE	4
#define CR_LOCK_WRITE_ONLY_DISCARD		5

#define CR_LESS					1
#define CR_LESS_OR_EQUAL		2
#define CR_EQUAL				3
#define CR_ALWAYS				4
#define CR_NEVER				5
#define CR_GREATER				6
#define CR_GREATER_OR_EQUAL		7

#define CR_NONE          0
#define CR_FRONT         1
#define CR_BACK          2

#define CR_CLEAR_COLOR     (1<<1)
#define CR_CLEAR_DEPTH     (1<<2)
#define CR_CLEAR_STENCIL   (1<<3)

#define CR_RTT_COLOR_AND_DEPTH 0
#define CR_RTT_SHADOW_MAP      1

// blendfunc values
#define CR_BF_ZERO					1
#define CR_BF_ONE					2
#define CR_BF_DST_COLOR				3
#define CR_BF_ONE_MINUS_DST_COLOR   4
#define CR_BF_SRC_ALPHA				5
#define CR_BF_ONE_MINUS_SRC_COLOR   6
#define CR_BF_DST_ALPHA             7
#define CR_BF_ONE_MINUS_DST_ALPHA   8
#define CR_BF_SRC_ALPHA_SATURATE    9
#define CR_BF_ONE_MINUS_SRC_ALPHA  10
#define CR_BF_SRC_COLOR            11

#define TECHNIQUE_RENDER_DEPTH_MAP		0xFFFF
#define TECHNIQUE_RENDER_DEPTH_FIRST	0xFFFE




enum eStreams{
	STREAM_POSITION=0,
	STREAM_COLOR,
	STREAM_NORMAL,
	STREAM_TEXCOORD0,
	STREAM_TEXCOORD1,
	STREAM_TEXCOORD2,
	STREAM_TEXCOORD3,
	STREAM_TEXCOORD4,
	STREAM_TEXCOORD5,
	STREAM_TEXCOORD6,
	STREAM_TEXCOORD7,
	STREAM_MAX
};





#define CR_TEX_UNIT_0 (1<<0)
#define CR_TEX_UNIT_1 (1<<1)
#define CR_TEX_UNIT_2 (1<<2)
#define CR_TEX_UNIT_3 (1<<3)


#define CR_TEXTURE_OFF 0
#define CR_TEXTURE_1D  1
#define CR_TEXTURE_2D  2

#define CR_MAX_TEX_UNITS 4

#define CR_STREAM_NON_INIT 0xFFFF


// directions to the render function
#define CR_PASS_RENDER_ALL_PASSES     0 
#define CR_PASS_RENDER_AMBIENT        1
#define CR_PASS_RENDER_LIGHT          2


// type of pass
#define MAT_PASS_LIGHTING_DEFAULT     0 
#define MAT_PASS_LIGHTING_AMBIENT  1
#define MAT_PASS_LIGHTING_LIGHT    2



#define CR_NEAR_Z 7.f
#define CR_FAR_Z 10000.f

/*#define CR_NEAR_Z 1.f
#define CR_FAR_Z 100.f*/



#define SAFE_RELEASE(x) if (x) x->Release()
 

#define CE_DONT_FORCE_CULLING_METHOD 0xFFFF

#define CR_API_GL    0
#define CR_API_D3D   1

//#define CR_API_USED CR_API_GL
//#define CR_API_USED CR_API_D3D


#define SafeRelease(pObject) if(pObject != NULL) {pObject->Release(); pObject=NULL;} 


//#define PERF_HUD
#define D3D_DEBUG_INFO

//#include "Math3D.h"
///////////////////////////////


// a safe delete for single
#define SafeDelete(x) { if (x) delete x; x=NULL; };

// a safe delete for an array
#define SafeDeleteArray(x) {if (x!=NULL) delete[] x; x=NULL;}

/////////////////////////
// Debug
////////////////////////

 #include <stdio.h>

#define BREAKPOINT_NO_LOG(x)    if (x) __asm int 3
//#define BREAKPOINT(x)    if (x) {}  // a dummy, for the release
#define BREAKPOINT(x) if (x) { char msg[500]; sprintf(msg,"Breakpoint met at [File: %s] [Line: %i]",__FILE__,__LINE__); g_pLogFile->OutPutPlainText(msg,LOG_MESSAGE_ERROR); {__asm int 3}};

#include "LogFile_I.h"
extern ILogFile* g_pLogFile;

#define CR_ERROR(x,y)  if (x) g_pLogFile->OutPutPlainText(y,LOG_MESSAGE_ERROR)
#define CR_WARNING3(x,y)  if (x) g_pLogFile->OutPutPlainText(y,LOG_MESSAGE_WARNING_LVL_3)
#define CR_WARNING2(x,y)  if (x) g_pLogFile->OutPutPlainText(y,LOG_MESSAGE_WARNING_LVL_2)
#define CR_WARNING1(x,y)  if (x) g_pLogFile->OutPutPlainText(y,LOG_MESSAGE_WARNING_LVL_1)
#define CR_WARNING0(x,y)  if (x) g_pLogFile->OutPutPlainText(y,LOG_MESSAGE_WARNING_LVL_0)










#endif