#include "GP_CommonType.h"

class GP_GeoItem
{
public:
	virtual int GetGeoType() = 0;
	virtual GP_RECT GetMBR() = 0;
	virtual int GetPointCount() = 0;
	virtual GP_Point GetPiont() = 0;
}