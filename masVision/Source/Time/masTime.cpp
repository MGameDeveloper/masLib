#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>


/************************************************************************************
*
*************************************************************************************/
static double Frequency   = 0.0;
static double ElapsedTime = 0.0;
static double AppTime     = 0.0;
static double BeginFrame  = 0.0;


/************************************************************************************
*
*************************************************************************************/
static double masTimeInternal_Now()
{
	LARGE_INTEGER LargeInt = { 0 };
	QueryPerformanceCounter(&LargeInt);

	double TimeStamp = (double)LargeInt.QuadPart * Frequency;
	return TimeStamp;
}


/************************************************************************************
*
*************************************************************************************/
void masTime_Init()
{
	LARGE_INTEGER LargeInt = { 0 };
	QueryPerformanceFrequency(&LargeInt);
	Frequency = 1.0 / (double)LargeInt.QuadPart;
	AppTime   = masTimeInternal_Now();
}

void masTime_Tick()
{
	double TimeStamp = masTimeInternal_Now();

	if (ElapsedTime == 0.0)
		ElapsedTime = TimeStamp - AppTime;
	else
		ElapsedTime = TimeStamp - BeginFrame;

	BeginFrame = TimeStamp;
}

double masTime_Now()
{
	double TimeStamp = masTimeInternal_Now();
	double Time = TimeStamp - AppTime;
	return Time;
}

double masTime_Elapsed()
{
	return ElapsedTime;
}