#ifndef _GP_BASETOOLS_H_
#define _GP_BASETOOLS_H_

class GP_BaseTools{
public:
	GP_BaseTools();
	~GP_BaseTools();

	static GP_BaseTools* Instance();

public:
	double gp_sin(int iAng);
	double gp_cos(int iAng);
	int gp_rotate_one_point(int iCenterX,int iCenterY,int x1,int y1,int &x2,int &y2,int iAng);
	int gp_rotate_points(int iCenterX,int iCenterY,int* ptIn,int iCount,int iAng);

private:
	int format_ang(int iAng);

private:
	int m_iSIN[91];
	double m_dSIN[91];
};

GP_BaseTools* GP_GetBaseTools();

#endif