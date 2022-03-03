#include "stdafx.h"
#include "MeshFilter.h"
XMFLOAT4 MeshFilter::m_DefaultColor = XMFLOAT4(1,0,0,1);
XMFLOAT4 MeshFilter::m_DefaultFloat4 = XMFLOAT4(0, 0, 0, 0);
XMFLOAT3 MeshFilter::m_DefaultFloat3 = XMFLOAT3(0, 0, 0);
XMFLOAT2 MeshFilter::m_DefaultFloat2 = XMFLOAT2(0, 0);

MeshFilter::~MeshFilter()
{
	m_Positions.clear();
	m_Normals.clear();
	m_TexCoords.clear();
	m_Colors.clear();
	m_Indices.clear();
	m_Tangents.clear();
	m_Binormals.clear();

	for(VertexBufferData& data : m_VertexBuffers)
	{
		data.Destroy();
	}

	m_VertexBuffers.clear();
	SafeRelease(m_pIndexBuffer);
}

void MeshFilter::BuildIndexBuffer(const SceneContext& sceneContext)
{
	if (m_pIndexBuffer != nullptr)
		return;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DWORD) * UINT(m_Indices.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = m_Indices.data();

	HANDLE_ERROR(sceneContext.d3dContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer))
}

int MeshFilter::GetVertexBufferId(UINT inputLayoutId) const
{
	for (UINT i = 0; i<m_VertexBuffers.size(); ++i)
	{
		if (m_VertexBuffers[i].InputLayoutID == inputLayoutId)
			return i;
	}

	return -1;
}

void MeshFilter::BuildVertexBuffer(const SceneContext& sceneContext, BaseMaterial* pMaterial)
{
	auto& techniqueContext = pMaterial->GetTechniqueContext();

	//Check if VertexBufferInfo already exists with requested InputLayout
	if (GetVertexBufferId(techniqueContext.inputLayoutID) >= 0)
		return;

	VertexBufferData data;
	data.VertexStride = techniqueContext.inputLayoutSize;
	data.VertexCount = m_VertexCount;
	data.BufferSize = data.VertexStride * m_VertexCount;
	data.IndexCount = m_IndexCount;

	void *pDataLocation = malloc(data.BufferSize);
	if (pDataLocation == nullptr)
	{
		Logger::LogWarning(L"Failed to allocate the required memory!");
		return;
	}

	data.pDataStart = pDataLocation;
	data.InputLayoutID = techniqueContext.inputLayoutID;

	for (UINT i = 0; i < m_VertexCount; ++i)
	{
		for (UINT j = 0; j < techniqueContext.pInputLayoutDescriptions.size(); ++j)
		{
			const auto& ilDescription = techniqueContext.pInputLayoutDescriptions[j];

			if (i == 0 && !HasElement(ilDescription.SemanticType))
			{
			 std::wstring name = EffectHelper::GetIlSemanticName(ilDescription.SemanticType);
				Logger::LogWarning(L"Mesh \"{}\" has no vertex {} data, using a default value!", m_MeshName.c_str(), name.c_str());
			}

			switch (ilDescription.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType)?&m_Positions[i]:&m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::NORMAL:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Normals[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::COLOR:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Colors[i] : &m_DefaultColor, ilDescription.Offset);
				break;
			case ILSemantic::TEXCOORD:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_TexCoords[i] : &m_DefaultFloat2, ilDescription.Offset);
				break;
			case ILSemantic::TANGENT:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Tangents[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::BINORMAL:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_Binormals[i] : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::BLENDINDICES:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_BlendIndices[i] : &m_DefaultFloat4, ilDescription.Offset);
				break;
			case ILSemantic::BLENDWEIGHTS:
				memcpy(pDataLocation, HasElement(ilDescription.SemanticType) ? &m_BlendWeights[i] : &m_DefaultFloat4, ilDescription.Offset);
				break;
			default:
				HANDLE_ERROR(L"Unsupported SemanticType!");
				break;
			}

			pDataLocation = static_cast<char*>(pDataLocation) + ilDescription.Offset;
		}
	}

	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = data.BufferSize;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = data.pDataStart;

	//create a ID3D10Buffer in graphics memory containing the vertex info
	sceneContext.d3dContext.pDevice->CreateBuffer(&bd, &initData, &data.pVertexBuffer);

	m_VertexBuffers.push_back(data);
}

const VertexBufferData& MeshFilter::GetVertexBufferData(const SceneContext& sceneContext, BaseMaterial* pMaterial)
{
	auto& techniqueContext = pMaterial->GetTechniqueContext();
	const int possibleBuffer = GetVertexBufferId(techniqueContext.inputLayoutID);

	if (possibleBuffer < 0)
	{
		Logger::LogWarning(L"No VertexBufferInformation for this material found! Building matching VertexBufferInformation (Performance Issue).");
		BuildVertexBuffer(sceneContext, pMaterial);

		//Return last created vertexbufferinformation
		return m_VertexBuffers.back();
	}

	return m_VertexBuffers[possibleBuffer];
}
