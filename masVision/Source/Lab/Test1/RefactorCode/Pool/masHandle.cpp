#include "masHandle.h"


masHandle::masHandle() :
	Sig(0)
{

};

bool masHandle::operator==(const masHandle& Rhs)
{
	return (Sig == Rhs.Sig);
}