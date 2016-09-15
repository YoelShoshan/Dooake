#include "stdafx.h"
#include "Material.h"
#include "Defines.h"
#include "LogFile.h"

#include "LogFile.h"
extern ILogFile* g_pLogFile;

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

#include "ceParser.h"

#include "RTTProxy.h"

#include "RendererBackend.h"
extern CRendererBackend* g_pRendererBack;

#include "ShaderUniforms.h"
extern CShaderUniforms* g_pAutoUniforms;

#include "GraphicsAPI.h"
extern IGraphicsAPI* g_pGraphicsAPI;

#include "MaterialManager.h"

#include "Profiler.h"


CMaterial::CMaterial()
{
	m_uiTechniquesNum=0;
	m_ppTechniques = new CTechnique*[CRIMSON_TECHNIQUES_MAX];
	for (int i=0;i<CRIMSON_TECHNIQUES_MAX;i++)
		m_ppTechniques[i] = NULL;

	m_uiSort = MATERIAL_SORT_OPAQUE;

	m_pcName = NULL;	


	static unsigned int s_uiID = 0;

	m_uiID = s_uiID;
	s_uiID++;
}

CMaterial::~CMaterial()
{
	char message[400];
	sprintf_s(message,400,"Deleting Material - %s...",m_pcName);
	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
	sprintf_s(message,400,"Deleting Material - %s DONE!",m_pcName); // it's here because name will be deleted later...
	

	st_MaterialManager->Unregister(m_pcName);

	for (unsigned int i=0;i<m_uiTechniquesNum;i++)
	{
		SafeDelete(m_ppTechniques[i]);
	}

	SafeDeleteArray(m_ppTechniques);
	SafeDeleteArray(m_pcName);

	g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_INFORMATIVE);
}

bool CMaterial::IsTransparent(void)
{
	if (m_uiTechniquesNum<1)
		return false;

	if (m_ppTechniques[0]->GetPassesNum()<1)
		return false;

	CPass* pPass = m_ppTechniques[0]->GetPass(0);
	
	if ( (pPass->m_RendererState.m_cBlendSource != CR_BF_ONE) ||
		 (pPass->m_RendererState.m_cBlendTarget != CR_BF_ZERO))
	{
		return true;
	}

	return false;
}

bool CMaterial::CreateMaxVD(void)
{
	const CPass* pPass;
	const IShaderProgram* pShaderProg;
	//const tMaxVD* pCurrentMaxVD;
	const CVertDecl* pMaxVD;


	for (int k=0;k<m_uiTechniquesNum;k++)
	{
		for (int j=0;j<m_ppTechniques[k]->GetPassesNum();j++)
		{
			pPass = m_ppTechniques[k]->GetPass(j);

			pShaderProg = pPass->vertexProgram.GetProgram();

			pMaxVD = pShaderProg->GetVD();

			for (int i=0;i<STREAM_MAX;i++)
			{
				//if (pCurrentMaxVD->elements[i].bActive)
				if (pMaxVD->IsStreamActive(i))
				{
					if (!m_MaxVD.elements[i].bActive) // if nothing was there 
					{
						//simply add it
						m_MaxVD.elements[i].bActive = true;
						//m_MaxVD.elements[i].chStreamDataType = pCurrentMaxVD->elements[i].chStreamDataType;
						m_MaxVD.elements[i].chStreamDataType = pMaxVD->GetStreamMap(i).chElementType;
					}
					else // if there was already something there
					{
						// check for mismatches!

						//if (m_MaxVD.elements[i].chStreamDataType != pCurrentMaxVD->elements[i].chStreamDataType)
						if (m_MaxVD.elements[i].chStreamDataType != pMaxVD->GetStreamMap(i).chElementType)
						{
							// a mismatch was found!

							char message[400];
							sprintf_s(message,400,"a conflict between incoming varying types was found! found at:%s in stream %u",
								pShaderProg->GetName(),i);	
							g_pLogFile->OutPutPlainText(message,LOG_MESSAGE_ERROR);
							return false;
						}

					}


				}
				

			}

		}


	}

	return true;
}

void CMaterial::AddVertexDeformation(CPass* pPass,const CQ3A_Shader_Data* pQ3AShader, UINT uiPassNum)
{
	CTechnique* pTech = m_ppTechniques[0];

	std::string shaderString_Start = "";
	std::string shaderString_Body = "";
	std::string shaderString_End  = "";

	
	char temp[1024];
	

	shaderString_Start += "struct s_VertexIn\n";
	shaderString_Start += "{\n";
	shaderString_Start += "float4 position      : POSITION;\n";
	shaderString_Start += "float2 TexCoord      : TEXCOORD0; // base map\n";
	shaderString_Start += "};\n";
	shaderString_Start += "struct s_VertexOut\n";
	shaderString_Start += "{\n";
	shaderString_Start += "float4 oPosition : POSITION;\n";
	shaderString_Start += "float2 TexCoord  : TEXCOORD0;\n";
	shaderString_Start += "};\n";
	shaderString_Start += "void main(in  s_VertexIn  v_in,\n";
	shaderString_Start += "in uniform float4x4 worldViewProjection,\n";
	shaderString_Start += "in uniform float time,\n";
	shaderString_Start += "out s_VertexOut v_out)\n";         	           
	shaderString_Start += "{\n";
	shaderString_Start += "float4 vPos_Curr = v_in.position;\n";

	shaderString_Start += "float t_fWaveResult = 0.f;\n";

	
	
	shaderString_Body+= "float wave_val = 0.f;\n";

	// Handle all MOVE commands - Not Tested Yet (And implementation only started)
	if (pQ3AShader->m_DVObject.m_iMoveNum>0)
	{
		shaderString_Body += "float4 t_VecWaveTotalResult = float4(0.f,0.f,0.f,0.f);\n";

		// sum up all deformVertex MOVE commands into one Vector in order to save some time
		for (int v=0; v<pQ3AShader->m_DVObject.m_iMoveNum;v++)
		{
			sprintf_s(temp,1024,"float3 t_VecWaveResult_%d = float3(0.f,0.f,0.f);\n",v);
			shaderString_Body += temp;

			//t_fWaveResult = pQ3AShader->m_DVObject.m_ppMove[v]->Compute(st_Timer::Inst()->TimerGetTimeFAKE());
			/*t_VecWaveResult.v[0] = pQ3AShader->m_DVObject.m_MoveVert[v]->v[0];
			t_VecWaveResult.v[1] = pQ3AShader->m_DVObject.m_MoveVert[v]->v[1];
			t_VecWaveResult.v[2] = pQ3AShader->m_DVObject.m_MoveVert[v]->v[2];
			t_VecWaveResult= t_VecWaveResult*t_fWaveResult;*/

			sprintf_s(temp,1024,"t_VecWaveTotalResult.xyz += t_VecWaveResult_%d;\n",v);
			shaderString_Body += temp;
		}

		/*// add that Vector into all temp vertices
		for (int s=0; s<pVB->GetSize();s++)
		{
			m_pTempVertexArray[s]+=t_VecWaveTotalResult;
		}
		*/
	}

	// Handle all WAVE commands
	//CVector3f*  t_pNormals = (CVector3f*) pVB->Get(VB_HAS_NORMALS);

	// I need to fine tune this some more!
	if (pQ3AShader->m_DVObject.m_iWaveNum>0)
	{
		//shaderString_Body += "float t_fW;\nfloat t_fT;\n";
		shaderString_Body += "float3 t_Def = float3(0.f,0.f,0.f);\n";
		
		// The concept is to push the vertices in the direction of their normal
		// Until i add passing normal in this "fallback" mode, using constant value
		shaderString_Body += "float3 t_normal = float3(1.f,0.f,0.f);\n";


		for (int v=0; v<pQ3AShader->m_DVObject.m_iWaveNum;v++)
		{			
			//sprintf_s(temp,1024,"t_fT = 1.f/%.2f*2000.f;\n",pQ3AShader->m_DVObject.m_pWaveDivs[v]);
			//shaderString_Body+=temp;

			//for (int s=0; s<pVB->GetSize();s++) //per vertex
			{
				/*t_fW = t_DVObject->m_ppWave[v]->Compute(
					st_Timer::Inst()->TimerGetTimeFAKE()+
					t_fT*(t_pVertices[s].v[0]+t_pVertices[s].v[1]+t_pVertices[s].v[2])
					);*/

				// After next line, wave_val should contain the wave value
				shaderString_Body += pQ3AShader->m_DVObject.m_ppWave[v]->Compute_String();
				shaderString_Body += "t_Def = wave_val*t_normal;\n";
				shaderString_Body += "vPos_Curr.xyz += t_Def;\n";
				//t_Def = t_pNormals[s] * t_fW;

				//m_pTempVertexArray[s] += t_Def;
			}
		}
	}


	// Not Tested Yet
	// I might need the bezier patch information!!!
	if (pQ3AShader->m_DVObject.m_iBulgeNum>0/* && pVB->GetFaceType()==VB_BEZIER_PATCH*/)
	{

		shaderString_Body += "float width,height,speed;\n";

		for (int v=0; v<pQ3AShader->m_DVObject.m_iBulgeNum;v++)
		{			
			sprintf_s(temp,1024,"width = %.2f;\n", pQ3AShader->m_DVObject.m_ppfBulgeParms[v][0]);
			shaderString_Body+= temp;

			sprintf_s(temp,1024,"height = %.2f;\n", pQ3AShader->m_DVObject.m_ppfBulgeParms[v][1]);
			shaderString_Body+= temp;

			sprintf_s(temp,1024,"speed = %.2f;\n", pQ3AShader->m_DVObject.m_ppfBulgeParms[v][2]);
			shaderString_Body+= temp;
			
			shaderString_Body+= "speed = time/(speed*width);\n";
			shaderString_Body+= "speed-= floor(speed);\n";

			//int FinalDimX = (pVB->GetBezierPatchDimX()/2) * (pVB->GetCurrentlyTesselatedLOD()+1);
			//int FinalDimY = (pVB->GetBezierPatchDimY()/2) * (pVB->GetCurrentlyTesselatedLOD()+1);

			//width/=(float) FinalDimY;

			//int	elem=0;

			//CVector3f def;
			shaderString_Body+= "float w = 0.f;\n";
			// I'm supposed to use the normal, using constant normal for now
			shaderString_Body+= "float3 t_normal_b = float3(1.f,0.f,0.f);\n";
			shaderString_Body+= "float3 t_Def_b = float3(0.f,0.f,0.f);\n";

		
			//for (int y=0;y<FinalDimY;++y)
			{
				shaderString_Body+= "w=sin(((float)y*width+speed)*6.2831853)*height;\n";
				//for (v=0;v<FinalDimX;++v)
				{	
					
					// using t_normal_b instead of next line
					//def=t_pNormals[elem]*w;
					shaderString_Body+= "t_Def_b = w*t_normal_b;\n";
		
					//m_pTempVertexArray[elem]=t_pVertices[elem]+def;

					shaderString_Body+= "vPos_Curr += t_Def_b;\n";

					//elem++;
				}
			}				
		}
	}

	
	//shaderString_End += "//v_out.oPosition = mul( worldViewProjection, v_in.position);\n";
	//shaderString_End += "float tmp_x = fmod(time,200);\n";  
	shaderString_End += "// move to clip space\n";
	shaderString_End += "v_out.oPosition = mul( worldViewProjection, vPos_Curr);\n";      
	shaderString_End += "// Pass texture coordinates through\n";
	shaderString_End += "v_out.TexCoord = v_in.TexCoord;\n";         
	shaderString_End += "}\n";
	
	std::string shaderString_Final = shaderString_Start+shaderString_Body+shaderString_End;

	//pPass->vertexProgram.LoadVP("cgShaders/BasicTextureVP-Debug1.cg");		
	pPass->vertexProgram.LoadVPFromText("dbg",shaderString_Final.c_str());
	pPass->vertexProgram.AddUniform("worldViewProjection",g_pAutoUniforms->Get("worldViewProjection"));
	pPass->vertexProgram.AddUniform("time",g_pAutoUniforms->Get("time"));

}

bool CMaterial::Generate_AddQ3AShaderPass(const CQ3A_Shader_Data* pQ3AShader, UINT uiPassNum)
{
	CTechnique* pTech = m_ppTechniques[0];

	CQ3A_ShaderPass* pQ3AShaderPass = pQ3AShader->m_ppPasses[uiPassNum];


	CPass* pPass = NULL;

	// first try - just generate one pass

	pPass = pTech->AddPass();

	int iTextureAnimNum = 0;

	CSmartPointer<ITexture> spTex;
	spTex = g_pRendererBack->LoadTexture(pQ3AShaderPass->GetTextureName(iTextureAnimNum));
	pPass->AddTexture(spTex,0);

	if (pQ3AShaderPass->GetTextureClamp(iTextureAnimNum))
		pPass->SamplerStates[iTextureAnimNum].cClamping = SS_CLAMPING_CLAMP;
	else
		pPass->SamplerStates[iTextureAnimNum].cClamping = SS_CLAMPING_REPEAT;




	/*
	vertexshader "cgShaders/BasicTextureVP.cg"
	{
	autouniform worldViewProjection
	}
	*/

	if (!pQ3AShader->m_bDVUsed)
	{
		pPass->vertexProgram.LoadVP("cgShaders/BasicTextureVP.cg");		
		pPass->vertexProgram.AddUniform("worldViewProjection",g_pAutoUniforms->Get("worldViewProjection"));
	} else // Vertex Deformation
	{
		AddVertexDeformation(pPass,pQ3AShader,uiPassNum);		
	}



	//TODO: use something like auto_generated_[script name]

	//pPass->fragmentProgram.LoadFP("cgShaders/Q3A_Shader_Base_FP.cg");	

	std::string shaderString_Start = "";
	std::string shaderString_Sample = "";
	std::string shaderString_Inject_tc = "";
	std::string shaderString_Inject_rgba = "";
	std::string shaderString_End  = "";

	shaderString_Start += "#define CE_TEX_2D(x,y) tex2D(x,y)\n";
	
	shaderString_Start+= "struct s_FragmentIn\n\n";
	shaderString_Start+= "{\n";
	shaderString_Start+= "	float4 oPosition : POSITION;\n";
	shaderString_Start+= "	float2 texCoord  : TEXCOORD0;\n";
	shaderString_Start+= "};\n";
	shaderString_Start+= "\n";


	shaderString_Start+= "void main(  in s_FragmentIn f_in,\n";
	shaderString_Start+= "in uniform sampler2D RegularTexture  :register(s0),\n";
	shaderString_Start+= "in uniform float time,\n";
	shaderString_Start+= "out float4 color : COLOR  )\n";
	shaderString_Start+= "{\n";
	shaderString_Start+= "float wave_val;\n";
	

	

	// generated code

	

	char temp[100];

	int iParamNum = 0;

	bool bDebugPrint = false;

	STCModObject* t_TCModObject = NULL;
	if (pQ3AShaderPass->GetIsTCModUsed()==true)
	{
		bDebugPrint = true;
		t_TCModObject = pQ3AShaderPass->GetTCModObject();

		for (int iCounter=(*t_TCModObject).m_iNum-1;iCounter>=0;iCounter--)
			//for (int iCounter=0;iCounter<(*t_TCModObject).m_iNum;iCounter++)
		{
			switch((*t_TCModObject).m_dwOrder[iCounter])
			{
			case TCMOD_SCALE:
				//glScalef((*t_TCModObject).m_pfScale[0],(*t_TCModObject).m_pfScale[1],1.0f);
				
				//THIS LOOKS LIKE A CRAZY CG BUG!
				// "cg unkown error" happens if it's -1.0 (which means -register in the assembly)

				sprintf(temp,"f_in.texCoord.x = f_in.texCoord.x*%.2f;\n",(*t_TCModObject).m_pfScale[0]+0.01f);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"f_in.texCoord.y = f_in.texCoord.y*%.2f;\n",(*t_TCModObject).m_pfScale[1]+0.01f);
				shaderString_Inject_tc+= temp;

				// i should probably add count - like tmp_15
				/*sprintf(temp,"float2 tmp = f_in.texCoord.xy;\n");
				shaderString_Inject_tc+= temp;

				sprintf(temp,"tmp.x = -1.0*tmp.x;\n");
				shaderString_Inject_tc+= temp;

				sprintf(temp,"tmp.y = -1.0*tmp.y;\n");
				shaderString_Inject_tc+= temp;

				sprintf(temp,"f_in.texCoord.xy = tmp;\n");
				shaderString_Inject_tc+= temp;*/
				

				break;
			case TCMOD_SCROLL:
				//break;
				//glTranslatef(Par1,-Par2,0.0f);

				sprintf(temp,"float Par1_%d= %.2f * time;\n",iParamNum,(*t_TCModObject).m_pfScroll[0]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"float Par2_%d= %.2f * time;\n",iParamNum,(*t_TCModObject).m_pfScroll[1]);
				shaderString_Inject_tc+= temp;
			
				sprintf(temp,"f_in.texCoord.xy += float2(Par1_%d,-Par2_%d);\n",iParamNum,iParamNum);
				shaderString_Inject_tc+= temp;

				
				iParamNum++;

				break;
			case TCMOD_ROTATE:
				//break;
				/*Par1=fmodf((*t_TCModObject).m_pfRotate[0] * fTimeSeconds,360.0);				
				glTranslatef(0.5f,0.5f,0.f);						
				glRotatef(-Par1,0,0,1.0f);
				glTranslatef(-0.5f,-0.5f,0);*/

				sprintf(temp,"float Par1_%d= fmod(%.2f * time,260.0);\n",iParamNum,(*t_TCModObject).m_pfRotate[0]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"Par1_%d = (Par1_%d*3.1415)/180.0;\n",iParamNum,iParamNum); 
				shaderString_Inject_tc+= temp;

				sprintf(temp,"f_in.texCoord.xy -= float2(0.5,0.5);\n");
				shaderString_Inject_tc+= temp;
				
				//rotation matrix
				// [ cos -sin ]
				// [ sin cos  ]

				sprintf(temp,"float2x2 rotMat_%d;\n",iParamNum);
				shaderString_Inject_tc+= temp;
				sprintf(temp,"rotMat_%d._m00 = cos(-Par1_%d);",iParamNum,iParamNum);
				shaderString_Inject_tc+= temp;
				sprintf(temp,"rotMat_%d._m01 = -sin(-Par1_%d);",iParamNum,iParamNum);
				shaderString_Inject_tc+= temp;
				sprintf(temp,"rotMat_%d._m10 = sin(-Par1_%d);",iParamNum,iParamNum);
				shaderString_Inject_tc+= temp;
				sprintf(temp,"rotMat_%d._m11 = cos(-Par1_%d);",iParamNum,iParamNum);
				shaderString_Inject_tc+= temp;
				sprintf(temp,"\n");
				shaderString_Inject_tc+= temp;
				sprintf(temp,"f_in.texCoord.xy = mul(rotMat_%d,f_in.texCoord.xy);\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"f_in.texCoord.xy += float2(0.5,0.5);\n");
				shaderString_Inject_tc+= temp;
				
				iParamNum++;

				break;
			case TCMOD_STRETCH:
				//break;
				/*{
					Par1 = 1.f / (*t_TCModObject).m_pStretch[0].Compute(fTimeMS);
					glTranslatef(0.5f,0.5f,0);
					glScalef(Par1,Par1,1.0f);
					glTranslatef(-0.5f,-0.5f,0);
				}*/

				shaderString_Inject_tc+= (*t_TCModObject).m_pStretch[0].Compute_String().c_str();

				sprintf(temp,"float Par1_%d = 1.f / wave_val;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"f_in.texCoord.xy -= float2(0.5,0.5);\n");
				shaderString_Inject_tc+= temp;

				sprintf(temp,"f_in.texCoord.xy *= float2(Par1_%d,Par1_%d);\n",iParamNum,iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"f_in.texCoord.xy += float2(0.5,0.5);\n");
				shaderString_Inject_tc+= temp;				

				iParamNum++;

				break;
			case TCMOD_TRANSFORM:

				// not supported for now
				/*{
					float m[16]={
						(*t_TCModObject).m_pfTransform[0],(*t_TCModObject).m_pfTransform[1],0,0,
						(*t_TCModObject).m_pfTransform[2],(*t_TCModObject).m_pfTransform[3],0,0,
						0,0,1,0,
						(*t_TCModObject).m_pfTransform[4],(*t_TCModObject).m_pfTransform[5],0,1};
						glMultMatrixf(m);
				}*/

				/*
				sprintf(temp,"float4x4 transMat_%d;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				/////////////

				sprintf(temp,"transMat_%d._m00 = %.2f;\n",iParamNum,(*t_TCModObject).m_pfTransform[0]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m01 = %.2f;\n",iParamNum,(*t_TCModObject).m_pfTransform[1]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m02 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m03 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				/////////////

				sprintf(temp,"transMat_%d._m10 = %.2f;\n",iParamNum,(*t_TCModObject).m_pfTransform[2]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m11 = %.2f;\n",iParamNum,(*t_TCModObject).m_pfTransform[3]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m12 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m13 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				/////////////

				sprintf(temp,"transMat_%d._m20 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m21 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m22 = 1;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m23 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				/////////////

				sprintf(temp,"transMat_%d._m30 = %.2f;\n",iParamNum,(*t_TCModObject).m_pfTransform[4]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m31 = %.2f;\n",iParamNum,(*t_TCModObject).m_pfTransform[5]);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m32 = 0;\n",iParamNum);
				shaderString_Inject_tc+= temp;

				sprintf(temp,"transMat_%d._m33 = 1;\n",iParamNum);
				shaderString_Inject_tc+= temp;


				iParamNum++;
				*/

				break;
			case TCMOD_TURB: // TODO: THIS IS BUGGY!!!!!!!!!!!
				{
					/*Par1=t_TCModObject->m_pfTurb[3]*fTimeSeconds+t_TCModObject->m_pfTurb[2];
					Par1-=floorf(Par1);
					Par1*=TWOPI;

					float Par2 = t_TCModObject->m_pfTurb[1];
					for (int s=0; s<pVB->GetSize();s++)
					{					
						m_pTempTexCoordsArray[s].v[0]=pOriginalTexCoords[s].v[0]+sinf(Par1+pOriginalTexCoords[s].v[0])*Par2;
						m_pTempTexCoordsArray[s].v[1]=pOriginalTexCoords[s].v[1]+sinf(Par1+pOriginalTexCoords[s].v[1])*Par2;
					}*/

					sprintf(temp,"float Par1_%d=%.2f*time+%.2f;\n",iParamNum,t_TCModObject->m_pfTurb[3],t_TCModObject->m_pfTurb[2]);
					shaderString_Inject_tc+= temp;

					sprintf(temp,"Par1_%d-=floor(Par1_%d);\n",iParamNum,iParamNum);
					shaderString_Inject_tc+= temp;

					sprintf(temp,"Par1_%d*=6.2831853;\n",iParamNum);
					shaderString_Inject_tc+= temp;

					sprintf(temp,"float Par2_%d = %.2f;\n",iParamNum,t_TCModObject->m_pfTurb[1]);
					shaderString_Inject_tc+= temp;

					sprintf(temp,"f_in.texCoord.x += sin(Par1_%d + f_in.texCoord.x * Par2_%d);\n",iParamNum,iParamNum);
					shaderString_Inject_tc+= temp;

					sprintf(temp,"f_in.texCoord.y += sin(Par1_%d + f_in.texCoord.y * Par2_%d);\n",iParamNum,iParamNum);
					shaderString_Inject_tc+= temp;
					
					iParamNum++;					
				}

				break;
				//default:
			}
		}
	}
	//shaderString_Inject+= "f_in.texCoord.xy += float2(time,time);\n";


	shaderString_Sample += "color = CE_TEX_2D(RegularTexture,f_in.texCoord.xy).rgba;\n";


	// rgbgen + alphagen

	UINT t_uiRGBGen		= pQ3AShaderPass->GetRGBGen();
	UINT t_uiALPHAGen	= pQ3AShaderPass->GetALPHAGen();

	switch(t_uiRGBGen)
	{
	case RGBGEN_WAVE:
		// RESTORE RESTORE RESTORE
		/*t_fRGBColorR = pShaderPass->ComputeRGBGenWave(fTimeMS);
		// DEBUG DEBUG DEBUG
		//t_fRGBColorR = pShaderPass->ComputeRGBGenWave(fTimeSeconds);
		t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;*/

		//REMOVED FOR DEBUGGING
		shaderString_Inject_rgba+=pQ3AShaderPass->GetRGBGenWave()->Compute_String().c_str();

		sprintf(temp,"float3 rgb_factor_%d = float3(wave_val,wave_val,wave_val);\n",iParamNum);
		shaderString_Inject_rgba+= temp;

		sprintf(temp,"color.xyz *= rgb_factor_%d;\n",iParamNum);
		shaderString_Inject_rgba+= temp;

		iParamNum++;

		bDebugPrint = true;

		break;
	case RGBGEN_IDENTITY:
		/*t_fRGBColorR=1.f;
		t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;*/
		break;
	case RGBGEN_CONST:
		{
			/*CWave* t_pWave = pShaderPass->GetRGBGenWave();
			t_fRGBColorR = t_pWave->m_fBase;
			t_fRGBColorG = t_pWave->m_fAmplitude;
			t_fRGBColorB = t_pWave->m_fPhase;*/

		}
		break;		
	case RGBGEN_IDENTITYLIGHTING:  // I'm not using GAMMA so I'm ignoring this...
		/*t_fRGBColorR=1.f;
		t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;*/
		break;

	default:
		/*t_fRGBColorR=1.f;
		t_fRGBColorG = t_fRGBColorB = t_fRGBColorR;*/
		break;
	}

	switch(t_uiALPHAGen)
	{
	case ALPHAGEN_WAVE:
		//t_fALPHAColor = pShaderPass->ComputeALPHAGenWave(fTimeMS);

		shaderString_Inject_rgba+=pQ3AShaderPass->GetAlphaGenWave()->Compute_String().c_str();

		sprintf(temp,"float alpha_factor_%d = wave_val;\n",iParamNum);
		shaderString_Inject_rgba+= temp;

		sprintf(temp,"color.a *= alpha_factor_%d;\n",iParamNum);
		shaderString_Inject_rgba+= temp;

		iParamNum++;

		bDebugPrint = true;

		break;
	case ALPHAGEN_IDENTITY:
		//t_fALPHAColor=1.f;
		break;

	case ALPHAGEN_IDENTITYLIGHTING:  // I'm not using GAMMA so I'm ignoring this...
		//t_fALPHAColor=1.f;
		break;

	default:
		//t_fALPHAColor=1.f;
		break;
	}
	
	shaderString_End +="}\n";


	std::string shaderString_Final;

	//shaderString_Final = shaderString_Start+shaderString_Inject_tc + shaderString_Sample + shaderString_Inject_rgba + shaderString_End;

	shaderString_Final = shaderString_Start +shaderString_Inject_tc + shaderString_Sample + shaderString_Inject_rgba + shaderString_End;

	//if (bDebugPrint)
	{
		//OutputDebugStringA("----------------\n\n");
		//OutputDebugStringA(shaderString_Final.c_str());
	}

	if (strstr(pQ3AShader->m_cpShaderName,"proto_light_2k"))
	{
		int i=0;
		i++;
	}

	char shader_name[300];
	sprintf(shader_name,"auto_generated_%s_pass_%d",pQ3AShader->m_cpShaderName,uiPassNum);

	pPass->fragmentProgram.LoadFPFromText(shader_name,shaderString_Final.c_str());
	pPass->fragmentProgram.AddUniform("time",g_pAutoUniforms->Get("time"));
	//pPass->fragmentProgram.AddUniform()

	/*tUniform* pUniform = NULL;
	pUniform = new tUniform(CR_FLOAT3);

	const char* pcAmbientColorName = "ambientColor";

	char * pchName = new char[strlen(pcAmbientColorName)+1];
	sprintf(pchName,pcAmbientColorName);
	pUniform->m_pchName = pchName;

	pUniform->m_pfValue[0] = 1.f;
	pUniform->m_pfValue[1] = 1.f;
	pUniform->m_pfValue[2] = 1.f;

	//assert(_CrtCheckMemory());
	pPass->fragmentProgram.AddUniform(pcAmbientColorName,pUniform);	*/


	DWORD dwShaderFlags = pQ3AShader->m_dwShaderFlags;

	if (dwShaderFlags & FLAG_NOCULL)
	{
		pPass->m_RendererState.m_bCull = false;
		pPass->m_RendererState.m_dwCullFunc = CR_NONE;		
	}
	else if (dwShaderFlags & FLAG_CULLFRONT)
	{
		pPass->m_RendererState.m_bCull = true;
		pPass->m_RendererState.m_dwCullFunc = CR_BACK;			
	}
	else
	{
		pPass->m_RendererState.m_bCull = true;
		pPass->m_RendererState.m_dwCullFunc = CR_FRONT;		
	}
	

	if (dwShaderFlags & FLAG_DEPTHWRITE)
		pPass->m_RendererState.m_bDepthWrite = true;
	else
		pPass->m_RendererState.m_bDepthWrite = false;


	pPass->m_RendererState.m_bDepthTest = true;

	//////////////////////
	// ALPHAFunc handling
	//////////////////////

	DWORD t_dwALPHAFunc = pQ3AShaderPass->GetALPHAFunc();

	switch(t_dwALPHAFunc) 
	{
	case ALPHAFUNC_GT0:
		//glEnable(GL_ALPHA_TEST);
		//glAlphaFunc(GL_GREATER,0);
		pPass->m_RendererState.m_dwAlphaRefValue = 0;
		pPass->m_RendererState.m_dwAlphaFunc = CR_GREATER;		
		break;

	case ALPHAFUNC_LT128:		
		//glEnable(GL_ALPHA_TEST);
		//glAlphaFunc(GL_LESS,0.5);
		pPass->m_RendererState.m_dwAlphaRefValue = 128;
		pPass->m_RendererState.m_dwAlphaFunc = CR_LESS;		
		break;

	case ALPHAFUNC_GE128:
		//glEnable(GL_ALPHA_TEST);
		//glAlphaFunc(GL_GEQUAL,0.5);
		pPass->m_RendererState.m_dwAlphaRefValue = 128;
		pPass->m_RendererState.m_dwAlphaFunc = CR_GREATER;
		break;
	default:
		pPass->m_RendererState.m_dwAlphaFunc = CR_ALWAYS;
	};

	DWORD dwSrc		= pQ3AShaderPass->GetSrcBlend();
	DWORD dwDest	= pQ3AShaderPass->GetDstBlend();

	switch (dwSrc)
	{
	case GL_ONE:
		pPass->m_RendererState.m_cBlendSource = CR_BF_ONE;
		break;
	case GL_ZERO:
		pPass->m_RendererState.m_cBlendSource = CR_BF_ZERO;
		break;
	case GL_DST_COLOR:
		pPass->m_RendererState.m_cBlendSource = CR_BF_DST_COLOR;
		break;
	case GL_SRC_COLOR:
		pPass->m_RendererState.m_cBlendSource = CR_BF_SRC_COLOR;
		break;
	case GL_ONE_MINUS_DST_COLOR:
		pPass->m_RendererState.m_cBlendSource = CR_BF_ONE_MINUS_DST_COLOR;
		break;
	case GL_ONE_MINUS_SRC_COLOR:
		pPass->m_RendererState.m_cBlendSource = CR_BF_ONE_MINUS_SRC_COLOR;
		break;
	case GL_SRC_ALPHA:
		pPass->m_RendererState.m_cBlendSource = CR_BF_SRC_ALPHA;
		break;
	case GL_ONE_MINUS_SRC_ALPHA:
		pPass->m_RendererState.m_cBlendSource = CR_BF_ONE_MINUS_SRC_ALPHA;
		break;
	}

	switch (dwDest)
	{
	case GL_ONE:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE;
		break;
	case GL_ZERO:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_ZERO;
		break;
	case GL_DST_COLOR:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_DST_COLOR;
		break;
	case GL_SRC_COLOR:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_SRC_COLOR;
		break;
	case GL_ONE_MINUS_DST_COLOR:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE_MINUS_DST_COLOR;
		break;
	case GL_ONE_MINUS_SRC_COLOR:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE_MINUS_SRC_COLOR;
		break;
	case GL_SRC_ALPHA:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_SRC_ALPHA;
		break;
	case GL_ONE_MINUS_SRC_ALPHA:
		pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE_MINUS_SRC_ALPHA;
		break;
	}

	DWORD dwPassSrc = pQ3AShaderPass->GetSrcBlend();
	DWORD dwPassDst = pQ3AShaderPass->GetDstBlend();

	if ( (uiPassNum == 0 ) )
	{
		if( ! ((dwPassSrc==GL_ONE) && (dwPassDst==GL_ZERO)))			
		{
			m_uiSort =  MATERIAL_SORT_BLEND;
		}
	}

	char message[400];
	sprintf_s(message,300,"Pass %d src=%d dst=%d map[0]=%s\n",
		uiPassNum,
		dwSrc,
		dwDest,
		pQ3AShaderPass->GetTextureName(0));

	g_pLogFile->OutPutPlainText(message,"4","000000",true,LOG_MESSAGE_INFORMATIVE);


	//== GL_ONE && pPass->GetDstBlend() == GL_ONE)

	return true;
}

bool CMaterial::GenerateFromQ3AShader(const CQ3A_Shader_Data* pQ3AShader)
{
	if (pQ3AShader->m_uiPassesNum < 1 )
		return false;

	//bool bRes = GenerateSimpleTextureMaterial(pQ3AShader->m_ppPasses[0]->GetTextureName(0));

	char temp[256];
	sprintf(temp,"Convert [%s] to cg\n",pQ3AShader->m_cpShaderName);
	//OutputDebugString(temp);

	if (strstr(pQ3AShader->m_cpShaderName,"proto_light_2k"))
	{
		int i=0;
		i++;
	}

	CTechnique* pTech = AddTechnique();
	

	for (int i=0;i<pQ3AShader->m_uiPassesNum;i++)
	{
		Generate_AddQ3AShaderPass(pQ3AShader,i);
	}
	

	return true;
}

bool CMaterial::GenerateSimpleTextureMaterial(const char* pchTexName)
{
	char tempName[500];


	PARAMETERS_BINDING_MAP bindingMap;

	std::string dkey("%diffuseMap");
	std::string dvalue(pchTexName);
	bindingMap[dkey] = dvalue;



	bool bResult = LoadMAT("Materials/SimpleTexture_ZERO_ALPHA.mat",bindingMap);
	

	char message[400];
	sprintf_s(message,300,"////CMaterial::Auto generated SIMPLE_TEXTURE:%s material Finished.",pchTexName);
	g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

	static int id=0;
	static char name[200];
	sprintf(name,"genSimpleTex%d",id);

	m_pcName = new char[strlen(name)+1];
	strcpy(m_pcName,name);


	id++;

	return bResult;
}

bool CMaterial::GenerateAlphaBlendedAlwaysDrawTextureMaterial(const char* pchTexName)
{
	char tempName[500];

	
	//


	PARAMETERS_BINDING_MAP bindingMap;

	std::string dkey("%diffuseMap");
	std::string dvalue(pchTexName);
	bindingMap[dkey] = dvalue;



	bool bResult = LoadMAT("Materials/GUI/SimpleAlphaBlendedTextureAlwaysDraw.mat",bindingMap);
	

	char message[400];
	sprintf_s(message,300,"////CMaterial::Auto generated SIMPLE_ALPHA_BLENDED_TEXTURE:%s material Finished.",pchTexName);
	g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

	static int id=0;
	static char name[200];
	sprintf(name,"genSimpleTex%d",id);

	m_pcName = new char[strlen(name)+1];
	strcpy(m_pcName,name);


	id++;

	return bResult;

}

bool CMaterial::GeneratePerPixelLitMaterial(const char* pchTexName)
{

	char tempName[500];

	
	//


	PARAMETERS_BINDING_MAP bindingMap;

	std::string dkey("%diffuseMap");
	std::string dvalue(pchTexName);
	bindingMap[dkey] = dvalue;

	sprintf(tempName,"%s",pchTexName);
	int len1 = strlen(pchTexName);
	int len2 = strlen(tempName);

	tempName[len2] = '_';
	tempName[len2+1] = 'l';
	tempName[len2+2] = 'o';
	tempName[len2+3] = 'c';
	tempName[len2+4] = 'a';
	tempName[len2+5] = 'l';
	tempName[len2+6] = NULL;
	

	std::string nkey("%normalMap");
	std::string nvalue(tempName);
	bindingMap[nkey] = nvalue;

	std::string skey("%specularMap");
	std::string svalue(pchTexName);
	bindingMap[skey] = svalue;


	
	


	bool bResult = LoadMAT("Materials/PPL/regularPPL.mat",bindingMap);
	//bool bResult = LoadMAT("Materials/SimpleTexture_alphaDOF.mat",bindingMap);
	

	/*BREAKPOINT(strcmp(pchTexName,"noshader")==0);
	BREAKPOINT(strstr(pchTexName,"shader")!=0);*/

	char message[400];
	sprintf_s(message,300,"////CMaterial::Auto generated SHADOW MAPPED - LIGHT PASSES BASED:%s material Finished.",pchTexName);
	g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

	static int id=0;
	static char name[200];
	sprintf(name,"genShadowMappedLitTex%d",id);


	m_pcName = new char[strlen(name)+1];
	strcpy(m_pcName,name);


	id++;

	return bResult;
	

}

/*
bool CMaterial::GenerateSimpleTextureMaterial(const char* pchTexName)
{

	PARAMETERS_BINDING_MAP bindingMap;

	std::string key("%diffuseMap");

	std::string value(pchTexName);

	bindingMap[key] = value;


	bool bResult = LoadMAT("Materials/SimpleTexture.mat",bindingMap);
	//bool bResult = LoadMAT("Materials/SimpleTexture_alphaDOF.mat",bindingMap);
	

	char message[400];
	sprintf_s(message,300,"////CMaterial::Auto generated SIMPLE_TEXTURE:%s material Finished.",pchTexName);
	g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

	static int id=0;
	static char name[200];
	sprintf(name,"genSimpleTex%d",id);


	m_pcName = new char[strlen(name)+1];
	strcpy(m_pcName,name);


	id++;

	return bResult;
	

}
*/

/*bool CMaterial::Load(const CQ3A_Shader* pQ3A_Shader)
{

}*/

bool CMaterial::Load(const char* pchFileName)
{
	char message[300];
	PARAMETERS_BINDING_MAP bindingMap;

	m_pcName = new char[strlen(pchFileName)+1];
	strcpy(m_pcName,pchFileName);

	if(strstr(pchFileName, ".mat"))
	{
	
		sprintf_s(message,300,"CMaterial::Load loading %s mat file...",pchFileName);
		g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

		bool bResult = LoadMAT(pchFileName,bindingMap);

		sprintf_s(message,300,"////CMaterial::Load loading of %s mat file Finished.",pchFileName);
		g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

		return bResult;
	}
	else
	if(strstr(pchFileName, ".utm"))
	{
		sprintf_s(message,300,"CMaterial::Load loading %s utm file...",pchFileName);
		g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

		char materialName[400];

		FILE* pFile;
		if ((pFile=fopen (pchFileName, "rb"))== NULL)
		{
			char temp[1000];
			sprintf(temp,"CMaterial::Load opening of UTM file [%s] failed!",pchFileName);

			DWORD dwError = GetLastError();
			sprintf(temp,"last error:%d",dwError);

			

			g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
			return false;
		}

		///////////////////////////////////////////////////
		// First of all read entire file into a string
		int iFileLength = filelength (fileno (pFile));
		char* pText = new char[iFileLength+1];
		fread(pText,sizeof(char)*(iFileLength+1),1,pFile);
		pText[iFileLength] = NULL;

		char currentWord[500];

		// create a parser containing it
		CParser par(pText,iFileLength+1);

		while (!par.ReachedEnd())
		{
			par.NextWord_IC(); 
			par.FillCurrentWord(currentWord);

			if (strcmp(currentWord,"using")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentQuetuePhrase(currentWord);


				strcpy(materialName,currentWord);
				continue;
			}

			if (currentWord[0]=='%') // a parameter binding
			{
				std::string key(currentWord);

				par.NextWord_IC(); 
				par.FillCurrentQuetuePhrase(currentWord);

				std::string value(currentWord);

				bindingMap[key] = value;

			}

		}
		

		fclose(pFile);



		bool bResult = LoadMAT(materialName,bindingMap);

		sprintf_s(message,300,"////CMaterial::Load loading of %s utm file Finished.",pchFileName);
		g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_INFORMATIVE);

		return bResult;
	}
	else
	{
		sprintf_s(message,300,"CMaterial::Unrecognized material file extension in file: ""%s""!",pchFileName);
		g_pLogFile->OutPutPlainText(message,"4","006699",true,LOG_MESSAGE_WARNING_LVL_3);
		return false;
	}




}

bool CMaterial::LoadMAT(const char* pchFileName)
{
	PARAMETERS_BINDING_MAP bindingMap;
	return (LoadMAT(pchFileName,bindingMap));
}


void  CMaterial::FillCurrentWord(char* pFillMe,CParser& par,PARAMETERS_BINDING_MAP& parBinding)
{
	if (pFillMe[0]=='%')
	{
		PARAMETERS_BINDING_MAP_IT it = parBinding.find(std::string(pFillMe));

		if (!(it==parBinding.end()))
			strcpy(pFillMe,it->second.c_str());
		else
		{
			char temp[300];
			sprintf_s(temp,300,"CMaterial::FillCurrentQueute didn't find ""%s""!",pFillMe);
			g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
		}

	}
	else
	{
		par.FillCurrentWord(pFillMe);
	}

}

void CMaterial::FillCurrentQueute(char* pFillMe,CParser& par,PARAMETERS_BINDING_MAP& parBinding)
{
	if (pFillMe[0]=='%')
	{
		PARAMETERS_BINDING_MAP_IT it = parBinding.find(std::string(pFillMe));

		if (!(it==parBinding.end()))
			strcpy(pFillMe,it->second.c_str());
		else
		{
			char temp[300];
			sprintf_s(temp,300,"CMaterial::FillCurrentQueute didn't find ""%s""! - are you trying to load abstract material file?? ",pFillMe);
			g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
		}

	}
	else
	{
		par.FillCurrentQuetuePhrase(pFillMe);
	}
}

bool CMaterial::LoadMAT(const char* pchFileName,PARAMETERS_BINDING_MAP& parBinding)
{
	FILE* pFile;
	if ((pFile=fopen (pchFileName, "rb"))== NULL)
	{
		char temp[1000];
		sprintf(temp,"CMaterial::Load opening of Material file [%s] failed!",pchFileName);
		g_pLogFile->OutPutPlainText(temp,LOG_MESSAGE_ERROR);
		return false;
	}

	///////////////////////////////////////////////////
	// First of all read entire file into a string
	int iFileLength = filelength (fileno (pFile));
	char* pText = new char[iFileLength+1];
	fread(pText,sizeof(char)*(iFileLength+1),1,pFile);
	pText[iFileLength] = NULL;

	char currentWord[500];

	char errorMessage[600];

	int iBracketsState=0;

	int iCurrentTexUnit;

	CSmartPointer<ITexture> spTex;
	CPass*    pPass;
	CTechnique* pTech;
	

	// create a parser containing it
	CParser par(pText,iFileLength+1);


	par.NextWord_IC(); // move to '{'
		
	iBracketsState++;

	par.NextWord_IC();

while (iBracketsState==1) // read techniques and material flags
{
	par.FillCurrentWord(currentWord);

	if (strcmp(currentWord,"}")==0)
	{
		par.NextWord_IC();
		iBracketsState--;
		break;

	}
	else
	if (strcmp(currentWord,"sort")==0)
	{
		/*par.NextLine();
		par.EatWhiteSpace();
		continue;*/


		par.NextWord_IC(); 
		par.FillCurrentWord(currentWord);
		FillCurrentWord(currentWord,par,parBinding);

		if (strcmp(currentWord,"opaque")==0)
		{
				m_uiSort= MATERIAL_SORT_OPAQUE;
		}else if (strcmp(currentWord,"blend")==0)
		{
				m_uiSort = MATERIAL_SORT_BLEND;
		} else if (strcmp(currentWord,"opaque_trigger_postprocess")==0) // no need to use, will be found automagically
		{
			m_uiSort = MATERIAL_SORT_OPAQUE_TRIGGER_POSTPROCESS;

		} else if (strcmp(currentWord,"blend_trigger_postprocess")==0) // no need to use, will be found automagically
		{
			m_uiSort = MATERIAL_SORT_BLEND_TRIGGER_POSTPROCESS;

		} else if (strcmp(currentWord,"postprocess")==0) // no need to use, will be found automagically
		{
			m_uiSort = MATERIAL_SORT_POSTPROCESS;

		} else
		{
			sprintf_s(errorMessage,600,"%s: Unrecognized sort value: %s",pchFileName,currentWord);
			g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_3);

		}

		par.NextLine();
		par.EatWhiteSpace();
		continue;
	} else
if(strcmp(currentWord,"technique")==0) 
{
	pTech = AddTechnique();
		
	par.NextWord_IC(); // skip '{'
	par.NextWord_IC();
	iBracketsState++;

	while (iBracketsState==2) // read technique
	{
		par.FillCurrentWord(currentWord);

		if (strcmp(currentWord,"}")==0)
		{
			par.NextWord_IC(); 
			iBracketsState--;

			//AddTechnique(pTech);

			break;
		}
		
		while (strcmp(currentWord,"pass")!=0) 
		{
			par.NextWord_IC(); 
			par.FillCurrentWord(currentWord);
		}
	
		par.NextWord_IC(); // skip '{'
		iBracketsState++;

		par.NextWord_IC();

		iCurrentTexUnit = 0;



		pPass = pTech->AddPass();
		while (iBracketsState==3) // read pass
		{						
			par.FillCurrentWord(currentWord);

			if (strcmp(currentWord,"}")==0) // current pass ended
			{
				iBracketsState--;
				par.NextWord_IC(); 
				//pTech->AddPass(pPass);
				break;
			}
			else if (strcmp(currentWord,"lightpass")==0)
			{				
				pPass->m_dwLighting = MAT_PASS_LIGHTING_LIGHT;							

				par.NextWord_IC(); 
				continue;
			} else	if (strcmp(currentWord,"cullfunc")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				FillCurrentWord(currentWord,par,parBinding);

				if (strcmp(currentWord,"FRONT")==0)
					pPass->m_RendererState.m_dwCullFunc = CR_FRONT;
				else if (strcmp(currentWord,"BACK")==0)
					pPass->m_RendererState.m_dwCullFunc = CR_BACK;
				else if (strcmp(currentWord,"NONE")==0)
					pPass->m_RendererState.m_dwCullFunc = CR_NONE;
				else
				{
					sprintf_s(errorMessage,600,"%s: Unrecognized cullfunc value: %s",pchFileName,currentWord);
					g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
				}


				par.NextWord_IC(); 
				continue;
			} else
			if (strcmp(currentWord,"blendfunc")==0)
				{

					// read source value

					par.NextWord_IC();
					par.FillCurrentWord(currentWord);
					FillCurrentWord(currentWord,par,parBinding);

					if (strcmp(currentWord,"zero")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_ZERO;
					else if (strcmp(currentWord,"one")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_ONE;
					else if (strcmp(currentWord,"dst_color")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_DST_COLOR;
					else if (strcmp(currentWord,"one_minus_dst_color")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_ONE_MINUS_DST_COLOR;
					else if (strcmp(currentWord,"src_alpha")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_SRC_ALPHA;
					else if (strcmp(currentWord,"one_minus_src_color")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_ONE_MINUS_SRC_COLOR;
					else if (strcmp(currentWord,"dst_alpha")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_DST_ALPHA;
					else if (strcmp(currentWord,"one_minus_dst_alpha")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_ONE_MINUS_DST_ALPHA;
					else if (strcmp(currentWord,"src_alpha_saturate")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_SRC_ALPHA_SATURATE;
					else if (strcmp(currentWord,"one_minus_src_alpha")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_ONE_MINUS_SRC_ALPHA;
					else if (strcmp(currentWord,"src_color")==0)
						pPass->m_RendererState.m_cBlendSource = CR_BF_SRC_COLOR;
					else
					{
						sprintf_s(errorMessage,600,"%s: Unrecognized blendfunc SOURCE value: ""%s"", defaulting to one ",pchFileName,currentWord);
						g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
						pPass->m_RendererState.m_cBlendSource = CR_BF_ONE;
					}

					// read target value

					par.NextWord_IC();
					par.FillCurrentWord(currentWord);
					FillCurrentWord(currentWord,par,parBinding);

					if (strcmp(currentWord,"zero")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_ZERO;
					else if (strcmp(currentWord,"one")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE;
					else if (strcmp(currentWord,"dst_color")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_DST_COLOR;
					else if (strcmp(currentWord,"one_minus_dst_color")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE_MINUS_DST_COLOR;
					else if (strcmp(currentWord,"src_alpha")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_SRC_ALPHA;
					else if (strcmp(currentWord,"one_minus_src_color")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE_MINUS_SRC_COLOR;
					else if (strcmp(currentWord,"dst_alpha")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_DST_ALPHA;
					else if (strcmp(currentWord,"one_minus_dst_alpha")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE_MINUS_DST_ALPHA;
					else if (strcmp(currentWord,"src_alpha_saturate")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_SRC_ALPHA_SATURATE;
					else if (strcmp(currentWord,"one_minus_src_alpha")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_ONE_MINUS_SRC_ALPHA;
					else if (strcmp(currentWord,"src_color")==0)
						pPass->m_RendererState.m_cBlendTarget = CR_BF_SRC_COLOR;
					else
					{
						sprintf_s(errorMessage,600,"%s: Unrecognized blendfunc TARGET value: ""%s"", defaulting to zero ",pchFileName,currentWord);
						g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
						pPass->m_RendererState.m_cBlendTarget = CR_BF_ZERO;
					}

					
					par.NextWord_IC(); 
					continue;

				}
			else
			if (strcmp(currentWord,"depthfunc")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				FillCurrentWord(currentWord,par,parBinding);

				if (strcmp(currentWord,"LESS_OR_EQUAL")==0)
					pPass->m_RendererState.m_dwDepthFunc = CR_LESS_OR_EQUAL;
				else if (strcmp(currentWord,"LESS")==0)
					pPass->m_RendererState.m_dwDepthFunc = CR_LESS;
				else if (strcmp(currentWord,"ALWAYS")==0)
					pPass->m_RendererState.m_dwDepthFunc = CR_ALWAYS;
				else if (strcmp(currentWord,"EQUAL")==0)
					pPass->m_RendererState.m_dwDepthFunc = CR_EQUAL;
				else
				{
					sprintf_s(errorMessage,600,"%s: Unrecognized depthfunc value: %s",pchFileName,currentWord);
					g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
				}


				par.NextWord_IC(); 
				continue;
			} else
			if (strcmp(currentWord,"depthtest")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				FillCurrentWord(currentWord,par,parBinding);

				if (strcmp(currentWord,"1")==0)
					pPass->m_RendererState.m_bDepthTest = true;
				else if (strcmp(currentWord,"0")==0)
					pPass->m_RendererState.m_bDepthTest = false;
				else
				{
					sprintf_s(errorMessage,600,"%s: Unrecognized depthtest value: %s",pchFileName,currentWord);
					g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
				}

				par.NextWord_IC(); 
				continue;
			} else
			if (strcmp(currentWord,"depthwrite")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				FillCurrentWord(currentWord,par,parBinding);

				if (strcmp(currentWord,"1")==0)
					pPass->m_RendererState.m_bDepthWrite = true;
				else if (strcmp(currentWord,"0")==0)
					pPass->m_RendererState.m_bDepthWrite = false;
				else
				{
					sprintf_s(errorMessage,600,"%s: Unrecognized depthwrite value: %s",pchFileName,currentWord);
					g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
				}

				par.NextWord_IC(); 
				continue;
			} else
			if (strcmp(currentWord,"vertexshader")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				FillCurrentQueute(currentWord,par,parBinding);
				pPass->vertexProgram.LoadVP(currentWord);					

				par.NextWord_IC(); // skip '{'
				iBracketsState++;

				while (iBracketsState==4)
				{
					par.NextWord_IC();
					par.FillCurrentWord(currentWord);

					if (strcmp(currentWord,"}")==0)
					{
						iBracketsState--;
						par.NextWord_IC();
						break;
					}

					if (strcmp(currentWord,"autouniform")==0)
					{
						par.NextWord_IC();
						par.FillCurrentWord(currentWord);

						FillCurrentWord(currentWord,par,parBinding);

						pPass->vertexProgram.AddUniform(currentWord,g_pAutoUniforms->Get(currentWord));
						continue;
					}

					if ( (strcmp(currentWord,"uniform1")==0) || 
						(strcmp(currentWord,"uniform2")==0) ||
						(strcmp(currentWord,"uniform3")==0) ||
						(strcmp(currentWord,"uniform4")==0) ||
						(strcmp(currentWord,"uniform4x4")==0)						
						)
					{

						int iUniformSize=1;
						if (strcmp(currentWord,"uniform1")==0)
							iUniformSize=1;
						else if (strcmp(currentWord,"uniform2")==0)
						iUniformSize=2;
						else if (strcmp(currentWord,"uniform3")==0)
						iUniformSize=3;
						else if (strcmp(currentWord,"uniform4")==0)
						iUniformSize=4;
						else if (strcmp(currentWord,"uniform4x4")==0)
						iUniformSize=16;


						par.NextWord_IC();
						par.FillCurrentWord(currentWord);
						FillCurrentWord(currentWord,par,parBinding);

						/****************************************/
						//////////////////////////
						// Uniform Brightness
						tUniform* pUniform;
						if (iUniformSize==1)
							pUniform = new tUniform(CR_FLOAT1);
						else if (iUniformSize==2)
							pUniform = new tUniform(CR_FLOAT2);
						else if (iUniformSize==3)
							pUniform = new tUniform(CR_FLOAT3);
						else if (iUniformSize==4)
							pUniform = new tUniform(CR_FLOAT4);
						else if (iUniformSize==16)
							pUniform = new tUniform(CR_FLOAT4X4);

						char * pchName = new char[strlen(currentWord)+1];
						sprintf(pchName,currentWord);
						pUniform->m_pchName = pchName;

						for (int k=0;k<iUniformSize;k++)
						{
							par.NextWord_IC();
							par.FillCurrentWord(currentWord);
							FillCurrentWord(currentWord,par,parBinding);
							pUniform->m_pfValue[k] = atof(currentWord);	
						}
		
						//assert(_CrtCheckMemory());
						pPass->vertexProgram.AddUniform(currentWord,pUniform);			
						/////////////////////////////
						continue;
					}


				}

				continue;
			}
			else
			if (strcmp(currentWord,"fragmentshader")==0)
			{
				par.NextWord_IC(); 
				par.FillCurrentWord(currentWord);
				FillCurrentQueute(currentWord,par,parBinding);
				pPass->fragmentProgram.LoadFP(currentWord);					

				par.NextWord_IC(); // skip '{'
				iBracketsState++;

				while (iBracketsState==4)
				{
					par.NextWord_IC();
					par.FillCurrentWord(currentWord);

					if (strcmp(currentWord,"}")==0)
					{
						iBracketsState--;
						par.NextWord_IC();
						break;
					}

					if (strcmp(currentWord,"autouniform")==0)
					{
						par.NextWord_IC();
						par.FillCurrentWord(currentWord);
						pPass->fragmentProgram.AddUniform(currentWord,g_pAutoUniforms->Get(currentWord));
						continue;
					}


										if ( (strcmp(currentWord,"uniform1")==0) || 
						(strcmp(currentWord,"uniform2")==0) ||
						(strcmp(currentWord,"uniform3")==0) ||
						(strcmp(currentWord,"uniform4")==0) ||
						(strcmp(currentWord,"uniform4x4")==0)						
						)
					{

						int iUniformSize;
						if (strcmp(currentWord,"uniform1")==0)
							iUniformSize=1;
						else if (strcmp(currentWord,"uniform2")==0)
						iUniformSize=2;
						else if (strcmp(currentWord,"uniform3")==0)
						iUniformSize=3;
						else if (strcmp(currentWord,"uniform4")==0)
						iUniformSize=4;
						else if (strcmp(currentWord,"uniform4x4")==0)
						iUniformSize=16;


						par.NextWord_IC();
						par.FillCurrentWord(currentWord);
						FillCurrentWord(currentWord,par,parBinding);

						/****************************************/
						//////////////////////////
						// Uniform Brightness
						tUniform* pUniform;
						if (iUniformSize==1)
							pUniform = new tUniform(CR_FLOAT1);
						else if (iUniformSize==2)
							pUniform = new tUniform(CR_FLOAT2);
						else if (iUniformSize==3)
							pUniform = new tUniform(CR_FLOAT3);
						else if (iUniformSize==4)
							pUniform = new tUniform(CR_FLOAT4);
						else if (iUniformSize==16)
							pUniform = new tUniform(CR_FLOAT4X4);

						char * pchName = new char[strlen(currentWord)+1];
						sprintf(pchName,currentWord);
						pUniform->m_pchName = pchName;

						for (int k=0;k<iUniformSize;k++)
						{
							par.NextWord_IC();
							par.FillCurrentWord(currentWord);
							FillCurrentWord(currentWord,par,parBinding);
							pUniform->m_pfValue[k] = atof(currentWord);	
						}
		
						//assert(_CrtCheckMemory());
						pPass->fragmentProgram.AddUniform(currentWord,pUniform);			
						/////////////////////////////
						continue;
					}
				}

				continue;
			}


			if (strcmp(currentWord,"texunit")==0)
			{
				par.NextWord_IC(); // read '{'
				iBracketsState++;

				while (strcmp(currentWord,"}")!=0)
				{

				par.NextWord_IC();
				par.FillCurrentWord(currentWord);
				if (strcmp(currentWord,"map")==0)
				{
					par.NextWord_IC();
					//par.FillCurrentWord(currentWord); // might be a problem here...
					// since it might be a quoted actual path !!
					par.FillCurrentQuetuePhrase(currentWord);
					FillCurrentQueute(currentWord,par,parBinding);

					//pTex = g_pRendererBack->LoadTexture(currentWord);
					//spTex = st_TextureManager->Load(currentWord);
					if (currentWord[0] == '$') // render to texture!
					{
						if (strcmp(currentWord,"$shadowMap")==0)
						{
							CRenderToTextureProxy* pRTTProxy = new CRenderToTextureProxy();
							pPass->AddTexture( pRTTProxy,iCurrentTexUnit);
							pPass->SetShadowMapRTTProxy(pRTTProxy);
						}
						else
						if (strcmp(currentWord,"$PreProcess")==0)
						{
							m_uiSort = MATERIAL_SORT_POSTPROCESS;
							/*pPass->AddTexture(g_pRendererBack->GetPreProcessingRenderTarget(0),
								iCurrentTexUnit);	*/
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(PRE_PROCESSING_FINAL),
								iCurrentTexUnit);								
						}
						else
						if (strcmp(currentWord,"$Camera0")==0)
						{
							m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_0),
								iCurrentTexUnit);								
						}
						else
						if (strcmp(currentWord,"$Camera1")==0)
						{
							m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_1),
								iCurrentTexUnit);	
						}
						else
						if (strcmp(currentWord,"$Camera2")==0)
						{
							m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_2),
								iCurrentTexUnit);	
						}
						else
						if (strcmp(currentWord,"$Camera3")==0)
						{
							m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_3),
								iCurrentTexUnit);	
						}
						else
						if (strcmp(currentWord,"$Camera4")==0)
						{
							m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_4),
								iCurrentTexUnit);	
						}
						else
						if (strcmp(currentWord,"$Camera5")==0)
						{
							m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_5),
								iCurrentTexUnit);	
						}
						else
						if (strcmp(currentWord,"$CameraDepth")==0)
						{
							//m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_DEPTH),
								iCurrentTexUnit);	
						}
						else
						if (strcmp(currentWord,"$ScaleDownCamera")==0)
						{
							//m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_SCALE_DOWN),
								iCurrentTexUnit);	
							// would cause creation of preProcess RTT, ignoring output...
							g_pRendererBack->GetRenderTarget(PRE_PROCESSING_FINAL);
						}
						else
						if (strcmp(currentWord,"$ScaleDownCamera2")==0)
						{
							//m_uiSort = MATERIAL_SORT_POSTPROCESS;
							pPass->AddTexture(g_pRendererBack->GetRenderTarget(RENDER_CAMERA_SCALE_DOWN_2),
								iCurrentTexUnit);	
							// would cause creation of preProcess RTT, ignoring output...
							g_pRendererBack->GetRenderTarget(PRE_PROCESSING_FINAL);
						}
						else
						{
							sprintf_s(errorMessage,600,"%s: Unrecognized Render Texture! -  %s",pchFileName,currentWord);
							g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
						}


					}
					else // regular texture
					{
						spTex = g_pRendererBack->LoadTexture(currentWord);
						pPass->AddTexture(spTex,iCurrentTexUnit);
					}

					continue;
				}
				else
				if (strcmp(currentWord,"clamp")==0)
				{
					par.NextWord_IC();
					//par.FillCurrentWord(currentWord); // might be a problem here...
					// since it might be a quoted actual path !!
					par.FillCurrentWord(currentWord);
					FillCurrentWord(currentWord,par,parBinding);

					if (strcmp(currentWord,"CLAMP")==0)
					{
						pPass->SamplerStates[iCurrentTexUnit].cClamping = SS_CLAMPING_CLAMP;
					} 
					else if (strcmp(currentWord,"REPEAT")==0)
					{
						pPass->SamplerStates[iCurrentTexUnit].cClamping = SS_CLAMPING_REPEAT;
					} 
					else
					{
						sprintf_s(errorMessage,600,"%s: Unrecognized clamp value: %s",pchFileName,currentWord);
						g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
					}

					continue;

				}				
				else
				if (strcmp(currentWord,"filtering")==0)
				{
					par.NextWord_IC();
					par.FillCurrentWord(currentWord);
					FillCurrentWord(currentWord,par,parBinding);

					if (strcmp(currentWord,"NEAREST")==0)
					{
						pPass->SamplerStates[iCurrentTexUnit].cFiltering = SS_FILTERING_NEAREST;
					} 
					else if (strcmp(currentWord,"NEAREST_NO_MIPMAPS")==0)
					{
						pPass->SamplerStates[iCurrentTexUnit].cFiltering = SS_FILTERING_NEAREST_NO_MIPMAPS;
					} 
					else if (strcmp(currentWord,"BILINEAR")==0)
					{
						pPass->SamplerStates[iCurrentTexUnit].cFiltering = SS_FILTERING_BILINEAR;
					} 
					else if (strcmp(currentWord,"TRILINEAR")==0)
					{
						pPass->SamplerStates[iCurrentTexUnit].cFiltering = SS_FILTERING_TRILINEAR;
					} 
					else if (strcmp(currentWord,"ANISOTROPIC")==0)
					{
						pPass->SamplerStates[iCurrentTexUnit].cFiltering = SS_FILTERING_ANISOTROPIC;

						par.NextWord_IC();
						par.FillCurrentWord(currentWord);
						FillCurrentWord(currentWord,par,parBinding);

						pPass->SamplerStates[iCurrentTexUnit].cAnisotropicLevel = atoi(currentWord);

						//TODO: add check if it's a power of 2
					} 
					else
					{
						sprintf_s(errorMessage,600,"%s: Unrecognized filtering value: %s",pchFileName,currentWord);
						g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
					}

					continue;

				}
				

				}



				//par.NextLine();
				par.NextWord_IC();

				iBracketsState--;
				iCurrentTexUnit++;

				continue;
			}

			// if we got here then there was incorrect command inside pass


			sprintf_s(errorMessage,600,"%s: Unrecognized pass command: %s",pchFileName,currentWord);
			g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);

			par.NextWord_IC();

		} // single pass read
		
	} // END of "while (iBracketsState==2) // read pass"


	} // END of "if(strcmp(currentWord,"technique")==0)"
	else
	{
		sprintf_s(errorMessage,600,"%s: Unrecognized material command: %s",pchFileName,currentWord);
		g_pLogFile->OutPutPlainText(errorMessage,LOG_MESSAGE_WARNING_LVL_2);
		par.NextWord_IC();

	}

}


	return CreateMaxVD();	

}

void CMaterial::Bind(void) const
{
/*	static CProfileStep* step = NULL;
	if (!step)
		step = new CProfileStep("VS:");

	step->Begin();

	step->End();*/

	// nothing for now

}

//bool CMaterial::AddTechnique(CTechnique* pTech)
CTechnique* CMaterial::AddTechnique(void)
{
	if (m_uiTechniquesNum >= CRIMSON_TECHNIQUES_MAX)
	{
		g_pLogFile->OutPutPlainText("CMaterial::AddTechnique::Trying to add more techniques then CRIMSON_TECHNIQUES_MAX to a material.",LOG_MESSAGE_ERROR);
		return false;
	}

	CTechnique* pTech = new CTechnique();

	m_ppTechniques[m_uiTechniquesNum++] = pTech;


	return pTech;
}

const CTechnique* CMaterial::GetTechnique(unsigned int iIndex) const
{
	if (iIndex >= m_uiTechniquesNum)
	{
		g_pLogFile->OutPutPlainText("CMaterial::GetTechnique::Trying to access an index>=m_uiTechniquesNum.",LOG_MESSAGE_ERROR);
		return NULL;
	}

	return m_ppTechniques[iIndex];

}

void CMaterial::InjectRTT(IRenderToTexture* pRTT)
{
	CTechnique* pTech = (CTechnique*) GetTechnique(0);
	CPass* pTempPass = (CPass*) pTech->GetPass(0);

	CRenderToTextureProxy* pRTTProxy = pTempPass->GetShadowMapRTTProxy();
	BREAKPOINT(!pRTTProxy);

	pRTTProxy->SetRenderToTexture(	pRTT);

}

