#pragma once

#include "masMesh.h"


MAS_DECLARE_HANDLE(masModel);

masModel masModel_Load  (const char* Name);
void     masModel_UnLoad(masModel* Model);
uint32_t masModel_GetMeshCount();