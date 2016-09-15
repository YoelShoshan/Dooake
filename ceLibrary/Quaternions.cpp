#include "stdafx.h"
#include "Quaternions.h"
#include "Math.h"

CQuat::CQuat()
{
};
CQuat::~CQuat()
{
};

CQuat::CQuat(const CQuat& quat)
{
	w = quat.w;
	x = quat.x;
	y = quat.y;
	z = quat.z;
}

void CQuat::ComputeW(void)
{
	w = - sqrt(fabs( 1.0f - ( (x*x) + (y*y) + 	(z*z) ) ));
}

void CQuat::operator = (const CQuat& quat)
{
	w = quat.w;
	x = quat.x;
	y = quat.y;
	z = quat.z;
}

void CQuat::Set(const float* fpMat)
{
	float  tr, s, q[4];
  int    i, j, k;


  int nxt[3] = {1, 2, 0};


  tr = fpMat[0*4+0] + fpMat[1*4+1] + fpMat[2*4+2];


  // check the diagonal
  if (tr > 0.0) {
    s = sqrt (tr + 1.0);
    w = s / 2.0;
    s = 0.5 / s;
    x = (fpMat[1*4+2] - fpMat[2*4+1]) * s;
    y = (fpMat[2*4+0] - fpMat[0*4+2]) * s;
    z = (fpMat[0*4+1] - fpMat[1*4+0]) * s;
} else {		
	 // diagonal is negative
    	  i = 0;
          if (fpMat[1*4+1] > fpMat[0*4+0]) i = 1;
	     if (fpMat[2*4+2] > fpMat[i*4+i]) i = 2;
            j = nxt[i];
            k = nxt[j];


            s = sqrt ((fpMat[i*4+i] - (fpMat[j*4+j] + fpMat[k*4+k])) + 1.0);
      
	     q[i] = s * 0.5;
            
            if (s != 0.0) s = 0.5 / s;


	    q[3] = (fpMat[j*4+k] - fpMat[k*4+j]) * s;
            q[j] = (fpMat[i*4+j] + fpMat[j*4+i]) * s;
            q[k] = (fpMat[i*4+k] + fpMat[k*4+i]) * s;


	  x = q[0];
	  y = q[1];
	  z = q[2];
	  w = q[3];
  }

}
void CQuat::Set(float roll, float pitch, float yaw)
{
	float cr, cp, cy, sr, sp, sy, cpcy, spsy;

	// calculate trig identities
	cr = cos(roll/2);

	cp = cos(pitch/2);
	cy = cos(yaw/2);


	sr = sin(roll/2);
	sp = sin(pitch/2);
	sy = sin(yaw/2);
	
	cpcy = cp * cy;
	spsy = sp * sy;

	w = cr * cpcy + sr * spsy;
	x = sr * cpcy - cr * spsy;
	y = cr * sp * cy + sr * cp * sy;
	z = cr * cp * sy - sr * sp * cy;

}
void CQuat::Set(float setW, float setX, float setY, float setZ)
{
	w = setW;
	x = setX;
	y = setY;
	z = setZ;
 }

CQuat::CQuat(float setW, float setX, float setY, float setZ)
{
	Set(setW,setX,setY,setZ);	
};

CQuat::CQuat(float roll, float pitch, float yaw)
{
	Set(roll,pitch,yaw);	
}


CQuat::CQuat(const float* fpMat)
{
	Set(fpMat);
 };

void CQuat::GetMatrix(CMatrix& mat) const
{
	 float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    // calculate coefficients
    x2 = x + x; y2 = y + y;
    z2 = z + z;
    xx = x * x2; xy = x * y2; xz = x * z2;
    yy = y * y2; yz = y * z2; zz = z * z2;
    wx = w * x2; wy = w * y2; wz = w * z2;


	mat.m[0][0] = 1.0 - (yy + zz); mat.m[0][1] = xy - wz;
    mat.m[0][2] = xz + wy; mat.m[0][3] = 0.0;

    mat.m[1][0] = xy + wz; mat.m[1][1] = 1.0 - (xx + zz);
    mat.m[1][2] = yz - wx; mat.m[1][3] = 0.0;


    mat.m[2][0] = xz - wy; mat.m[2][1] = yz + wx;
    mat.m[2][2] = 1.0 - (xx + yy); mat.m[2][3] = 0.0;


    mat.m[3][0] = 0; mat.m[3][1] = 0;
    mat.m[3][2] = 0; mat.m[3][3] = 1;


}

// assumes allocated!
void CQuat::ReturnMatrix(float* pfMatrix) const
{
    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    // calculate coefficients
    x2 = x + x; y2 = y + y;
    z2 = z + z;
    xx = x * x2; xy = x * y2; xz = x * z2;
    yy = y * y2; yz = y * z2; zz = z * z2;
    wx = w * x2; wy = w * y2; wz = w * z2;


    pfMatrix[0*4+0] = 1.0 - (yy + zz); pfMatrix[1*4+0] = xy - wz;
    pfMatrix[2*4+0] = xz + wy; pfMatrix[3*4+0] = 0.0;

    pfMatrix[0*4+1] = xy + wz; pfMatrix[1*4+1] = 1.0 - (xx + zz);
    pfMatrix[2*4+1] = yz - wx; pfMatrix[3*4+1] = 0.0;


    pfMatrix[0*4+2] = xz - wy; pfMatrix[1*4+2] = yz + wx;
    pfMatrix[2*4+2] = 1.0 - (xx + yy); pfMatrix[3*4+2] = 0.0;


    pfMatrix[0*4+3] = 0; pfMatrix[1*4+3] = 0;
    pfMatrix[2*4+3] = 0; pfMatrix[3*4+3] = 1;
	
};
void CQuat::operator += (CQuat& quat)
{
	w+=quat.w;
	x+=quat.x;
	y+=quat.y;
	z+=quat.z;

}

CQuat CQuat::operator * (float fT) const
{
	return CQuat(w*fT,x*fT,y*fT,z*fT);

}

/*where xyzw = quat, and abc= vector: 
[2·x·(b·y + c·z) - 2·a·y^2 - 2·c·w·y - 2·a·z^2 + 2·b·w·z + a, - 2·b·x^2 + 2·x·(a·y + c·w) + 2·c·y·z - 2·b·z^2 - 2·a·w·z + b, - 2·c·x^2 + 2·x·(a·z - b·w) - 2·c·y^2 + 2·y·(b·z + a·w) + c]*/

CQuat CQuat::operator * (CQuat& quat) const
{
	float A, B, C, D, E, F, G, H;


A = (quat.w + quat.x)*(w + x);
B = (quat.z - quat.y)*(y - z);
C = (quat.w - quat.x)*(y + z); 
D = (quat.y + quat.z)*(w - x);
E = (quat.x + quat.z)*(x + y);
F = (quat.x - quat.z)*(x - y);
G = (quat.w + quat.y)*(w - z);
H = (quat.w - quat.y)*(w + z);

	return CQuat(B + (-E - F + G + H) /2,
				A - (E + F + G + H)/2,
				C + (E - F + G - H)/2,
				D + (E - F - G + H)/2);
};

CQuat CQuat::Inverse(void) const
{
	return CQuat(-w,-x,-y,-z);

}

// this functions returns a quaternion that is the slerp interpolation between
	// self and other
	// fT is in the range [0,1]
CQuat CQuat::Slerp(const CQuat& other,float fT) const
{
	 float           to1[4];
     double        omega, cosom, sinom, scale0, scale1;


      // calc cosine
     cosom = x * other.x + y * other.y + z * other.z	+ w * other.w;


        // adjust signs (if necessary)
        if ( cosom <0.0 ){ cosom = -cosom; to1[0] = - other.x;
		to1[1] = - other.y;
		to1[2] = - other.z;
		to1[3] = - other.w;
        } else  {
		to1[0] = other.x;
		to1[1] = other.y;
		to1[2] = other.z;
		to1[3] = other.w;
        }


        // calculate coefficients


       if ( (1.0 - cosom) > QUAT_SLERP_DELTA ) {
                // standard case (slerp)
                omega = acos(cosom);
                sinom = sin(omega);
                scale0 = sin((1.0 - fT) * omega) / sinom;
                scale1 = sin(fT * omega) / sinom;


        } else {        
    // "from" and "to" quaternions are very close 
	    //  ... so we can do a linear interpolation
                scale0 = 1.0 - fT;
                scale1 = fT;
        }
	// calculate final values
	   return CQuat(scale0 * w + scale1 * to1[3],
		   scale0 * x + scale1 * to1[0],
		   scale0 * y + scale1 * to1[1],
		   scale0 * z + scale1 * to1[2]);

}