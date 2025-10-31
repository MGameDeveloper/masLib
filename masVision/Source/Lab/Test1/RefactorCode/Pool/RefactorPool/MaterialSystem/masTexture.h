#pragma once

#include "../masHandle.h"


MAS_DECLARE_HANDLE(masTexture);

masTexture  masTexture_Load             (const char* Name);
void        masTexture_UnLoad           (masTexture* Texture);
void*       masTexture_GetGPUTexture    (masTexture  Texture);
void*       masTexture_GetGPUTextureView(masTexture  Texture);
const char* masTexture_GetName          (masTexture  Texture);
int32_t     masTexture_GetWidth         (masTexture  Texture);
int32_t     masTexture_GetHeight        (masTexture  Texture);
int8_t      masTexture_GetChannels      (masTexture  Texture);
float       masTExture_GetAspectRatio   (masTexture  Texture);