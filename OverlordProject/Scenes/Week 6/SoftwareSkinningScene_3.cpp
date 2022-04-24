#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_3::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	ColorMaterial* pColorMat{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };
	GameObject* pRoot{ new GameObject() };
	AddChild(pRoot);
	const float boneLenght{ 15.f };

	m_pBone0 = new BoneObject(pColorMat, boneLenght);
	pRoot->AddChild(m_pBone0);
	m_pBone1 = new BoneObject(pColorMat, boneLenght);
	m_pBone0->AddBone(m_pBone1);

	m_pBone0->CalculateBindPose();
	m_MeshDrawer = new MeshDrawComponent(24, true);
	GameObject* pBoxDrawer{ new GameObject() };
	AddChild(pBoxDrawer);
	pBoxDrawer->AddComponent<MeshDrawComponent>(m_MeshDrawer);

	InitializeVertices(boneLenght);
}

void SoftwareSkinningScene_3::Update()
{
	// rotation part
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

	// BoneTransformation
	auto boneTransfrom0{ XMMatrixMultiply(XMLoadFloat4x4(&m_pBone0->GetBindPose()), XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld())) };
	auto boneTransfrom1{ XMMatrixMultiply(XMLoadFloat4x4(&m_pBone1->GetBindPose()), XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld())) };

	for (size_t i = 0; i < m_SkinnedVertices.size(); i++)
	{

		XMVECTOR transfromedVertexBone0{ XMVector3TransformCoord(XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position), boneTransfrom0) };
		XMVECTOR transfromedVertexBone1{ XMVector3TransformCoord(XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position), boneTransfrom1) };
		XMStoreFloat3(&m_SkinnedVertices[i].transformedVertex.Position, transfromedVertexBone0 * m_SkinnedVertices[i].blendWeight0 + transfromedVertexBone1 * m_SkinnedVertices[i].blendWeight1);
	}

	// meshDrawer
	m_MeshDrawer->RemoveTriangles();
	for (size_t i = 0; i < m_SkinnedVertices.size(); i += 4)
	{
		QuadPosNormCol verticesQuad{ m_SkinnedVertices[i].transformedVertex,
									m_SkinnedVertices[i + 1].transformedVertex,
									m_SkinnedVertices[i + 2].transformedVertex,
									m_SkinnedVertices[i + 3].transformedVertex };
		m_MeshDrawer->AddQuad(verticesQuad);
	}
	m_MeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::OnGUI()
{
	ImGui::Checkbox("Rotate Manualy", &m_ManualRotate);
	ImGui::SliderFloat("Rotation Bone 0", &m_BoneRotation0, -45.f, 45.f);
	ImGui::SliderFloat("Rotation Bone 1", &m_BoneRotation1, -45.f, 45.f);
}

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
	float weightFull{ 1.f };
	float weightHalf{ 0.5f };
	float weightEmpty{ 0.f };
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightFull, weightEmpty);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightHalf, weightHalf);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, weightEmpty, weightFull);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, weightEmpty, weightFull);
#pragma endregion
}