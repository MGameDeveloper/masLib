#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


/********************************************************************************************
*
*********************************************************************************************/
typedef struct masTime_
{
    double Frequency;  
    double InitTime;
    double FrameStartTime;    
    double ElapsedTime;
} masTime;


/********************************************************************************************
*
*********************************************************************************************/
static masTime Time = {0};


/********************************************************************************************
*
*********************************************************************************************/
static double mas_internal_time_stamp()
{
    LARGE_INTEGER LargeInt = {0};
    QueryPerformanceCounter(&LargeInt);

    double TimeStamp = (double)LargeInt.QuadPart / Time.Frequency;
    return TimeStamp;
}


/********************************************************************************************
*
*********************************************************************************************/
void mas_impl_time_init()
{
    LARGE_INTEGER LargeInt = {0};
    QueryPerformanceFrequency(&LargeInt);
    Time.Frequency = 1.0 / (double)LargeInt.QuadPart;

    Time.InitTime = mas_internal_time_stamp() / Time.Frequency;
}

void mas_impl_time_calculate_elapsed()
{
    double TimeStamp = mas_internal_time_stamp();  
    if(Time.ElapsedTime == 0.0)
        Time.ElapsedTime = TimeStamp - Time.InitTime;
    else
        Time.ElapsedTime = TimeStamp - Time.FrameStartTime; 
    Time.FrameStartTime = TimeStamp;
}

double mas_impl_time_app()
{
    double TimeStamp   = mas_internal_time_stamp() / Time.Frequency;
    double RunningTime = TimeStamp - Time.InitTime;
    return RunningTime;
}

double mas_impl_time_elapsed()
{
    return Time.ElapsedTime;
}

double mas_impl_time_now()
{
    return mas_internal_time_stamp();
}