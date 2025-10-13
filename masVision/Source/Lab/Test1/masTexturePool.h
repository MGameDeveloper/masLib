#pragma once

struct masTexture;

masTexture* masTexture_Load(const char* Path);
void masTexture_UnLoad(masTexture** Texture);