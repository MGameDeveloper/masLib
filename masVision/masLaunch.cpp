#include "masVision.h"


int32_t main(int32_t argc, const char** argv)
{
	if (!masVision_Init())
		return -1;

	return masVision_Launch();
}