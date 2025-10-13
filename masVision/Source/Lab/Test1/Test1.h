#pragma once

#include "../LabAPI.h"

struct Test1 : LabAPI
{
	bool Init()      override;
	void Tick()      override;
	void Present()   override;
	void Terminate() override;
};