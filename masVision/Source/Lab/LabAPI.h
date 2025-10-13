#pragma once


struct LabAPI
{
	virtual bool Init()      = 0;
	virtual void Tick()      = 0;
	virtual void Present()   = 0;
	virtual void Terminate() = 0;
};