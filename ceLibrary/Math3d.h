#ifndef CRIMSON_MATH3D_H
#define CRIMSON_MATH3D_H

#pragma once

#include <assert.h>

//todo: Completely recode this...

#include "Defines.h"

#include "Windows.h"

#define EPSILON         0.0078125f
#define TINY_EPSILON    0.00000001f

#include "Math3d.h"

#include "math.h"
#include "ce.h"


/////////////////////////////////////////////////////////
//GENERAL TODO:
// /*add consts when it's possible (both parameters and function body)
// add memcpy when possible */
////////////////////////////////////////////////////////////


/////////////////////////////////////////////
// class CColor4
// *****************
// contains a 4 char array
//////////////////////////////////////////////

#include <windows.h>

class CColor4
{
	public:
	char c[4]; // 4 components: Red Green Blue Alpha

	////////////////
	// Constructer
	///////////////
	CColor4();

	////////////////////////////////
	// Constructer, accepts 4 bytes
	////////////////////////////////
	CColor4(const char r,const char g,const char b,const char a);

	/////////////////////////
	// Set()
	// simply set the values
	/////////////////////////
	void Set(const char r,const char g,const char b,const char a);

	//////////////////////////////////////////////////
	// Set() -- faster
	// accepts a reference to a 4 byte array
	///////////////////////////////////////////////////
	void Set(const CColor4& color);
};

///////////////////////////////////////////////////
// *********class CVec3f***********/////////////
///////////////////////////////////////////////////

class CVec3f
{
	public:

	union
	{
		float v[3]; // 3 components - x,y,z

		struct
		{
			float x;
			float y;
			float z;
		};

	};

	///////////////////////////////////////////////
	// *********** Constructers ******************
	///////////////////////////////////////////////


	////////////////////////////////////////////
	// Basic Constructer
	// notice: v[0,1,2] aren't inited, for speed
	////////////////////////////////////////////
    CVec3f();

	CVec3f(float* pfXYZ);


	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	CVec3f(float x, float y, float z);

	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	CVec3f(const CVec3f& vSet);

	// TODO:  CVec3f(float* fBlah)

	/////////////////////
	// Destructer()
	////////////////////
	~CVec3f() {};

	/////////////////////////////////////////////////
	// ***********methods to alter data*************
	/////////////////////////////////////////////////

	//TODO: operator = on other things (like float*, so I can memcpy it)
 

	//////////////////////
	// operator: "="
	///////////////////////
	inline CVec3f operator=(const CVec3f &pVec);


	//////////////////////
	// operator: "=="
	///////////////////////
	inline bool operator==(const CVec3f &pVec) const
	{
		/*if ( (v[0] == pVec.v[0]) && (v[1] == pVec.v[1]) && (v[2] == pVec.v[2]))
			return true;*/

		if ( (v[0] <= pVec.v[0]+TINY_EPSILON) && (v[0] >= pVec.v[0]-TINY_EPSILON) &&
			 (v[1] <= pVec.v[1]+TINY_EPSILON) && (v[1] >= pVec.v[1]-TINY_EPSILON) &&
			 (v[2] <= pVec.v[2]+TINY_EPSILON) && (v[2] >= pVec.v[2]-TINY_EPSILON))
			 return true;

		return false;
	}


	/////////////////////
	// Set() 
	/////////////////////
	void Set(float x, float y, float z);   

	///////////////////////
	// Set()
	///////////////////////
	void Set(CVec3f &Vec3f);

	//////////////////////////////////////////////////
	// Set() - FASTER
	// (pfSet must point to at least 3 valid floats)
	//////////////////////////////////////////////////
	void Set(float* pfSet);


	///////////////////////////////////////////////////////////////////////////////////////////
	///////*************Operators that return a result and don't change inner data*************
	/////// FIXME: make the functions themselves const +++++++ the parameters
	///////////////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////
    // *******SCALAR parameter*********
	////////////////////////////////////////

    ////////////////////
    // CVector + float
	////////////////////
    CVec3f operator+(float fNum) const ;

	////////////////////
	// CVector - float
	////////////////////  
    CVec3f operator-(float fNum) const ;

	////////////////////
	// CVector * float
	////////////////////
    CVec3f operator*(float fNum) const ;

	////////////////////
	// CVector / float
	////////////////////
    CVec3f operator/(float fNum) const ;


	//////////////////////////////////////////////
    // ***********Vector parameter***********
	//////////////////////////////////////////////


   
	/////////////////////////
	// Vector + Vector
	///////////////////////////
	__forceinline CVec3f operator+(const CVec3f& Vec1) const
	{
		return CVec3f(v[0]+Vec1.v[0],v[1]+Vec1.v[1],v[2]+Vec1.v[2]);
	}

	/////////////////////////
	// Vector - Vector
	///////////////////////////
	__forceinline CVec3f operator-(const CVec3f& Vec1) const
	{
		return CVec3f(v[0]-Vec1.v[0],v[1]-Vec1.v[1],v[2]-Vec1.v[2]);
	}


   
   /////////////////////////////
   // Vector / Vector
   /////////////////////////////
   CVec3f operator/(const CVec3f& Vec1) const;

   /////////////////////////////
	// Vector dot Vector
	/////////////////////////////
	__forceinline float operator*(const CVec3f& Vec1) const
	{
		return v[0]*Vec1.v[0]+v[1]*Vec1.v[1]+v[2]*Vec1.v[2];
	}
   /////////////////////////
	// Vector cross Vector
	///////////////////////////
	__forceinline CVec3f operator^(const CVec3f& Vec1) const
	{
		return CVec3f(v[1] * Vec1.v[2] - v[2] * Vec1.v[1],
						v[2] * Vec1.v[0] - v[0] * Vec1.v[2],
						v[0] * Vec1.v[1] - v[1] * Vec1.v[0]);
	}



   /////////////////////////////////////////////////////////////////
   ///////*************Operators that DO change inner data*************
   /////////////////////////////////////////////////////////////////

   
   ////////////////////////////
   //// a SCALAR parameter
   ////////////////////////////

   /////////////////////////
   // vector+=scalar
   /////////////////////////
   void operator+=(float fNum) ;
   
   /////////////////////////
   // vector-=scalar
   /////////////////////////
   void operator-=(float fNum) ;

   /////////////////////////
   // vector*=scalar
   /////////////////////////
   void operator*=(float fNum) ;

   /////////////////////////
   // vector/=scalar
   /////////////////////////
   void operator/=(float fNum) ;

   ///////////////////////////////////////////////////////////////////////////
   //// a VECTOR parameter
   //// TODO: maybe i should pass the parameter as a reference (also const)
   ///////////////////////////////////////////////////////////////////////////

	/////////////////////////
	// Vector+=Vector
	/////////////////////////
	__forceinline void operator+=(CVec3f Vec1) 
	{
	v[0]+= Vec1.v[0];
	v[1]+= Vec1.v[1];
		v[2]+= Vec1.v[2];
	}

	/////////////////////////
	// Vector-=Vector
	/////////////////////////
	__forceinline void operator-=(CVec3f Vec1) 
	{
	v[0]-= Vec1.v[0];
	v[1]-= Vec1.v[1];
	v[2]-= Vec1.v[2];
	}

/////////////////////////////////////
// Vector*=Vector
// (should work like a dot product)
////////////////////////////////////
__forceinline void operator*=(CVec3f Vec1) 
{
 v[0]*= Vec1.v[0];
 v[1]*= Vec1.v[1];
 v[2]*= Vec1.v[2];
}

/////////////////////////
// Vector/=Vector
/////////////////////////
__forceinline void operator/=(CVec3f Vec1) 
{
 v[0]/= Vec1.v[0];
 v[1]/= Vec1.v[1];
 v[2]/= Vec1.v[2];
}


   ///////////////////////////////////
   // Operating on the Class itself
   // returns a result, no parameters
   ///////////////////////////////////

   ///////////////////////////////////
   // Length()
   // get the length of the vector
   //////////////////////////////////
   float Length(void) const;


   //////////////////////////////////
   // Inverse()
   // *(-1) for each axis component
   //////////////////////////////////
   void Inverse(void);

  ////////////////////////////////////////////////////////////
// Normalize()
// normalize the vector (after normalization, length==1.f)
///////////////////////////////////////////////////////////

__forceinline CVec3f Normalize(void)
{
	float t;
	t = (float) sqrtf( v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
		
	v[0] /= t;
	v[1] /= t;
	v[2] /= t;

	return (*this);
}

   float GetSize(void);


/*   //// Only use Add,Subtract,Multiply,Divide on Time Critical stuff
   ///  and only after the Program is finished

   void Add(CVec3f& v1,CVec3f& v2);

   void Subtract(CVec3f& v1,CVec3f& v2);
   
   void Multiply(CVec3f& v1,CVec3f& v2);
   
   void Divide(CVec3f& v1,CVec3f& v2);
   
   void Cross(CVec3f& v1,CVec3f& v2);

   void Inverse(CVec3f& Vec1);

   void Normalize(CVec3f& Vec1);

   void PutRayPoint(CVec3f& RayOrigin,CVec3f& RayDirection,float t);*/
  
};


///////////////////////////////////////////////////
// *********class CVec3f***********/////////////
///////////////////////////////////////////////////

class CVector4f
{
public:

	union
	{
		float v[4]; // 4 components - n.x,n.y,n.z,d
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};

	///////////////////////////////////////////////
	// *********** Constructers ******************
	///////////////////////////////////////////////


	////////////////////////////////////////////
	// Basic Constructer
	// notice: v[0,1,2] aren't inited, for speed
	////////////////////////////////////////////
	CVector4f();


	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	CVector4f(float x, float y, float z,float d);

	//////////////////////////////////
	// Constructer with set values
	//////////////////////////////////
	//CVec3f(const CVector4f& vSet);

	// TODO:  CVec3f(float* fBlah)

	/////////////////////
	// Destructer()
	////////////////////
	~CVector4f() {};

	/////////////////////////////////////////////////
	// ***********methods to alter data*************
	/////////////////////////////////////////////////

	//TODO: operator = on other things (like float*, so I can memcpy it)


	//////////////////////
	// operator: "="
	///////////////////////
	inline CVector4f operator=(const CVector4f &pVec);


	inline void operator /= (float fScalar)
	{
		x/=fScalar;
		y/=fScalar;
		z/=fScalar;
		w/=fScalar;
	}

	/////////////////////
	// Set() 
	/////////////////////
	void Set(float x, float y, float z, float d);   

	/////////////////////
	// Normalize()
	//////////////////////

	CVector4f Normalize(void);

	///////////////////////
	// Set()
	///////////////////////
	//void Set(CVector4f &Vec4f);
};


///////////////////////////////////////////////////
// *********class CVec2f***********/////////////
///////////////////////////////////////////////////

class CVec2f
{
	public:

		union
		{
			float v[2]; // 2 components - x,y

			struct
			{
				float x;
				float y;
			};

		};

	///////////////////////////////////////////////
	// *********** Constructers ******************
	///////////////////////////////////////////////
	

	///////////////////////
	//Constructer()
	///////////////////////
	CVec2f();

	////////////////////////////////////
	//Constructer()
	// constructs and inits using x,y
	////////////////////////////////////
	CVec2f(float x, float y);


	/////////////////////////////////
	//Destructer()
	//////////////////////////////////
	~CVec2f() {};
     

	///////////////////////////////
	//Set() - accepts 2 floats
	////////////////////////////////
	void Set(float x, float y);   


	/////////////////////////////////////////
	//Set() // accept a reference to a vector
	/////////////////////////////////////////
	void Set(CVec2f &Vec2f);

	//////////////////////////////////////////////////////////
	//Set() // accepts a float array (of at least 2 elements)
	///////////////////////////////////////////////////////////
	void Set(float* pfSet);

	/////////////////////////////////////////////////////////////////
	///////*************Operators that DO NOT change inner data*************
	/////////////////////////////////////////////////////////////////


	/////////////////////////////////
	//// *****a SCALAR parameter****
	/////////////////////////////////


	////////////////////////////
	//// vector2f + float
	////////////////////////////
    CVec2f operator+(float fNum) const;
   
	////////////////////////////
	//// vector2f - float
	////////////////////////////
    CVec2f operator-(float fNum) const;

	////////////////////////////
	//// vector2f * float
	////////////////////////////
    CVec2f operator*(float fNum) const;

	////////////////////////////
	//// vector2f / float
	////////////////////////////
	CVec2f operator/(float fNum) const;

   
	/////////////////////////////////
	//// *****a VECTOR parameter****
	/////////////////////////////////

	/////////////////////////////////
	//// vector + vector
	/////////////////////////////////
    CVec2f operator+(CVec2f Vec1) const;
   
	/////////////////////////////////
	//// vector - vector
	/////////////////////////////////
    CVec2f operator-(CVec2f Vec1) const;

	/////////////////////////////////
	//// vector dot vector
	/////////////////////////////////
    float     operator*(CVec2f Vec1) const;  // dot product

	/////////////////////////////////
	//// vector / vector
	/////////////////////////////////
	CVec2f operator/(CVec2f Vec1) const;

   
   	/////////////////////////////////////////////////////////////////
	///////*************Operators that CHANGE inner data*************
	/////////////////////////////////////////////////////////////////


	/////////////////////////////////
	//// *****a SCALAR parameter****
	/////////////////////////////////

   
	/////////////////////////////////
	//// vector+=scalar
	/////////////////////////////////
	void operator+=(float fNum) ;
   
	/////////////////////////////////
	//// vector-=scalar
	/////////////////////////////////
    void operator-=(float fNum) ;

	/////////////////////////////////
	//// vector*=scalar
	/////////////////////////////////
    void operator*=(float fNum) ;

	/////////////////////////////////
	//// vector/=scalar
	/////////////////////////////////
    void operator/=(float fNum) ;

   
	/////////////////////////////////
	//// *****a VECTOR parameter****
	/////////////////////////////////

    
	/////////////////////////////////
	//// vector+=vector
	/////////////////////////////////
	void operator+=(CVec2f Vec1) ;
   
	/////////////////////////////////
	//// vector-=vector
	/////////////////////////////////
    void operator-=(CVec2f Vec1) ;

	/////////////////////////////////
	//// vector*=vector
	/////////////////////////////////
	void operator*=(CVec2f Vec1) ;

	/////////////////////////////////
	//// vector/=vector
	/////////////////////////////////
	void operator/=(CVec2f Vec1) ;


/*   //// Only use Add,Subtract,Multiply,Divide on Time Critical stuff
   ///  and only after the Program is finished

   void Add(CVec2f& v1,CVec2f& v2);

   void Subtract(CVec2f& v1,CVec2f& v2);
   
   void Multiply(CVec2f& v1,CVec2f& v2);
   
   void Divide(CVec2f& v1,CVec2f& v2);*/

   
/*   void Cross(CVec2f& v1,CVec2f& v2);

   void Inverse(CVec2f& Vec1);

   void Normalize(CVec2f& Vec1);

   void PutRayPoint(CVec2f& RayOrigin,CVec2f& RayDirection,float t);*/
  

/////////////////////////////////////////////
// Fast Operations End
/////////////////////////////////////////////

   // Operating on the Class itself

/*   float Length(void);

   void Inverse(void);

   void Normalize(void);*/

   // on another CVec3f
   
	
};

////////////////////////////////////////////////////
// ***********Stand Alone Functions***************
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// CVec3f dot CVec3f
////////////////////////////////////////////////////
float Dot(CVec3f v1,CVec3f v2);

////////////////////////////////////////////////////
// CVec2f dot CVec2f
////////////////////////////////////////////////////
float Dot(CVec2f v1,CVec2f v2);

////////////////////////////////////////////////////
// CVec3f cross CVec3f
////////////////////////////////////////////////////
CVec3f Cross(CVec3f v1,CVec3f v2);

///////////////////////////////////////////////////
// AngleBetweenVectorsANG()
// angle between two vectors (angle)
////////////////////////////////////////////////////
float AngleBetweenVectorsANG(CVec3f Vec1,CVec3f Vec2);

////////////////////////////////////////////////////
// AngleBetweenVectorsRAD()
// angle between two vectors (radian)
////////////////////////////////////////////////////
float AngleBetweenVectorsRAD(CVec3f Vec1,CVec3f Vec2);

void GetLookAtAngles(CVec3f& vFrom, CVec3f& vTo,float& angX,float& angY);

CVec3f GetRayPoint(CVec3f RayOrigin,CVec3f RayDirection,float t);

CVec3f RayPlaneIntersection(CVec3f RayOrigin, CVec3f RayDirection, 
							   CVec3f PlaneOrigin ,CVec3f PlaneNormal);




///////////////////////////////////////////////////////
// RayPlaneIntersectionT()
// returns the T intersection between a ray and a plane
// (normal should be just normal+d)
///////////////////////////////////////////////////////
/*float RayPlaneIntersectionT(CVec3f RayOrigin,CVec3f RayDirection,CVec3f PlaneOrigin ,CVec3f PlaneNormal);*/




bool PointInTriangle(CVec3f Point,CVec3f T1,CVec3f T2,CVec3f T3);


bool PointInPolygon(CVec3f& Point, CVec3f* pVerts,int iNum);



float DISTANCE(CVec3f& v1,CVec3f& v2);



////////////////////////////////////////////////
//*************CMatrix Class*************
////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
// TODO:
// an optimization would be to create a method inside CMatrix that does:
// 1. CMatrix::MatMakeRot() --> this needs to create the matrix in ONE step - without creating
//                               small (mX*mY*mZ)
// 2. CMatrix::MatMakeMoveAndRotate(x,y,z,xAng,yAng,zAng);
//
// FIXME:
// move to opengl column based system
//////////////////////////////////////////////////////////////////////////////////////////////

class CMatrix
{
public:  
  union
	{
		float m_fpMat[16];
		// m[ column ][ row ] 
		float m[4][4];
		
		struct
		{
			float xx, xy, xz, xw;
			float yx, yy, yz, yw;
			float zx, zy, zz, zw;
			float wx, wy, wz, ww;
		};

		// logical matrix:
		// xx yx zx wx
		// xy yy zy wy
		// xz yz zz wz
		// xw yw zw ww
	};

 
  inline void CMatrix::Set(const float* pMat)
  {
	  if (!pMat)
	  {
		  MatMakeUnit();
		  return;
	  }

	   //BREAKPOINT(!pMat);

 	  for (int i=0;i<4;i++)
	  	for (int j=0;j<4;j++)
	  		m[i][j] = pMat[i + j*4];		
	
	   //memcpy(m_fpMat,pMat,sizeof(float)*16);

  }

  inline void CMatrix::SetTransposed(const float* pMat)
  {
	   if (!pMat)
	  {
		  MatMakeUnit();
		  return;
	  }
	 // BREAKPOINT(!pMat);
	  //__asm { int 3 };
  	/*for (int i=0;i<4;i++)
  		for (int j=0;j<4;j++)
			m[j][i] = pMat[i + j*4];	*/

	   memcpy(m,pMat,sizeof(float)*16);
  }
  
  //////////////////////////////////
  // MatMakeUnit()
  // Init matrix to a unit matrix
  //////////////////////////////////
  void MatMakeUnit();

  ////////////////////////////////
  // MatImportFrom3x3()
  // Import from a 3x3 matrix
  ///////////////////////////////
  void MatImportFrom3x3(float* pInput);

  //////////////////////////////////
  // MatMakeMove()
  // a translation matrix
  //////////////////////////////////
  void MatMakeMove(float x,float y,float z);

  //////////////////////////////////
  // MatMakeScale()
  // scale matrix
  //////////////////////////////////
  void MatMakeScale(float x,float y,float z);



  // get determinant of matrix
  float Determinant(void) const;

  ///////////////////////////////////////////////
  // Inverse()
  // FIXME: change into correct implementation!
  ///////////////////////////////////////////////
  void Inverse(void);


  
  ///////////////////////////////////////////
  // Transpose()
  // Transposes self
  ///////////////////////////////////////////
  void Transpose(void);
	
  

  ///////////////////////////////////////////
  // MatMakeXRot()
  // a rotation matrix (around the x axis)
  ///////////////////////////////////////////
  void MatMakeXRot(float radians);

  ///////////////////////////////////////////
  // MatMakeYRot()
  // a rotation matrix (around the y axis)
  ///////////////////////////////////////////
  void MatMakeYRot(float radians);

  ///////////////////////////////////////////
  // MatMakeZRot()
  // a rotation matrix (around the z axis)
  ///////////////////////////////////////////
  void MatMakeZRot(float radians);

  ///////////////////////////////////////////
  // MatMakeZRot()
  // a rotation matrix (around the x,y,z axis)
  ///////////////////////////////////////////
  void MatMakeRot(float xrad,float yrad,float zrad);


/*  ///////////////////////////////////////////
  // SetPerspective()
  // sets a perspective matrix
  ///////////////////////////////////////////
  void SetPerspective(	float left, float right, float bottom,
								float top, float n, float f);

  ///////////////////////////////////////////
  // SetPerspective()
  // sets a perspective matrix
  ///////////////////////////////////////////
  void SetPerspective(float fovy, float aspect, float n, float f);*/


  ////////////////////////////////////////////////////////
  // Return the array version of the matrix
  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  // Return the array version of the matrix
	 ////////////////////////////////////////////////////////
	inline void CMatrix::ReturnArray(float * faMatrix)
	{
		/*for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
		{
			faMatrix[i + j*4] = m[i][j];			
		}*/

		// RESTORE RESTORE RESTORE

		 memcpy(faMatrix,m_fpMat,sizeof(float)*16);
	}

	////////////////////////////////////////////////////////
	// Turn my ROW based matrix into a COLUMN based matrix
	////////////////////////////////////////////////////////
	inline void CMatrix::ReturnTransposedArray(float * faMatrix)
	{
//		__asm { int 3 };
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
		{
			//faMatrix[i + j*4] = m[i][j];
			faMatrix[i + j*4] = m[j][i];
		}
	}



  ///////////////////////////////////////////////////////////////////
  ////////////********* Matrix Operators************/////////////////
  ///////////////////////////////////////////////////////////////////

 
  
  ///////////////////////////////////////////
  // vector3 * matrix
  ///////////////////////////////////////////
  inline CVec3f CMatrix::operator * (const CVec3f v)
  {
    CVec3f res;
    res.v[0] = ( v.v[0] * m[0][0] ) + ( v.v[1] * m[0][1] ) + ( v.v[2] * m[0][2] ) + m[0][3];
    res.v[1] = ( v.v[0] * m[1][0] ) + ( v.v[1] * m[1][1] ) + ( v.v[2] * m[1][2] ) + m[1][3];
    res.v[2] = ( v.v[0] * m[2][0] ) + ( v.v[1] * m[2][1] ) + ( v.v[2] * m[2][2] ) + m[2][3];
    return res;  
  }

    ///////////////////////////////////////////
	// vector4 * matrix
	///////////////////////////////////////////
	inline CVector4f CMatrix::operator * (const CVector4f v)
	{
		CVector4f res;
		res.v[0] = ( v.v[0] * m[0][0] ) + ( v.v[1] * m[0][1] ) + ( v.v[2] * m[0][2] ) + (v.v[3] * m[0][3]);
		res.v[1] = ( v.v[0] * m[1][0] ) + ( v.v[1] * m[1][1] ) + ( v.v[2] * m[1][2] ) + (v.v[3] * m[1][3]);
		res.v[2] = ( v.v[0] * m[2][0] ) + ( v.v[1] * m[2][1] ) + ( v.v[2] * m[2][2] ) + (v.v[3] * m[2][3]);
		res.v[3] = ( v.v[0] * m[3][0] ) + ( v.v[1] * m[3][1] ) + ( v.v[2] * m[3][2] ) + (v.v[3] * m[3][3]);

		return res;  
	}



  ///////////////////////////////////////////
  // matrix * matrix
  ///////////////////////////////////////////
  inline CMatrix CMatrix::operator * (const CMatrix mat)
  {
    /*CMatrix ret;
	ret.xx=xx*mat.xx+yx*mat.xy+zx*mat.xz+wx*mat.xw;
	ret.yx=xx*mat.yx+yx*mat.yy+zx*mat.yz+wx*mat.yw;
	ret.zx=xx*mat.zx+yx*mat.zy+zx*mat.zz+wx*mat.zw;
	ret.wx=xx*mat.wx+yx*mat.wy+zx*mat.wz+wx*mat.ww;

	ret.xy=xy*mat.xx+yy*mat.xy+zy*mat.xz+wy*mat.xw;
	ret.yy=xy*mat.yx+yy*mat.yy+zy*mat.yz+wy*mat.yw;
	ret.zy=xy*mat.zx+yy*mat.zy+zy*mat.zz+wy*mat.zw;
	ret.wy=xy*mat.wx+yy*mat.wy+zy*mat.wz+wy*mat.ww;

	ret.xz=xz*mat.xx+yz*mat.xy+zz*mat.xz+wz*mat.xw;
	ret.yz=xz*mat.yx+yz*mat.yy+zz*mat.yz+wz*mat.yw;
	ret.zz=xz*mat.zx+yz*mat.zy+zz*mat.zz+wz*mat.zw;
	ret.wz=xz*mat.wx+yz*mat.wy+zz*mat.wz+wz*mat.ww;

	ret.xw=xw*mat.xx+yw*mat.xy+zw*mat.xz+ww*mat.xw;
	ret.yw=xw*mat.yx+yw*mat.yy+zw*mat.yz+ww*mat.yw;
	ret.zw=xw*mat.zx+yw*mat.zy+zw*mat.zz+ww*mat.zw;
	ret.ww=xw*mat.wx+yw*mat.wy+zw*mat.wz+ww*mat.ww;
	return ret;*/

//	   __asm { int 3 };

	int i,j,k;
    CMatrix res;

    for (i=0;i<4;i++)
      for (j=0;j<4;j++)
      {
        res.m[j][i]=0;
        for (k=0;k<4;k++)
			res.m[j][i] += m[j][k] * mat.m[k][i];
      }       
    return res;
  }

  ///////////////////////////////////////////
  // matrix = matrix
  ///////////////////////////////////////////
  CMatrix operator = (const CMatrix m1);

  ///////////////////////////////////////////
  // matrix = pointer to matrix (faster)
  ///////////////////////////////////////////
  CMatrix operator = (const CMatrix* m1);
};



CVec3f GetVectorFromAngles(float fAngX,float fAngY);

CVec3f RotateVectorByAngle(float fAngX,float fAngY,const CVec3f& vec);

///////////////////////////////////////////
// class CPlane
///////////////////////////////////////////

class  CPlane
{
public:
	CPlane() {};
	~CPlane() {};

	void FindPlane(const CVec3f& vec1, const CVec3f& vec2, const CVec3f& vec3);

	void FindPlane(const CVec3f& point, const CVec3f& normal);

	void FillFromOtherPlaneAndMultiplyByMatrix(CPlane& from, CMatrix& m);

	bool IsPointOnPlane(CVec3f& pPoint);

	inline void operator = (const CPlane &plane)
	{
		n = plane.n;
		d = plane.d;
	}


	bool IsPointOnPlane(CVec3f& pPoint,float fWithinDistance);

	void Normalize(void);

	CVec3f n;			// Plane normal. 
	float     d;					// The plane distance from origin 
};



CVec3f RayPlaneIntersection(CVec3f& RayStart,CVec3f& RayDirection,CPlane &Plane);

float RayPlaneIntersectionT(CVec3f& RayOrigin,CVec3f& RayDirection,CPlane& plane);



struct BBox
{
	CVec3f min,max;

	void Set(const BBox* pBBox)
	{
		assert( pBBox);
		min = pBBox->min;
		max = pBBox->max;
	}

	void Add(const BBox* pBBox)
	{
		if (pBBox->min.x < min.x)
			min.x = pBBox->min.x;
		if (pBBox->min.y < min.y)
			min.y = pBBox->min.y;
		if (pBBox->min.z < min.z)
			min.z = pBBox->min.z;

		if (pBBox->max.x > max.x)
			max.x = pBBox->max.x;
		if (pBBox->max.y > max.y)
			max.y = pBBox->max.y;
		if (pBBox->max.z > max.z)
			max.z = pBBox->max.y;
	}
};


// if (pIndices==NULL) all vertices are checked
BBox ComputeBBox(CVec3f* pVertices, UINT* pIndices, int iIndicesNum);

bool CheckBBoxSphereIntersection(const BBox& bbox, const CVec3f& vPos, float fRadius);

BBox GetBBoxFromSphere(CVec3f& center, float fRadius);
void GetSphereFromBBox(const BBox& bbox, CVec3f& vCenter, float& fRadius);

bool PointInBBox(const CVec3f& point,const BBox& bbox);


bool BBoxesIntersect(const CVec3f&min1,const CVec3f&max1,const CVec3f&min2,const CVec3f&max2);

bool BBoxesIntersect(const BBox& bb1, const BBox& bb2);


void MakeBrushFromBoundingBox(CVec3f& vMin, CVec3f& vMax,CPlane* pPlanes);

CVec3f Normalize(CVec3f& vec);


//FIXME: there's a problem with high value sweeps (sweeps to a long distance!!!)
// values higher then 100 units seem to fail!
bool AABBSweep(const CVec3f& Ea, //extents of AABB A
					  const CVec3f& A0, //its previous position
					  const CVec3f& A1, //its current position
					  const CVec3f& Eb, //extents of AABB B
					  const CVec3f& B0, //its previous position
           			  const CVec3f& B1, //its current position
					  float& u0,
					  CVec3f& normal);
	

#endif	//YOEL_MATH3D_H