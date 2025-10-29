#pragma once

#include "../../Resources/masTextureDefs.h"
#include "masTexture.h"
#include "masTextureSampler.h"
#include "masMaterialScalars.h"
#include "masShader.h"


class masMaterial
{
private:
	masTexture         Textures[MAS_TEXTURE_COUNT];
	masTextureSampler  Samplers[MAS_TEXTURE_COUNT];
	masMaterialScalars Sacalars;
	masShader          Shader;

public:
	masMaterial(const char* Name);
	~masMaterial();
	masMaterial(const masMaterial& Other)          = delete;
	masMaterial(masMaterial&& Other)               = delete;
	masMaterial& operator=(const masMaterial& Rhs) = delete;
	masMaterial& operator=(masMaterial&& Rhs)      = delete;

	void SetShader(const masShader& Shader);

	void SetTexture(int32_t TextureID, const masTexture& Texture);
	void UnSetTexture(int32_t TextureID);
	void SetTextureSampler(int32_t TextureID, const masTextureSampler& Sampler);

	const masTexture&         GetTexture(int32_t TextureID);
	const masTextureSampler&  GetTextureSampler(int32_t TextureID);
	const masMaterialScalars& GetMaterialScalars();
	const masShader&          GetShader();
};