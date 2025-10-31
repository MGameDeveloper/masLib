#pragma once

#include "../masHandle.h"

class masModel
{
private:
	masHandleNode Meshes;

public:
	masModel(const char* Path);
	~masModel();
	masModel(const masModel& Other);
	masModel(masModel&& Other);
	masModel& operator=(const masModel& Rhs);
	masModel& operator=(masModel&& Rhs);
};