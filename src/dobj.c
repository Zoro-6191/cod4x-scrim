#include "dobj.h"
// #include "xassets/xmodel.h"
// #include "cscr_stringlist.h"

void __cdecl ConvertQuatToMat(DObjAnimMat *mat, vec3_t axis[3])
{
	vec3_t scaledQuat;
	float xx;
	float xy;
	float xz;
	float xw;
	float yy;
	float yz;
	float yw;
	float zz;
	float zw;

	assert(!IS_NAN((mat->quat)[0]) && !IS_NAN((mat->quat)[1]) && !IS_NAN((mat->quat)[2]) && !IS_NAN((mat->quat)[3]));
	assert(!IS_NAN(mat->transWeight));

	VectorScale(mat->quat, mat->transWeight, scaledQuat);
	xx = scaledQuat[0] * mat->quat[0];
	xy = scaledQuat[0] * mat->quat[1];
	xz = scaledQuat[0] * mat->quat[2];
	xw = scaledQuat[0] * mat->quat[3];
	yy = scaledQuat[1] * mat->quat[1];
	yz = scaledQuat[1] * mat->quat[2];
	yw = scaledQuat[1] * mat->quat[3];
	zz = scaledQuat[2] * mat->quat[2];
	zw = scaledQuat[2] * mat->quat[3];
	axis[0][0] = 1.0 - yy + zz;
	axis[0][1] = xy + zw;
	axis[0][2] = xz - yw;
	axis[1][0] = xy - zw;
	axis[1][1] = 1.0 - xx + zz;
	axis[1][2] = yz + xw;
	axis[2][0] = xz + yw;
	axis[2][1] = yz - xw;
	axis[2][2] = 1.0 - xx + yy;
}

