#include "stdafx.h"
#include <math.h>
#include "GMath.h"
#include "Math3d.h"
#include <assert.h>
///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

/////////////////////////////////////////////
// class CColor4
// *****************
// contains a 4 char  array
//////////////////////////////////////////////

///////////////////////////
// Constructer
///////////////////////////
CColor4::CColor4()
{
	c[0]=c[1]=c[2]=c[3]=0;
}


////////////////////////////////
// Constructer, accepts 4 chars
////////////////////////////////
CColor4::CColor4(const char r,const char g,const char b,const char a)
{
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;
	
}

/////////////////////////
// Set()
// simply set the values
/////////////////////////
void CColor4::Set(const char r,const char g,const char b,const char a)
{
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;
}

//////////////////////////////////////////////////
// Set() -- faster
// accepts a reference to a 4 char array
///////////////////////////////////////////////////
void CColor4::Set(const CColor4& color)
{
// optimize this - memcpy or sth
	c[0] = color.c[0];
	c[1] = color.c[1];
	c[2] = color.c[2];
	c[3] = color.c[3];
}















///////////////////////////////////////////////////
// *********class CVec3f***********/////////////
///////////////////////////////////////////////////

////////////////////////////////////////////
// Basic Constructer
// notice: v[0,1,2] aren't inited, for speed
////////////////////////////////////////////
CVec3f::CVec3f()
   {
	  // commenting this would be not safe but faster  
      //v[0] = v[1] = v[2] = 0;
   };



CVec3f::CVec3f(float* pfXYZ)
{
	memcpy(v,pfXYZ,sizeof(float)*3);

}

//////////////////////////////////
// Constructer with set values
//////////////////////////////////
CVec3f::CVec3f(float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
CVec3f::CVec3f(const CVec3f& vSet)
{
	v[0] = vSet.v[0];
	v[1] = vSet.v[1];
	v[2] = vSet.v[2];
}


/////////////////////////////////////////////////
// ***********methods to alter data*************
/////////////////////////////////////////////////

//TODO: operator = on other things (like float*, so I can memcpy it)

//////////////////////
// operator: "="
///////////////////////
CVec3f CVec3f::operator=(const CVec3f &pVec)
{
	return CVec3f(v[0] = pVec.v[0],v[1] = pVec.v[1],v[2] = pVec.v[2]);
}

/////////////////////
// Set() 
/////////////////////
void CVec3f::Set(float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

///////////////////////
// Set()
///////////////////////
void CVec3f::Set(CVec3f &Vec3f)
{
	v[0] = Vec3f.v[0];
	v[1] = Vec3f.v[1];
	v[2] = Vec3f.v[2];
}

//////////////////////////////////////////////////
// Set() - FASTER
// (pfSet must point to at least 3 valid floats)
//////////////////////////////////////////////////
void CVec3f::Set(float* pfSet)
{

	///fixme: an obvious memcpy should be here ;)

	v[0] = pfSet[0];
	v[1] = pfSet[1];
	v[2] = pfSet[2];
}



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
CVec3f CVec3f::operator+(float fNum) const
{
 // Return the translated vector
   return CVec3f(v[0] + fNum, v[1] + fNum, v[2] + fNum);
}


////////////////////
// CVector - float
////////////////////
CVec3f CVec3f::operator-(float fNum) const
{
 // Return the scaled vector
   return CVec3f(v[0] - fNum, v[1] - fNum, v[2] - fNum);
}

////////////////////
// CVector * float
////////////////////
CVec3f CVec3f::operator*(float fNum) const
{
 // Return the scaled vector
   return CVec3f(v[0] * fNum, v[1] * fNum, v[2] * fNum);
}


////////////////////
// CVector / float
////////////////////
CVec3f CVec3f::operator/(float fNum) const
{
 // Return the scaled vector
   return CVec3f(v[0] / fNum, v[1] / fNum, v[2] / fNum);
}

//////////////////////////////////////////////
// ***********Vector parameter***********
//////////////////////////////////////////////





/*/////////////////////////////
// Vector / Vector
/////////////////////////////
float CVec3f::operator*(CVec3f& Vec1) const
{
	return v[0]*Vec1.v[0]+v[1]*Vec1.v[1]+v[2]*Vec1.v[2];
}*/


/////////////////////////
// Vector dot Vector
///////////////////////////
CVec3f CVec3f::operator/(const CVec3f& Vec1) const
{
return CVec3f(v[0]/Vec1.v[0],v[1]/Vec1.v[1],v[2]/Vec1.v[2]);
}



/////////////////////////////////////////////////////////////////
///////*************Operators that DO change inner data*************
/////////////////////////////////////////////////////////////////


////////////////////////////////////////////
// note: NO CHECK FOR DEVIDE BY ZERO!!!!!
////////////////////////////////////////////


////////////////////////////
//// a SCALAR parameter
////////////////////////////

/////////////////////////
// vector+=scalar
/////////////////////////
void CVec3f::operator+=(float fNum) 
{
 v[0]+= fNum;
 v[1]+= fNum;
 v[2]+= fNum;
}

/////////////////////////
// vector-=scalar
/////////////////////////
void CVec3f::operator-=(float fNum) 
{
 v[0]-= fNum;
 v[1]-= fNum;
 v[2]-= fNum;
}

/////////////////////////
// vector*=scalar
/////////////////////////
void CVec3f::operator*=(float fNum) 
{
 v[0]*= fNum;
 v[1]*= fNum;
 v[2]*= fNum;
}


/////////////////////////
// vector/=scalar
/////////////////////////
void CVec3f::operator/=(float fNum) 
{
 v[0]/= fNum;
 v[1]/= fNum;
 v[2]/= fNum;
}

///////////////////////////////////////////////////////////////////////////
//// a VECTOR parameter
//// TODO: maybe i should pass the parameter as a reference (also const)
///////////////////////////////////////////////////////////////////////////



///

/*void CVec3f::Cross(CVec3f& v1,CVec3f& v2)
{
	v[0] = v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1];
	v[1] = v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2];
	v[2] = v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0];
}

void CVec3f::Add(CVec3f& v1,CVec3f& v2)
{
	v[0] = v1.v[0] + v2.v[0];
	v[1] = v1.v[1] + v2.v[1];
	v[2] = v1.v[2] + v2.v[2];
}

void CVec3f::Subtract(CVec3f& v1,CVec3f& v2)
{
	v[0] = v1.v[0] - v2.v[0];
	v[1] = v1.v[1] - v2.v[1];
	v[2] = v1.v[2] - v2.v[2];
}
   
void CVec3f::Multiply(CVec3f& v1,CVec3f& v2)
{
	v[0] = v1.v[0] * v2.v[0];
	v[1] = v1.v[1] * v2.v[1];
	v[2] = v1.v[2] * v2.v[2];
}
   
void CVec3f::Divide(CVec3f& v1,CVec3f& v2)
{
	// TODO: Should I check for divide by Zero here?
	v[0] = v1.v[0] / v2.v[0];
	v[1] = v1.v[1] / v2.v[1];
	v[2] = v1.v[2] / v2.v[2];
}
 
void CVec3f::Inverse(CVec3f& Vec1)
{
	v[0] = -( Vec1.v[0] );
	v[1] = -( Vec1.v[1] );
	v[2] = -( Vec1.v[2] );
}

void CVec3f::Normalize(CVec3f& Vec1)
{
	float t;
	t = (float) sqrt( Vec1.v[0]*Vec1.v[0]+Vec1.v[1]*Vec1.v[1]+Vec1.v[2]*Vec1.v[2]);
	
	// Replace this with Set(blah) or sth
	v[0] = Vec1.v[0];
	v[1] = Vec1.v[1];
	v[2] = Vec1.v[2];

	v[0] /= t;
	v[1] /= t;
	v[2] /= t;
}*/



///////////////////////////////////
// Operating on the Class itself
// returns a result, no parameters
///////////////////////////////////

///////////////////////////////////
// Length()
// get the length of the vector
//////////////////////////////////
float CVec3f::Length(void) const
{
	//return (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	
}

//////////////////////////////////
// Inverse()
// *(-1) for each axis component
//////////////////////////////////
void CVec3f::Inverse(void)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}




float CVec3f::GetSize(void)
{
	return (float) sqrt( v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}


/*void CVec3f::PutRayPoint(CVec3f& RayOrigin,CVec3f& RayDirection,float t)
{	
	v[0]=RayOrigin.v[0] + t * RayDirection.v[0];
	v[1]=RayOrigin.v[1] + t * RayDirection.v[1];
	v[2]=RayOrigin.v[2] + t * RayDirection.v[2];
}*/


















///////////////////////////////////////////////////
// *********class CVector4f***********/////////////
///////////////////////////////////////////////////

////////////////////////////////////////////
// Basic Constructer
// notice: v[0,1,2] aren't inited, for speed
////////////////////////////////////////////
CVector4f::CVector4f()
{
	// commenting this would be not safe but fast 
	//v[0] = v[1] = v[2] = v[3]=0;
};


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
CVector4f::CVector4f(float x, float y, float z,float d)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = d;
}

CVector4f CVector4f::operator=(const CVector4f &pVec)
{
	return CVector4f(v[0] = pVec.v[0],v[1] = pVec.v[1],v[2] = pVec.v[2],v[3] = pVec.v[3]);
}

//////////////////////////////////
// Constructer with set values
//////////////////////////////////
void CVector4f::Set(float x, float y, float z,float d)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = d;
}


//////////////////////////////////
// Constructer with set values
//////////////////////////////////
/*CVector4f::CVector4f(const CVector4f& vSet)
{
	v[0] = vSet.v[0];
	v[1] = vSet.v[1];
	v[2] = vSet.v[2];
	v[3] = vSet.v[3];
}*/



CVector4f CVector4f::Normalize(void)
{
	float t;
	t = (float) sqrt( v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);

	v[0] /= t;
	v[1] /= t;
	v[2] /= t;
	v[3] /= t;

	return (*this);

}




















///////////////////////////////////////////////////
// *********class CVec2f***********/////////////
///////////////////////////////////////////////////


///////////////////////////////////////////////
// *********** Constructers ******************
///////////////////////////////////////////////


///////////////////////
//Constructer()
///////////////////////
CVec2f::CVec2f()
   {
      v[0] = v[1] = 0;
   }

////////////////////////////////////
//Constructer()
// constructs and inits using x,y
////////////////////////////////////
CVec2f::CVec2f(float x, float y)
{
	v[0] = x;
	v[1] = y;
}

///////////////////////////////
//Set() - accepts 2 floats
////////////////////////////////
void CVec2f::Set(float x, float y)
{
	v[0] = x;
	v[1] = y;
}

/////////////////////////////////////////
//Set() // accept a reference to a vector
/////////////////////////////////////////
void CVec2f::Set(CVec2f &Vec2f)
{
	v[0] = Vec2f.v[0];
	v[1] = Vec2f.v[1];
}

//////////////////////////////////////////////////////////
//Set() // accepts a float array (of at least 2 elements)
///////////////////////////////////////////////////////////
void CVec2f::Set(float* pfSet)
{
	v[0] = pfSet[0];
	v[1] = pfSet[1];
}



/////////////////////////////////////////////////////////////////
///////*************Operators that DO NOT change inner data*************
/////////////////////////////////////////////////////////////////


/////////////////////////////////
//// *****a SCALAR parameter****
/////////////////////////////////


////////////////////////////
//// vector2f + float
////////////////////////////
CVec2f CVec2f::operator+(float fNum) const
{
 // Return the scaled vector
   return CVec2f(v[0] + fNum, v[1] + fNum);
}

////////////////////////////
//// vector2f - float
////////////////////////////
CVec2f CVec2f::operator-(float fNum) const
{
 // Return the scaled vector
   return CVec2f(v[0] - fNum, v[1] - fNum);
}

////////////////////////////
//// vector2f * float
////////////////////////////
CVec2f CVec2f::operator*(float fNum)const 
{
 // Return the scaled vector
   return CVec2f(v[0] * fNum, v[1] * fNum);
}

////////////////////////////
//// vector2f / float
////////////////////////////
CVec2f CVec2f::operator/(float fNum) const
{
 // Return the scaled vector
   return CVec2f(v[0] / fNum, v[1] / fNum);
}

/////////////////////////////////
//// *****a VECTOR parameter****
/////////////////////////////////

/////////////////////////////////
//// vector + vector
/////////////////////////////////
CVec2f CVec2f::operator+(CVec2f Vec1)const
{
	return CVec2f(v[0]+Vec1.v[0],v[1]+Vec1.v[1]);
}

/////////////////////////////////
//// vector - vector
/////////////////////////////////
CVec2f CVec2f::operator-(CVec2f Vec1)const
{
	return CVec2f(v[0]-Vec1.v[0],v[1]-Vec1.v[1]);
}

/////////////////////////////////
//// vector dot vector
/////////////////////////////////
float CVec2f::operator*(CVec2f Vec1)const
{
	return v[0]*Vec1.v[0]+v[1]*Vec1.v[1];
}

/////////////////////////////////
//// vector / vector
/////////////////////////////////
CVec2f CVec2f::operator/(CVec2f Vec1)const
{
	return CVec2f(v[0]/Vec1.v[0],v[1]/Vec1.v[1]);
}


/////////////////////////////////////////////////////////////////
///////*************Operators that CHANGE inner data*************
/////////////////////////////////////////////////////////////////


/////////////////////////////////
//// *****a SCALAR parameter****
/////////////////////////////////


/////////////////////////////////
//// vector+=scalar
/////////////////////////////////
void CVec2f::operator+=(float fNum) 
{
 v[0]+= fNum;
 v[1]+= fNum;
}

/////////////////////////////////
//// vector-=scalar
/////////////////////////////////
void CVec2f::operator-=(float fNum) 
{
 v[0]-= fNum;
 v[1]-= fNum;
}

/////////////////////////////////
//// vector*=scalar
/////////////////////////////////
void CVec2f::operator*=(float fNum) 
{
 v[0]*= fNum;
 v[1]*= fNum;
}

/////////////////////////////////
//// vector/=scalar
/////////////////////////////////
void CVec2f::operator/=(float fNum) 
{
 v[0]/= fNum;
 v[1]/= fNum;
}


/////////////////////////////////
//// *****a VECTOR parameter****
/////////////////////////////////


/////////////////////////////////
//// vector+=vector
/////////////////////////////////
void CVec2f::operator+=(CVec2f Vec1) 
{
 v[0]+= Vec1.v[0];
 v[1]+= Vec1.v[1];
}

/////////////////////////////////
//// vector-=vector
/////////////////////////////////
void CVec2f::operator-=(CVec2f Vec1) 
{
 v[0]-= Vec1.v[0];
 v[1]-= Vec1.v[1];
}

/////////////////////////////////
//// vector*=vector
/////////////////////////////////
void CVec2f::operator*=(CVec2f Vec1) 
{
 v[0]*= Vec1.v[0];
 v[1]*= Vec1.v[1];
}

/////////////////////////////////
//// vector/=vector
/////////////////////////////////
void CVec2f::operator/=(CVec2f Vec1) 
{
 v[0]/= Vec1.v[0];
 v[1]/= Vec1.v[1];
}


/*
void CVec2f::Add(CVec2f& v1,CVec2f& v2)
{
	v[0] = v1.v[0] + v2.v[0];
	v[1] = v1.v[1] + v2.v[1];
}

void CVec2f::Subtract(CVec2f& v1,CVec2f& v2)
{
	v[0] = v1.v[0] - v2.v[0];
	v[1] = v1.v[1] - v2.v[1];
}
   
void CVec2f::Multiply(CVec2f& v1,CVec2f& v2)
{
	v[0] = v1.v[0] * v2.v[0];
	v[1] = v1.v[1] * v2.v[1];
	v[2] = v1.v[2] * v2.v[2];
}
   
void CVec2f::Divide(CVec2f& v1,CVec2f& v2)
{
	// TODO: Should I check for divide by Zero here?
	v[0] = v1.v[0] / v2.v[0];
	v[1] = v1.v[1] / v2.v[1];
}
 


*/




////////////////////////////////////////////////////
// ***********Stand Alone Functions***************
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// CVec3f dot CVec3f
////////////////////////////////////////////////////
float Dot(CVec3f v1,CVec3f v2)
{
	return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1] + v1.v[2] * v2.v[2];
}

////////////////////////////////////////////////////
// CVec2f dot CVec2f
////////////////////////////////////////////////////
float Dot(CVec2f v1,CVec2f v2)
{
	return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1];
}

////////////////////////////////////////////////////
// CVec3f cross CVec3f
////////////////////////////////////////////////////
CVec3f Cross(CVec3f v1,CVec3f v2)
{
	return CVec3f((v1.v[1] * v2.v[2]) - (v1.v[2] * v2.v[1]),
					 (v1.v[2] * v2.v[0]) - (v1.v[0] * v2.v[2]),
					 (v1.v[0] * v2.v[1]) - (v1.v[1] * v2.v[0]));
}

///////////////////////////////////////////////////
// AngleBetweenVectorsANG()
// angle between two vectors (angle)
////////////////////////////////////////////////////
float AngleBetweenVectorsANG(CVec3f Vec1,CVec3f Vec2)
{
    //return RAD2DEG(	 acos(Dot(Vec1,Vec2) / (Vec1.Length() * Vec2.Length()))	 );
	return RAD2DEG(	 acosf(Dot(Vec1,Vec2) / (Vec1.Length() * Vec2.Length()))	 );
}

////////////////////////////////////////////////////
// AngleBetweenVectorsRAD()
// angle between two vectors (radian)
////////////////////////////////////////////////////
float AngleBetweenVectorsRAD(CVec3f Vec1,CVec3f Vec2)
{
    return 	 acosf(Dot(Vec1,Vec2) / (Vec1.Length() * Vec2.Length()))	 ;
}


void GetLookAtAngles(CVec3f& vFrom, CVec3f& vTo,float& angX,float& angY)
{

	CVec3f vDir = (vTo-vFrom).Normalize();

    if(vDir.v[0] == 0) 
	{

        if(vDir.v[2] < 0)
            angY = 180;
        else
            angY = 0;
    }
    else
        angY = RAD2DEG( -atan2f(vDir.v[0], vDir.v[2]) );

    angX = RAD2DEG( asinf(vDir.v[1]));


	angY = - angY;

	angY+=180.f;






	// all of this function is really buggy and causing bad float values

/*	
	// DEBUG DEBUG DEBUG
	angX = 0.f;
	angY = 0.f;

	return;
	// DEBUG DEBUG DEBUG
	
*/



/*


	// handle angY

	CVec3f v1;
	v1.v[0] = vTo.v[0] - vFrom.v[0];
	v1.v[1] = 0.f;
	v1.v[2] = vTo.v[2] - vFrom.v[2];


	float ang1;

	if (DISTANCE(CVec3f(0.f,0.f,-1.f),v1)>0.01f)
		ang1 = AngleBetweenVectorsANG( (vTo - vFrom).Normalize() ,v1.Normalize());
	else 
		ang1 = 0.f;



	if (vTo.v[0]<vFrom.v[0])
		angY = ang1;
	else
		angY = -ang1;
	
	
	if (angY>=180.f && angY<=360.f)
		angY = angX - 360.f;
	else
	if (angY<=-180.f && angY>=-360.f)
		angY = angY + 360.f;
	

	
	// handle angX

	
	// DEBUG DEBUG DEBUG
	//angX = 0.f;
	//return;
	// DEBUG DEBUG DEBUG
	


	v1.v[0] = vTo.v[0] - vFrom.v[0];
	v1.v[1] = 0.f;
	v1.v[2] = vTo.v[2] - vFrom.v[2];




	if (DISTANCE((vTo - vFrom).Normalize(),v1.Normalize())>0.01f)
		ang1 = AngleBetweenVectorsANG( (vTo - vFrom).Normalize() ,v1.Normalize());
	else 
		ang1 = 0.f;


	if (vTo.v[1]>vFrom.v[1])
		angX = ang1;
	else
		angX = -ang1;
		
	


	if (angX>=180.f && angX<=360.f)
		angX = angX - 360.f;
	else
	if (angX<=-180.f && angX>=-360.f)
		angX = angX + 360.f;
	

	//angX = 0.f;

	
*/


}

////////////////////////////////////////////////////
// GetRayPoint()
// returns a point on a ray
////////////////////////////////////////////////////
CVec3f GetRayPoint(CVec3f RayOrigin,CVec3f RayDirection,float t)
{	

	return CVec3f(RayOrigin.v[0] + t * RayDirection.v[0],
					 RayOrigin.v[1] + t * RayDirection.v[1],
					 RayOrigin.v[2] + t * RayDirection.v[2]);
}

///////////////////////////////////////////////////////
// RayPlaneIntersection()
// returns the intersection between a ray and a plane
// (normal should be just normal+d)
///////////////////////////////////////////////////////
CVec3f RayPlaneIntersection(CVec3f RayOrigin,CVec3f RayDirection,CVec3f PlaneOrigin ,CVec3f PlaneNormal)
{
	float d = - (Dot(PlaneNormal,PlaneOrigin));
	float numer = Dot(PlaneNormal,RayOrigin) + d;
	float denom = Dot(PlaneNormal,RayDirection);
	float t =  - (numer / denom);
	return GetRayPoint(RayOrigin,RayDirection,t);
}



///////////////////////////////////////////////////////
// RayPlaneIntersectionT()
// returns the T intersection between a ray and a plane
// (normal should be just normal+d)
///////////////////////////////////////////////////////
/*float RayPlaneIntersectionT(CVec3f RayOrigin,CVec3f RayDirection,CVec3f PlaneOrigin ,CVec3f PlaneNormal)
{
	//float d = - (Dot(PlaneNormal,PlaneOrigin));

	float d = (Dot(PlaneNormal,PlaneOrigin));

	float numer = Dot(PlaneNormal,RayOrigin) + d;
	float denom = Dot(PlaneNormal,RayDirection);
	float t =  - (numer / denom);
	return t;
}*/





///////////////////////////////////////////////////////
// PointInTriangle()
// false = point OUTSIDE triangle
// true  = point ON triangle
///////////////////////////////////////////////////////
bool PointInTriangle(CVec3f Point,CVec3f T1,CVec3f T2,CVec3f T3)
{
	    // TODO: change this
		CVec3f v1,v2,v3;
		float angle1,angle2,angle3,AnglesSum;
		v1 = T1 - Point;
		v2 = T2 - Point;
		v3 = T3 - Point;
		angle1 = AngleBetweenVectorsANG(v1,v2);
		angle2 = AngleBetweenVectorsANG(v2,v3);
		angle3 = AngleBetweenVectorsANG(v1,v3);
		AnglesSum = angle1+angle2+angle3;

		if (AnglesSum > 359.5f && AnglesSum<360.5f)
		{
			return true;
		}
		return false;
}


bool PointInPolygon(CVec3f& Point, CVec3f* pVerts,int iNum)
{
	int iFirst,iSecond;

	float AnglesSum=0.f;

	CVec3f v1,v2;



	for (int i=0;i<iNum;i++)
	{	
		if (i==iNum-1) // last edge
		{
			iFirst=i;
			iSecond=0;
		}
		else
		{
			iFirst=i;
			iSecond=i+1;
		}

		v1 = pVerts[iFirst]  - Point;
		v2 = pVerts[iSecond] - Point;

		v1.Normalize();
		v2.Normalize();

		AnglesSum+= AngleBetweenVectorsANG( v1,v2 );
		//AnglesSum+= AngleBetweenVectorsANG( v2,v1 );


	}

	if (AnglesSum > 359.0f && AnglesSum<361.0f)
	{
		return true;
	}
	return false;

}



float DISTANCE(CVec3f& v1,CVec3f& v2)
{
	float a = (v1.v[0] - v2.v[0]);
	float b = (v1.v[1] - v2.v[1]);
	float c = (v1.v[2] - v2.v[2]);

	return sqrt (a*a + b*b + c*c);
}



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




//////////////////////////////////
// MatMakeUnit()
// Init matrix to a unit matrix
//////////////////////////////////
void CMatrix::MatMakeUnit()
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			if (j!=i) m[j][i]=0; 
				else m[j][i]=1;
}


//////////////////////////////////
// MatMakeMove()
// a translation matrix
//////////////////////////////////
void CMatrix::MatMakeMove(float x,float y,float z)
{
	int i,j;
	for (i=0;i<3;i++)
		for (j=0;j<4;j++)
			if (j!=i) m[j][i]=0;else m[j][i]=1;

	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;
	m[3][3] = 1;
}


//////////////////////////////////
// MatMakeScale()
// change size matrix
//////////////////////////////////
void CMatrix::MatMakeScale(float x,float y,float z)
{
	int i,j;
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			m[j][i]=0;

	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	m[3][3] = 1;
}


///////////////////////////////////////////
// Transpose()
// Transposes self
///////////////////////////////////////////
void CMatrix::Transpose(void)
{
/*	__asm { int 3 };*/
	float temp;
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
		{
			if (i>j)
			{
				temp = m[i][j];
				m[i][j] = m[j][i];
				m[j][i] = temp;

			}
		}
}



///////////////////////////////////////////////
// Inverse()
// FIXME: change into correct implementation!
///////////////////////////////////////////////
void CMatrix::Inverse(void)
{
	CMatrix V1(*this);
	CMatrix V2;
	V2.MatMakeUnit();


	unsigned int r, c;
	unsigned int cc;
	unsigned int RowMax; // Points to max abs value row in this column
	unsigned int row;
	float tmp;


	// Go through columns
	for (c=0; c<4; c++)
	{
		// Find the row with max value in this column
		RowMax = c;
		for (r=c+1; r<4; r++)
		{
			if (fabs(V1.m[c][r]) > fabs(V1.m[c][RowMax]))
			{
				RowMax = r;
			}
		}

		// If the max value here is 0, we can't invert.  Return identity.
		if (V1.m[RowMax][c] == 0.0F)
		{
			// there is no invert of this matrix!
			MatMakeUnit();
			return;
		}

		// Swap row "RowMax" with row "c"
		for (cc=0; cc<4; cc++)
		{
			tmp = V1.m[cc][c];
			V1.m[cc][c] = V1.m[cc][RowMax];
			V1.m[cc][RowMax] = tmp;
			tmp = V2.m[cc][c];
			V2.m[cc][c] = V2.m[cc][RowMax];
			V2.m[cc][RowMax] = tmp;
		}

		// Now everything we do is on row "c".
		// Set the max cell to 1 by dividing the entire row by that value
		tmp = V1.m[c][c];
		for (cc=0; cc<4; cc++)
		{
			V1.m[cc][c] /= tmp;
			V2.m[cc][c] /= tmp;
		}

		// Now do the other rows, so that this column only has V1.m 1 and 0's
		for (row = 0; row < 4; row++)
		{
			if (row != c)
			{
				tmp = V1.m[c][row];
				for (cc=0; cc<4; cc++)
				{
					V1.m[cc][row] -= V1.m[cc][c] * tmp;
					V2.m[cc][row] -= V2.m[cc][c] * tmp;
				}
			}
		}
	}

	*this = V2;

}

float CMatrix::Determinant(void) const
{
	return	  (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
		- (m[0][0] * m[1][2] - m[0][2] * m[1][0]) * (m[2][1] * m[3][3] - m[2][3] * m[3][1])
		+ (m[0][0] * m[1][3] - m[0][3] * m[1][0]) * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
		+ (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * (m[2][0] * m[3][3] - m[2][3] * m[3][0])
		- (m[0][1] * m[1][3] - m[0][3] * m[1][1]) * (m[2][0] * m[3][2] - m[2][2] * m[3][0])
		+ (m[0][2] * m[1][3] - m[0][3] * m[1][2]) * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
	
}

///////////////////////////////////////////
// MatMakeXRot()
// a rotation matrix (around the x axis)
///////////////////////////////////////////
void CMatrix::MatMakeXRot(float radians )
{
  float c = (float)cos( radians );
  float s = (float)sin( radians );


  m[0][0] = 1.0f;  m[1][0] = 0.0f;    m[2][0] = 0.0f;    m[3][0] = 0.0f;
  m[0][1] = 0.0f;  m[1][1] = c;       m[2][1] = s;       m[3][1] = 0.0f;
  m[0][2] = 0.0f;  m[1][2] = -s;      m[2][2] = c;       m[3][2] = 0.0f;
  m[0][3] = 0.0f;  m[1][3] = 0.0f;    m[2][3] = 0.0f;    m[3][3] = 1.0f;

}

///////////////////////////////////////////
// MatMakeYRot()
// a rotation matrix (around the y axis)
///////////////////////////////////////////
void CMatrix::MatMakeYRot(float radians )
{
  float c = (float)cos( radians );
  float s = (float)sin( radians );
	


  m[0][0] = c;     m[1][0] = 0.0f;    m[2][0] = -s;      m[3][0] = 0.0f;
  m[0][1] = 0.0f;  m[1][1] = 1.0f;    m[2][1] = 0.0f;    m[3][1] = 0.0f;
  m[0][2] = s;     m[1][2] = 0.0f;    m[2][2] = c;       m[3][2] = 0.0f;
  m[0][3] = 0.0f;  m[1][3] = 0.0f;    m[2][3] = 0.0f;    m[3][3] = 1.0f;
}

///////////////////////////////////////////
// MatMakeZRot()
// a rotation matrix (around the z axis)
///////////////////////////////////////////
void CMatrix::MatMakeZRot(float radians)
{
  float c = (float)cos( radians );
  float s = (float)sin( radians );

  
  m[0][0] = c;     m[1][0] = s;       m[2][0] = 0.0f;    m[3][0] = 0.0f;
  m[0][1] = -s;    m[1][1] = c;       m[2][1] = 0.0f;    m[3][1] = 0.0f;
  m[0][2] = 0.0f;  m[1][2] = 0.0f;    m[2][2] = 1.0f;    m[3][2] = 0.0f;
  m[0][3] = 0.0f;  m[1][3] = 0.0f;    m[2][3] = 0.0f;    m[3][3] = 1.0f;
}


///////////////////////////////////////////
// MatMakeZRot()
// a rotation matrix (around the x,y,z axis)
///////////////////////////////////////////
void CMatrix::MatMakeRot(float xrad,float yrad,float zrad)
{
  CMatrix x, y, z;
  x.MatMakeXRot (xrad);
  y.MatMakeYRot (yrad);
  z.MatMakeZRot (zrad);
  
  //(*this)= (y*x*z);

  (*this)= (x*y*z);

  //(*this)= (z*y*x);
  //(*this)= (y*z*x);
}



////////////////////////////////
  // MatImportFrom3x3()
  // Import from a 3x3 matrix
  ///////////////////////////////
void CMatrix::MatImportFrom3x3(float* pInput)
{
/*
	m[0][0] = pInput[0*3 + 0];
	m[0][1] = pInput[0*3 + 1];
	m[0][2] = pInput[0*3 + 2];
	m[0][3] = 0.f;

	m[1][0] = pInput[1*3 + 0];
	m[1][1] = pInput[1*3 + 1];
	m[1][2] = pInput[1*3 + 2];
	m[1][3] = 0.f;

	m[2][0] = pInput[2*3 + 0];
	m[2][1] = pInput[2*3 + 1];
	m[2][2] = pInput[2*3 + 2];
	m[2][3] = 0.f;
	
	// translation

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1.f;
*/
}









///////////////////////////////////////////////////////////////////
////////////********* Matrix Operators************/////////////////
///////////////////////////////////////////////////////////////////









///////////////////////////////////////////
// matrix = matrix
///////////////////////////////////////////
CMatrix CMatrix::operator = (const CMatrix m1)
{
  int i,j;
 
  for (i=0;i<4;i++)
    for (j=0;j<4;j++)
      m[i][j] = m1.m[i][j]; 
  return *this;
}

///////////////////////////////////////////
// matrix = pointer to matrix (faster)
///////////////////////////////////////////
CMatrix CMatrix::operator = (const CMatrix* m1)
{
	int i,j;

	if (m1==NULL)
	{
	   MatMakeUnit();	
	}
	else
	{	
		for (i=0;i<4;i++)
			for (j=0;j<4;j++)
				m[i][j] = (*m1).m[i][j]; 
	}
	return *this;
}



CVec3f GetVectorFromAngles(float fAngX,float fAngY)
{
	CMatrix t_RotateMatrix;
	CVec3f LookDirection;
	LookDirection.v[0]=LookDirection.v[1]=0.f;
	LookDirection.v[2]=-1;

	
	t_RotateMatrix.MatMakeRot(DEG2RAD(fAngX),DEG2RAD(0),DEG2RAD(0));
	LookDirection=t_RotateMatrix*LookDirection;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(fAngY),DEG2RAD(0)); 
	LookDirection=t_RotateMatrix*LookDirection;
	
	return LookDirection.Normalize();
}

CVec3f RotateVectorByAngle(float fAngX,float fAngY,const CVec3f& vec)
{
	CVec3f ans = vec;
	CMatrix t_RotateMatrix;
		
	t_RotateMatrix.MatMakeRot(DEG2RAD(fAngX),DEG2RAD(0),DEG2RAD(0));
	ans=t_RotateMatrix*ans;
	
	t_RotateMatrix.MatMakeRot(DEG2RAD(0),DEG2RAD(fAngY),DEG2RAD(0)); 
	ans=t_RotateMatrix*ans;
	
	return ans.Normalize();
}

void CPlane::FindPlane(const CVec3f& point, const CVec3f& normal)
{
	n = normal;
	n.Normalize();
	d = n*point;
}

void CPlane::FindPlane(const CVec3f& vec1, const CVec3f& vec2, const CVec3f& vec3)
{
	n = ((vec2-vec1)^(vec3-vec1));
	n.Normalize();

	assert(n.x < 100.0); // to find infi values

	d = n*vec1;
}


void CPlane::FillFromOtherPlaneAndMultiplyByMatrix(CPlane& from, CMatrix& m)
{
	CVector4f t_v;
	t_v.Set(from.n.v[0],from.n.v[1],from.n.v[2],from.d);
	t_v = m*t_v;
}

bool CPlane::IsPointOnPlane(CVec3f& pPoint)
{

	float	newD = Dot(pPoint,n) - d;  

	if (newD>EPSILON) return false; // infront

	if (newD<-EPSILON) return false; // behind

	return true; //on plane
	
}


bool CPlane::IsPointOnPlane(CVec3f& pPoint,float fWithinDistance)
{
	float	newD = Dot(pPoint,n) - d;  

	if (newD>0.1f+fWithinDistance) return false; // infront

	if (newD<-0.1f-fWithinDistance) return false; // behind

	return true; //on plane

}

void CPlane::Normalize(void)
{
	float magnitude = (float)sqrt( n.v[0] * n.v[0] + 
								   n.v[1] * n.v[1] + 
								   n.v[2] * n.v[2] );

	n.v[0] /= magnitude;
	n.v[1] /= magnitude;
	n.v[2] /= magnitude;
	d /= magnitude; 
}

/*inline int PointOnPlaneSide(const VECTOR3 &V,const PLANE &P) 
{  
	float	d = V*P.n - P.d;  
	if (d>PLANE_EPSILON) return FRONTSIDE;  
	if (d<-PLANE_EPSILON) return BACKSIDE;  
	return ON_PLANE; 
}*/



CVec3f RayPlaneIntersection(CVec3f& RayStart,CVec3f& RayEnd,CPlane &Plane)
{

	CVec3f RayDirection=RayEnd-RayStart;

	float numer = Dot(Plane.n,RayStart) - Plane.d;
	float denom = Dot(Plane.n,RayDirection);
	float t =  - (numer / denom);

	float a = RayEnd.v[0]-RayStart.v[0];
	float b = RayEnd.v[1]-RayStart.v[1];
	float c = RayEnd.v[2]-RayStart.v[2];

/*	float distance = sqrt( (a*a)+(b*b)+(c*c));

	if (distance<=t)*/
	//{
	return GetRayPoint(RayStart,RayDirection,t);
	//}
	/*else
		return NULL;*/
}







float RayPlaneIntersectionT(CVec3f& RayOrigin,CVec3f& RayDirection,CPlane& plane)
{
//	float d = (Dot(PlaneNormal,PlaneOrigin));

	float numer = Dot(plane.n,RayOrigin) + (plane.d );
	float denom = Dot(plane.n,RayDirection);
	float t =  - (numer / denom);
	return t;
}






bool PointInBBox(const CVec3f& point,const BBox& bbox)
{
	if (point.v[0]<bbox.min.v[0] || point.v[0]>bbox.max.v[0] ||
		point.v[1]<bbox.min.v[1] || point.v[1]>bbox.max.v[1] ||
		point.v[2]<bbox.min.v[2] || point.v[2]>bbox.max.v[2])
		return false;

	return true;

}

bool BBoxesIntersect(const CVec3f&min1,const CVec3f&max1,const CVec3f&min2,const CVec3f&max2)
{
	if (max1.v[0]<min2.v[0]||min1.v[0]>max2.v[0]) return false;
	if (max1.v[1]<min2.v[1]||min1.v[1]>max2.v[1]) return false;
	if (max1.v[2]<min2.v[2]||min1.v[2]>max2.v[2]) return false;

	return true;


/*inline bool BoundingBoxIntersect(const BBOX &bbox1,const BBOX &bbox2)
{
	if (bbox1.max.x<bbox2.min.x||bbox1.min.x>bbox2.max.x) return false;
	if (bbox1.max.y<bbox2.min.y||bbox1.min.y>bbox2.max.y) return false;
	if (bbox1.max.z<bbox2.min.z||bbox1.min.z>bbox2.max.z) return false;

	return true;
}*/

}

bool BBoxesIntersect(const BBox& bb1, const BBox& bb2)
{
	return BBoxesIntersect(bb1.min,bb1.max,bb2.min,bb2.max);
}



// LEFT
// RIGHT
// TOP
// BOTTOM
// FRONT
// BACK

void MakeBrushFromBoundingBox(CVec3f& vMin, CVec3f& vMax,CPlane* pPlanes)
{
	//original

	//LEFT

	pPlanes[0].n.Set(-1.f,0.f,0.f);
	pPlanes[0].d = -vMin.v[0];

	//RIGHT

	pPlanes[1].n.Set(1.f,0.f,0.f);
	pPlanes[1].d = vMax.v[0];



	//TOP

	pPlanes[2].n.Set(0.f,1.f,0.f);
	pPlanes[2].d = vMax.v[1];

	//BOTTOM

	pPlanes[3].n.Set(0.f,-1.f,0.f);
	pPlanes[3].d = -vMin.v[1];


	//FRONT

	pPlanes[4].n.Set(0.f,0.f,-1.f);
	pPlanes[4].d = -vMin.v[2];

	//BACK

	pPlanes[5].n.Set(0.f,0.f,1.f);
	pPlanes[5].d = vMax.v[2];

}

CVec3f Normalize(CVec3f& vec)
{
	float t;
	t = 1.f / (float) sqrtf( vec.v[0]*vec.v[0]+vec.v[1]*vec.v[1]+vec.v[2]*vec.v[2]);
	
	CVec3f ans;

	ans.v[0] = vec.v[0]*t;
	ans.v[1] = vec.v[1]*t;
	ans.v[2] = vec.v[2]*t;

	return ans;
}

// if (pIndices==NULL) all vertices are checked
BBox ComputeBBox(CVec3f* pVertices, UINT* pIndices, int iIndicesNum)
{
	assert(pIndices);
	assert(pVertices);

	BBox bbox;
	bbox.min = CVec3f(999999.f,999999.f,999999.f);
	bbox.max = CVec3f(-999999.f,-999999.f,-999999.f);

	int iIndex;

	for (int i=0;i<iIndicesNum;i++)
	{
		iIndex = pIndices[i];

		if (pVertices[iIndex].x < bbox.min.x)
			bbox.min.x = pVertices[iIndex].x;

		if (pVertices[iIndex].y < bbox.min.y)
			bbox.min.y = pVertices[iIndex].y;

		if (pVertices[iIndex].z < bbox.min.z)
			bbox.min.z = pVertices[iIndex].z;

		if (pVertices[iIndex].x > bbox.max.x)
			bbox.max.x = pVertices[iIndex].x;

		if (pVertices[iIndex].y > bbox.max.y)
			bbox.max.y = pVertices[iIndex].y;

		if (pVertices[iIndex].z > bbox.max.z)
			bbox.max.z = pVertices[iIndex].z;

	}


	return bbox;
}

bool CheckBBoxSphereIntersection(const BBox& bbox, const CVec3f& vPos, float fRadius)
{
	float s, d = 0; 
	float* pV=(float*)&vPos;
	float* pMi=(float*)&bbox.min.v;
	float* pMa=(float*)&bbox.max.v;
	//find the square of the distance
	//from the sphere to the box
	for( long i=0 ; i<3 ; i++ ) 
	{ 
		if( pV[i] < pMi[i] )
		{
			s = pV[i] - pMi[i];
			d += s*s; 
		}

		else if( pV[i] > pMa[i] )
		{ 
			s = pV[i] - pMa[i];
			d += s*s; 
		}
	}

	return d <= fRadius*fRadius;

	return true;
}

BBox GetBBoxFromSphere(CVec3f& center, float fRadius)
{
	BBox bbox;

	bbox.min = center;
	bbox.max = center;
	
	bbox.min -= CVec3f(fRadius,fRadius,fRadius);
	bbox.max += CVec3f(fRadius,fRadius,fRadius);

	return bbox;
}

void GetSphereFromBBox(const BBox& bbox, CVec3f& vCenter, float& fRadius)
{
	vCenter = (bbox.min + bbox.max)*0.5f;

	float fDist1 = (bbox.min - vCenter).Length();
	float fDist2 = (bbox.max - vCenter).Length();
	

	fRadius = max(fDist1,fDist2);

}

//FIXME: there's a problem with high value sweeps (sweeps to a long distance!!!)
// values higher then 100 units seem to fail!

bool AABBSweep(const CVec3f& Ea, //extents of AABB A
					  const CVec3f& A0, //its previous position
					  const CVec3f& A1, //its current position
					  const CVec3f& Eb, //extents of AABB B
					  const CVec3f& B0, //its previous position
           			  const CVec3f& B1, //its current position
					  float& u0,
					  CVec3f& normal)
{
		normal.Set(0.f,0.f,0.f);

		BBox A;
		A.max.Set(A0+Ea);//previous state of AABB A
		A.min.Set(A0-Ea);
		//g_pRenderer->DBG_RenderBox(A,CVec3(0,1,0));


		BBox B;
		B.max.Set(B0+Eb);//previous state of AABB B
		B.min.Set(B0-Eb);

		u0=0;

		

		if (BBoxesIntersect(A,B))
		{
			// Initial overlap - get axis of least overlap
			CVec3f bcenter = (B.min + B.max)*0.5f;
			CVec3f acenter = (A.min + A.max)*0.5f;

			// debug - temp try
			if (acenter==bcenter)
				return false;

			normal=Normalize(bcenter-acenter);
			BREAKPOINT(   *((unsigned long*)&normal.x) == 0xffc00000 );
						
			return true;
		}

		u0=1.0f;
		CVec3f origmin=B.min;
		CVec3f origmax=B.max;

		CVec3f vel=B1-B0;
		B.max.Set(origmax+vel*u0);
		B.min.Set(origmin+vel*u0);
		if (!BBoxesIntersect(A,B))
		{
			return false;
		}

		float t0=1.0f;
		float t1=0.0f;

		while (t0-t1>0.005)
		{
			float cur=(t0+t1)*0.5f;
			B.max.Set(origmax+vel*cur);
			B.min.Set(origmin+vel*cur);
			if (!BBoxesIntersect(A,B))
				t1=cur;
			else
				t0=cur;
		}


		B.max.Set(origmax+vel*t1);
		B.min.Set(origmin+vel*t1);
		CVec3f bcenter= (B.min + B.max)*0.5f;
		CVec3f acenter= (A.min + A.max)*0.5f;


		// debug - temp try
		if (acenter==bcenter)
			return false;

		int axis=0;
		for (int i=0;i<3;i++)
		{
			if ((fabs(bcenter.v[i]-acenter.v[i])-Ea.v[i]-Eb.v[i])>0)
			{
				axis=i;
				break;
			}
		}

		int casee=0;
		if (bcenter.v[axis]>acenter.v[axis])
			casee=0;
		else
			casee=1;


		u0=t1;
		normal.Set(int(axis==0),int(axis==1),int(axis==2));
		BREAKPOINT(   *((unsigned long*)&normal.x) == 0xffc00000 );
		if (casee==1) 
			normal*=-1.f;

		//g_pRenderer->DBG_printf(400,400,"%1.2f %1.2f %1.2f",normal[0],normal[1],normal[2]);

		return true;

}