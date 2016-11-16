#include "ofMain.h"

class Utils
{
public:
	static ofColor Debug1D(float measurement)
	{
		float base = 0.0f;
		float val = measurement * 255;

		if (measurement < 0.0f)
		{
			return ofColor(base + abs(val), base, base);
		}
		else if (measurement < 1.001f)
		{
			return ofColor(base + val);
		}
		else
		{
			return ofColor(0.0f, 255.0f, 255.0f);
		}
	}

	static ofColor Debug3D(const ofVec3f &measurement)
	{
		float base = 0.5f;
		return ofColor(base + measurement.x * 255,
					   base + measurement.y * 255,
					   base + measurement.z * 255);
	}
};