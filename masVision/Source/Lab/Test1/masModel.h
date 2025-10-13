#pragma once



struct masModel;

masModel* masModel_Load(const char* Path);
void masModel_UnLoad(masModel** Model);