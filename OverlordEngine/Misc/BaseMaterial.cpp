#include "stdafx.h"
#include "BaseMaterial.h"
BaseMaterial::~BaseMaterial()
{
	SafeRelease(m_pEffect);
}

void BaseMaterial::_baseInitialize(ID3DX11Effect* pRootEffect, UINT materialId)
{
	if (m_IsInitialized) return;

	m_MaterialId = materialId;
	pRootEffect->CloneEffect(0, &m_pEffect);

	//Retrieve Root variables
	m_pEVar_WORLD = m_pEffect->GetVariableBySemantic("world")->AsMatrix();
	if (!m_pEVar_WORLD->IsValid()) m_pEVar_WORLD = nullptr;

	m_pEVar_VIEW = m_pEffect->GetVariableBySemantic("view")->AsMatrix();
	if (!m_pEVar_VIEW->IsValid()) m_pEVar_VIEW = nullptr;

	m_pEVar_VIEWINVERSE = m_pEffect->GetVariableBySemantic("viewinverse")->AsMatrix();
	if (!m_pEVar_VIEWINVERSE->IsValid()) m_pEVar_VIEWINVERSE = nullptr;

	m_pEVar_WORLDVIEWPROJECTION = m_pEffect->GetVariableBySemantic("worldviewprojection")->AsMatrix();
	if (!m_pEVar_WORLDVIEWPROJECTION->IsValid()) m_pEVar_WORLDVIEWPROJECTION = nullptr;

	//Or to prevent warning but slower...
	//if (variableDesc.Semantic == nullptr) continue;

	//auto semanticStr = std::string(variableDesc.Semantic);
	//std::ranges::transform(semanticStr, semanticStr.begin(), ::tolower);

	//if (semanticStr == "world") m_pEVar_WORLD = pVariable->AsMatrix();
	//else if (semanticStr == "view") m_pEVar_VIEW = pVariable->AsMatrix();
	//else if (semanticStr == "viewinverse") m_pEVar_VIEWINVERSE = pVariable->AsMatrix();
	//else if (semanticStr == "worldviewprojection") m_pEVar_WORLDVIEWPROJECTION = pVariable->AsMatrix();

	m_IsInitialized = true;
}

void BaseMaterial::UpdateEffectVariables(const SceneContext& sceneContext, const ModelComponent* pModelComponent) const
{
	if (m_IsInitialized)
	{
		auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
		auto view = XMLoadFloat4x4(&sceneContext.pCamera->GetView());

		if (m_pEVar_WORLD)
			m_pEVar_WORLD->SetMatrix(reinterpret_cast<float*>(&world));

		if (m_pEVar_VIEW)
			m_pEVar_VIEW->SetMatrix(reinterpret_cast<float*>(&view));

		if (m_pEVar_WORLDVIEWPROJECTION)
		{
			const auto projection = XMLoadFloat4x4(&sceneContext.pCamera->GetProjection());
			auto wvp = world * view * projection;
			m_pEVar_WORLDVIEWPROJECTION->SetMatrix(reinterpret_cast<float*>(&wvp));
		}

		if (m_pEVar_VIEWINVERSE)
		{
			auto& viewInv = sceneContext.pCamera->GetViewInverse();
			m_pEVar_VIEWINVERSE->SetMatrix(&viewInv._11);
		}

		OnUpdateModelVariables(sceneContext, pModelComponent);
	}
}

ID3DX11EffectVariable* BaseMaterial::GetVariable(const std::wstring& varName) const
{
	auto& variableLUT = GetVariableIndexLUT();
	const auto variableHash = std::hash<std::wstring>{}(varName);

	if (!variableLUT.contains(variableHash)) return nullptr;
	return m_pEffect->GetVariableByIndex(variableLUT.at(variableHash));
}

void BaseMaterial::SetVariable_Scalar(const std::wstring& varName, float scalar) const
{
	if(const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsScalar()->SetFloat(scalar));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}

void BaseMaterial::SetVariable_Scalar(const std::wstring& varName, bool scalar) const
{
	if (const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsScalar()->SetBool(scalar));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}
void BaseMaterial::SetVariable_Scalar(const std::wstring& varName, int scalar) const
{
	if (const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsScalar()->SetInt(scalar));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}

void BaseMaterial::SetVariable_Matrix(const std::wstring& varName, XMFLOAT4X4 matrix) const
{
	SetVariable_Matrix(varName, &matrix._11);
}

void BaseMaterial::SetVariable_Matrix(const std::wstring& varName, const float* pData) const
{
	if (const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsMatrix()->SetMatrix(pData));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}

void BaseMaterial::SetVariable_MatrixArray(const std::wstring& varName, const float* pData, UINT count) const
{
	if (const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsMatrix()->SetMatrixArray(pData, 0, count));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}

void BaseMaterial::SetVariable_Vector(const std::wstring& varName, XMFLOAT3 vector) const
{
	SetVariable_Vector(varName, &vector.x);
}

void BaseMaterial::SetVariable_Vector(const std::wstring& varName, XMFLOAT4 vector) const
{
	SetVariable_Vector(varName, &vector.x);
}

void BaseMaterial::SetVariable_Vector(const std::wstring& varName, const float* pData) const
{
	if (const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsVector()->SetFloatVector(pData));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}

void BaseMaterial::SetVariable_VectorArray(const std::wstring& varName, const float* pData, UINT count) const
{
	if (const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsVector()->SetFloatVectorArray(pData, 0, count));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}

void BaseMaterial::SetVariable_Texture(const std::wstring& varName, TextureData* pTexture) const
{
	if (const auto pShaderVariable = GetVariable(varName))
	{
		HANDLE_ERROR(pShaderVariable->AsShaderResource()->SetResource(pTexture->GetShaderResourceView()));
		return;
	}

	Logger::LogWarning(L"Shader variable \'{}\' not found for \'{}\'", varName, GetEffectName());
}

void BaseMaterial::SetTechnique(const std::wstring& techName)
{
	auto& techniques = GetTechniques();
	const auto techNameHash = std::hash<std::wstring>{}(techName);

	if(techniques.contains(techNameHash))
	{
		m_TechniqueContext = techniques.at(techNameHash);
		m_TechniqueContext.pTechnique = m_pEffect->GetTechniqueByName(StringUtil::utf8_encode(techName).c_str());
		return;
	}

	Logger::LogWarning(L"Shader technique \'{}\' not found for \'{}\'", techName, GetEffectName());
}

void BaseMaterial::SetTechnique(int index)
{
	auto& techniques = GetTechniques();
	if(techniques.size() > index)
	{
		auto it = techniques.begin();
		std::advance(it, index);
		m_TechniqueContext = it->second;
		m_TechniqueContext.pTechnique = m_pEffect->GetTechniqueByIndex(index);
		return;
	}

	Logger::LogWarning(L"Shader technique with index \'{}\' not found for \'{}\'", index, GetEffectName());
}

void BaseMaterial::DrawImGui()
{
	ImGui::Dummy({ 0,5.f });
	const std::string title = std::format("Material > {} (ID#{})", m_MaterialNameUtf8.substr(6), m_MaterialId).c_str();
	if(ImGui::Button(title.c_str())){m_DrawImGui = true;}

	if (!m_DrawImGui) return;

	if (ImGui::Begin(title.c_str(), &m_DrawImGui))
	{
		D3DX11_EFFECT_DESC effectDesc{};
		m_pEffect->GetDesc(&effectDesc);

		for (UINT i{ 0 }; i < effectDesc.GlobalVariables; ++i)
		{
			const auto pVariable = m_pEffect->GetVariableByIndex(i);

			D3DX11_EFFECT_TYPE_DESC effectTypeDesc{};
			pVariable->GetType()->GetDesc(&effectTypeDesc);

			D3DX11_EFFECT_VARIABLE_DESC variableDesc{};
			pVariable->GetDesc(&variableDesc);

			bool isColor = std::string(variableDesc.Name).find("Color") != std::string::npos;

			switch (effectTypeDesc.Type)
			{
			case D3D_SVT_BOOL:
			{
				bool value{};
				pVariable->AsScalar()->GetBool(&value);
				if (ImGui::Checkbox(variableDesc.Name, &value))
				{
					pVariable->AsScalar()->SetBool(value);
				}
			}
			break;
			case D3D_SVT_INT:
			{
				int value{};
				pVariable->AsScalar()->GetInt(&value);
				if (ImGui::InputInt(variableDesc.Name, &value))
				{
					pVariable->AsScalar()->SetInt(value);
				}
			}
			break;
			case D3D_SVT_FLOAT:
			{
				if (effectTypeDesc.Class == D3D_SVC_SCALAR)
				{
					float value{};
					pVariable->AsScalar()->GetFloat(&value);
					if (ImGui::InputFloat(variableDesc.Name, &value))
						pVariable->AsScalar()->SetFloat(value);
				}
				else if (effectTypeDesc.Class == D3D_SVC_VECTOR)
				{
					float value[4]{};
					bool changed{};
					pVariable->AsVector()->GetFloatVector(&value[0]);
					switch (effectTypeDesc.Columns)
					{
					case 2: changed = ImGui::InputFloat2(variableDesc.Name, &value[0]); break;
					case 3: changed = isColor ? ImGui::ColorEdit3(variableDesc.Name, &value[0], ImGuiColorEditFlags_NoInputs) : ImGui::InputFloat3(variableDesc.Name, &value[0]); break;
					case 4: changed = isColor ? ImGui::ColorEdit4(variableDesc.Name, &value[0], ImGuiColorEditFlags_NoInputs) : ImGui::InputFloat4(variableDesc.Name, &value[0]); break;
					}

					if (changed) pVariable->AsVector()->SetFloatVector(&value[0]);
				}
			}
			break;
			case D3D_SVT_TEXTURE2D:
			case D3D_SVT_TEXTURECUBE:
			{
				char buffer[256]{};

				ID3D11ShaderResourceView* pSrv{};
				pVariable->AsShaderResource()->GetResource(&pSrv);

				if (pSrv)
				{
					TextureData* pTexData{};
					UINT size = sizeof(pTexData);
					pSrv->GetPrivateData(TextureData::GUID_TextureData, &size, &pTexData);
					if (pTexData)
					{
						auto pathUtf8 = StringUtil::utf8_encode(pTexData->GetAssetSubPath());
						pathUtf8.copy(&buffer[0], pathUtf8.size());
					}

					pSrv->Release();
				}

				if (ImGui::InputText(variableDesc.Name, &buffer[0], 256, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					auto filepath = StringUtil::utf8_decode(buffer);
					if (fs::exists(ContentManager::GetFullAssetPath(filepath)))
					{
						const auto pTextureData = ContentManager::Load<TextureData>(filepath);
						pVariable->AsShaderResource()->SetResource(pTextureData->GetShaderResourceView());
					}
				}
			}
			break;
			//case D3D_SVT_UINT: break;
			//case D3D_SVT_UINT8: break;
			//case D3D_SVT_DOUBLE: break;
			//default: ;
			}

		}
	}
	ImGui::End();
}
