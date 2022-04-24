#include "stdafx.h"
#include "MeshFilterLoader.h"

//#define MULTI_TEXCOORD
#define OVM_vMAJOR 1
#ifdef MULTI_TEXCOORD
#define OVM_vMINOR 61
#else
#define OVM_vMINOR 1
#endif


MeshFilter* MeshFilterLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
		return nullptr;

	//READ OVM FILE
	const int versionMajor = pReader->Read<char>();
	const int versionMinor = pReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogWarning(L"Wrong OVM version\n\tFile: \"{}\" \n\tExpected version {}.{}, not {}.{}.", loadInfo.assetFullPath.filename().wstring(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);

		delete pReader;
		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	const auto pMesh = new MeshFilter();

	for (;;)
	{
		const auto meshDataType = static_cast<MeshDataType>(pReader->Read<char>());
		if (meshDataType == MeshDataType::END)
			break;

		const auto dataOffset = pReader->Read<unsigned int>();

		switch (meshDataType)
		{
		case MeshDataType::HEADER:
		{
			pMesh->m_MeshName = pReader->ReadString();
			vertexCount = pReader->Read<unsigned int>();
			indexCount = pReader->Read<unsigned int>();

			pMesh->m_VertexCount = vertexCount;
			pMesh->m_IndexCount = indexCount;
		}
		break;
		case MeshDataType::POSITIONS:
		{
			pMesh->SetElement(ILSemantic::POSITION);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				XMFLOAT3 pos{};
				pos.x = pReader->Read<float>();
				pos.y = pReader->Read<float>();
				pos.z = pReader->Read<float>();
				pMesh->m_Positions.emplace_back(pos);
			}
		}
		break;
		case MeshDataType::INDICES:
		{
			for (unsigned int i = 0; i < indexCount; ++i)
			{
				pMesh->m_Indices.emplace_back(pReader->Read<DWORD>());
			}
		}
		break;
		case MeshDataType::NORMALS:
		{
			pMesh->SetElement(ILSemantic::NORMAL);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				XMFLOAT3 normal{};
				normal.x = pReader->Read<float>();
				normal.y = pReader->Read<float>();
				normal.z = pReader->Read<float>();
				pMesh->m_Normals.emplace_back(normal);
			}
		}
		break;
		case MeshDataType::TANGENTS:
		{
			pMesh->SetElement(ILSemantic::TANGENT);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				XMFLOAT3 tangent{};
				tangent.x = pReader->Read<float>();
				tangent.y = pReader->Read<float>();
				tangent.z = pReader->Read<float>();
				pMesh->m_Tangents.emplace_back(tangent);
			}
		}
		break;
		case MeshDataType::BINORMALS:
		{
			pMesh->SetElement(ILSemantic::BINORMAL);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				XMFLOAT3 binormal{};
				binormal.x = pReader->Read<float>();
				binormal.y = pReader->Read<float>();
				binormal.z = pReader->Read<float>();
				pMesh->m_Binormals.emplace_back(binormal);
			}
		}
		break;
		case MeshDataType::TEXCOORDS:
		{
			pMesh->SetElement(ILSemantic::TEXCOORD);

			constexpr auto amountTexCoords = 1;
#ifdef MULTI_TEXCOORD
			amountTexCoords = pReader->Read<USHORT>();
#endif
			pMesh->m_TexCoordCount = amountTexCoords;
			for (unsigned int i = 0; i < vertexCount * amountTexCoords; ++i)
			{
				XMFLOAT2 tc{};
				tc.x = pReader->Read<float>();
				tc.y = pReader->Read<float>();
				pMesh->m_TexCoords.emplace_back(tc);
			}
		}
		break;
		case MeshDataType::COLORS:
		{
			pMesh->SetElement(ILSemantic::COLOR);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				XMFLOAT4 color{};
				color.x = pReader->Read<float>();
				color.y = pReader->Read<float>();
				color.z = pReader->Read<float>();
				color.w = pReader->Read<float>();
				pMesh->m_Colors.emplace_back(color);
			}
		}
		break;
		case MeshDataType::BLENDINDICES:
		{
			//TODO_W7(L"Add BLENDINDICES information");
			pMesh->SetElement(ILSemantic::BLENDINDICES);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				XMFLOAT4 blendIndices{};
				blendIndices.x = pReader->Read<float>();
				blendIndices.y = pReader->Read<float>();
				blendIndices.z = pReader->Read<float>();
				blendIndices.w = pReader->Read<float>();
				pMesh->m_BlendIndices.emplace_back(blendIndices);
			}
		}
		break;
		case MeshDataType::BLENDWEIGHTS:
		{
			//TODO_W7(L"Add BLENDWEIGHTS information");
			pMesh->SetElement(ILSemantic::BLENDWEIGHTS);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				XMFLOAT4 blendWeights{};
				blendWeights.x = pReader->Read<float>();
				blendWeights.y = pReader->Read<float>();
				blendWeights.z = pReader->Read<float>();
				blendWeights.w = pReader->Read<float>();
				pMesh->m_BlendWeights.emplace_back(blendWeights);
			}
		}
		break;
		case MeshDataType::ANIMATIONCLIPS:
		{
			//TODO_W7(L"Add ANIMATIONCLIPS information");
			USHORT clipCount;
			clipCount = pReader->Read<USHORT>();
			if (clipCount > 0)
			{
				pMesh->m_HasAnimations = true;
			}

			for (USHORT clipI = 0; clipI < clipCount; ++clipI)
			{
				AnimationClip clip{};

				clip.name = pReader->ReadString();
				clip.duration = pReader->Read<float>();
				clip.ticksPerSecond = pReader->Read<float>();

				USHORT keyCount = pReader->Read<USHORT>();
				for (USHORT keyI = 0; keyI < keyCount; ++keyI)
				{
					AnimationKey key{};

					key.tick = pReader->Read<float>();
					USHORT boneTransformCount = pReader->Read<USHORT>();
					for (USHORT boneI = 0; boneI < boneTransformCount; ++boneI)
					{
						XMFLOAT4X4 transform{};
						transform._11 = pReader->Read<float>();
						transform._12 = pReader->Read<float>();
						transform._13 = pReader->Read<float>();
						transform._14 = pReader->Read<float>();
						transform._21 = pReader->Read<float>();
						transform._22 = pReader->Read<float>();
						transform._23 = pReader->Read<float>();
						transform._24 = pReader->Read<float>();
						transform._31 = pReader->Read<float>();
						transform._32 = pReader->Read<float>();
						transform._33 = pReader->Read<float>();
						transform._34 = pReader->Read<float>();
						transform._41 = pReader->Read<float>();
						transform._42 = pReader->Read<float>();
						transform._43 = pReader->Read<float>();
						transform._44 = pReader->Read<float>();

						key.boneTransforms.push_back(transform);
					}

					clip.keys.push_back(key);
				}

				pMesh->m_AnimationClips.push_back(clip);
			}
		}
		break;
		case MeshDataType::SKELETON:
		{
			//TODO_W7(L"Add SKELETON information");
			USHORT boneCount;
			boneCount = pReader->Read<USHORT>();
			pMesh->m_BoneCount = boneCount;
		}
		break;
		default:
			pReader->MoveBufferPosition(dataOffset);
			break;
		}

	}
	delete pReader;

	return pMesh;
}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}
