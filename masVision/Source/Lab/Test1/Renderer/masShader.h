#pragma once

#include <string>

#include "Graphics/masGraphics.h"

class masShader
{
private:
	ComPtr<ID3D11ShaderReflection> pReflection;
	ComPtr<ID3D11VertexShader>     pVS;
	ComPtr<ID3D11PixelShader>      pPS;
	ComPtr<ID3D11ComputeShader>    pCS;
	ComPtr<ID3D11DomainShader>     pDS;
	ComPtr<ID3D11HullShader>       pHS;
	ComPtr<ID3D11GeometryShader>   pGS;
	ComPtr<ID3D11InputLayout>      pInputLayout; // should be in a resource pool to prevent duplication if 2 shaders expect the same input but do different job

private:
	ComPtr<ID3DBlob> LoadShader(const std::string& Path, const std::string& EntryPoint, const std::string& ShaderModel)
	{
		std::wstring WPath(Path.begin(), Path.end());

		ComPtr<ID3DBlob> pCodeByte = nullptr;
		ComPtr<ID3DBlob> pError    = nullptr;
		HRESULT          hr        = D3DCompileFromFile(WPath.c_str(), nullptr, nullptr, EntryPoint.c_str(), ShaderModel.c_str(), 0, 0, &pCodeByte, &pError);
		if(FAILED(hr))
			MAS_ASSERT(SUCCEEDED(hr), "HRESULT[ %u ]: %s   PATH[ %s ] ENTRY_POIN[ %s ]", hr, pError->GetBufferPointer(), Path.c_str(), EntryPoint.c_str());

		return pCodeByte;
	}

	/*
	* From Copilot ;)
	*/
	DXGI_FORMAT GetFormatFromSignature(const D3D11_SIGNATURE_PARAMETER_DESC& desc) {
		int componentCount = 0;
		switch (desc.Mask) {
		case 0x1: componentCount = 1; break;
		case 0x3: componentCount = 2; break;
		case 0x7: componentCount = 3; break;
		case 0xF: componentCount = 4; break;
		default: return DXGI_FORMAT_UNKNOWN;
		}

		switch (desc.ComponentType) {
		case D3D_REGISTER_COMPONENT_UINT32:
			switch (componentCount) {
			case 1: return DXGI_FORMAT_R32_UINT;
			case 2: return DXGI_FORMAT_R32G32_UINT;
			case 3: return DXGI_FORMAT_R32G32B32_UINT;
			case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
			}
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			switch (componentCount) {
			case 1: return DXGI_FORMAT_R32_SINT;
			case 2: return DXGI_FORMAT_R32G32_SINT;
			case 3: return DXGI_FORMAT_R32G32B32_SINT;
			case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
			}
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			switch (componentCount) {
			case 1: return DXGI_FORMAT_R32_FLOAT;
			case 2: return DXGI_FORMAT_R32G32_FLOAT;
			case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
			case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			break;
		}

		return DXGI_FORMAT_UNKNOWN;
	}


public:
	masShader() 
	{ 
		memset(this, 0, sizeof(masShader)); 
	}

	~masShader()
	{
		if (pReflection) pReflection->Release();
		if (pVS)         pVS->Release();
		if (pPS)         pPS->Release();
		if (pCS)         pCS->Release();
		if (pDS)         pDS->Release();
		if (pHS)         pHS->Release();
		if (pGS)         pGS->Release();
		if (pInputLayout) pInputLayout->Release();
	}

	bool LoadVertexShader(const std::string& Path, const std::string& EntryPoint)
	{
		MAS_ASSERT(pInputLayout.Get() == nullptr, "SHDER_OBJECT_IS_NOT_EMPTY");

		ComPtr<ID3DBlob> pCodeByte = LoadShader(Path, EntryPoint, "vs_5_0");
		HRESULT hr = D3DReflect(pCodeByte->GetBufferPointer(), pCodeByte->GetBufferSize(), IID_PPV_ARGS(&pReflection));
		MAS_ASSERT(SUCCEEDED(hr), "HRESULT_D3D11[ %u ]: QUERY SHADER INFO TO BUILD INPUT LAYOUT", hr);

		D3D11_SHADER_DESC ShaderDesc = { };
		hr = pReflection->GetDesc(&ShaderDesc);
		MAS_ASSERT(SUCCEEDED(hr), "HRESULT_D3D11[ %u ]: QUERY SHDER DESC", hr);

		std::vector<D3D11_INPUT_ELEMENT_DESC> LayoutDesc;
		for (uint32_t i = 0; i < ShaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC ParamDesc = { };

			hr = pReflection->GetInputParameterDesc(i, &ParamDesc);
			MAS_ASSERT(SUCCEEDED(hr), "HRESULT_D3D11[ %u ]: QUERY SHDER INPUT PARAMETERS DESC", hr);

			D3D11_INPUT_ELEMENT_DESC ElementDesc = { };
			ElementDesc.SemanticName         = ParamDesc.SemanticName;
			ElementDesc.SemanticIndex        = ParamDesc.SemanticIndex;
			ElementDesc.Format               = GetFormatFromSignature(ParamDesc); // TODO: EXTRACT FORMAT FROM PARAM DESC ABOVE
			ElementDesc.InputSlot            = 0;
			ElementDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
			ElementDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
			ElementDesc.InstanceDataStepRate = 0;

			LayoutDesc.push_back(ElementDesc);
		}

		hr = masGraphics_D3D11()->Device->CreateInputLayout(LayoutDesc.data(), LayoutDesc.size(), pCodeByte->GetBufferPointer(), pCodeByte->GetBufferSize(), &pInputLayout);
		MAS_ASSERT(SUCCEEDED(hr), "HRESULT[ %u ]: CREATE_INPUT_LAYOUT_FROM_SHADER_REFLECTION\n Path: %ls\n EntryPoint: %s", hr, Path.c_str(), EntryPoint.c_str());

		hr = masGraphics_D3D11()->Device->CreateVertexShader(pCodeByte->GetBufferPointer(), pCodeByte->GetBufferSize(), nullptr, &pVS);
		MAS_ASSERT(SUCCEEDED(hr), "HRESULT[ %u ]: CREATE_VERTEX_SHADER", hr);

		return true;
	}

	bool LoadPixelShader(const std::string& Path, const std::string& EntryPoint)
	{
		ComPtr<ID3DBlob> pCodeByte = LoadShader(Path, EntryPoint, "ps_5_0");

		HRESULT hr = masGraphics_D3D11()->Device->CreatePixelShader(pCodeByte->GetBufferPointer(), pCodeByte->GetBufferSize(), nullptr, &pPS);
		MAS_ASSERT(SUCCEEDED(hr), "HRESULT[ %u ]: CREATE_PIXEL_SHADER\n Path: %ls\n EntryPoint: %s", hr, Path.c_str(), EntryPoint.c_str());

		return true;
	}
};