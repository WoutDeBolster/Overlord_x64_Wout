#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_1::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	ColorMaterial* pColorMat{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };
	GameObject* pRoot{ new GameObject() };

	m_pBone0 = new BoneObject(pColorMat, 15.f);
	pRoot->AddChild(m_pBone0);
	m_pBone1 = new BoneObject(pColorMat, 15.f);
	m_pBone0->AddBone(m_pBone1);

	AddChild(pRoot);
}

void SoftwareSkinningScene_1::Update()
{
	if (m_ManualRotate == false)
	{
		m_BoneRotation0 += 45.f * m_SceneContext.pGameTime->GetElapsed() * m_RotationSign;
		if (m_BoneRotation0 >= 45.f)
		{
			m_RotationSign = -1;
		}
		if (m_BoneRotation0 <= -45.f)
		{
			m_RotationSign = 1;
		}

		m_BoneRotation1 = -m_BoneRotation0;
	}

	m_pBone0->GetTransform()->Rotate(0.f, 0.f, m_BoneRotation0);
	m_pBone1->GetTransform()->Rotate(0.f, 0.f, -m_BoneRotation1 * 2.f);
}

void SoftwareSkinningScene_1::OnGUI()
{
	ImGui::Checkbox("Rotate Manualy", &m_ManualRotate);
	ImGui::SliderFloat("Rotation Bone 0", &m_BoneRotation0, -45.f, 45.f);
	ImGui::SliderFloat("Rotation Bone 1", &m_BoneRotation1, -45.f, 45.f);
}
