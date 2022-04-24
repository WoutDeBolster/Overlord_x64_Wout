#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter) :
	m_pMeshFilter{ pMeshFilter }
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	TODO_W7_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		float elapsedSec{ sceneContext.pGameTime->GetElapsed() };
		float durationTicks{ m_CurrentClip.duration };
		float ticksPerSec{ m_CurrentClip.ticksPerSecond };

		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		auto passedTicks = elapsedSec * ticksPerSec * m_AnimationSpeed;
		passedTicks = std::fmod(passedTicks, durationTicks);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
			{
				m_TickCount += durationTicks;
			}
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > durationTicks)
			{
				m_TickCount -= durationTicks;
			}
		}

		//3.
		//Find the enclosing keys
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		AnimationKey keyA, keyB;
		for (size_t i = 0; i < m_CurrentClip.keys.size() - 1; i++)
		{
			if (m_CurrentClip.keys[i].tick < m_TickCount && m_CurrentClip.keys[i + 1].tick > m_TickCount)
			{
				keyA = m_CurrentClip.keys[i];
				keyB = m_CurrentClip.keys[i + 1];
				break;
			}
		}


		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		//Clear the m_Transforms vector
		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
		float BlendFactor{ (keyB.tick - m_TickCount) / (durationTicks / m_CurrentClip.keys.size()) };
		m_Transforms.clear();
		m_Transforms.resize(keyA.boneTransforms.size());

		for (size_t i = 0; i < m_Transforms.size(); i++)
		{
			auto transformA = keyA.boneTransforms[i];
			auto transformB = keyB.boneTransforms[i];

			XMVECTOR positionA{};
			XMVECTOR rotationA{};
			XMVECTOR scaleA{};
			XMMatrixDecompose(&scaleA, &rotationA, &positionA, XMLoadFloat4x4(&transformA));

			XMVECTOR positionB{};
			XMVECTOR rotationB{};
			XMVECTOR scaleB{};
			XMMatrixDecompose(&scaleB, &rotationB, &positionB, XMLoadFloat4x4(&transformB));

			XMVECTOR positionLerp{ XMVectorLerp(positionA, positionB, BlendFactor) };
			XMVECTOR rotationLerp{ XMQuaternionSlerp(rotationA, rotationB, BlendFactor) };
			XMVECTOR scaleLerp{ XMVectorLerp(scaleA, scaleB, BlendFactor) };
			XMStoreFloat4x4(&m_Transforms[i], XMMatrixAffineTransformation(scaleLerp, XMVECTOR(), rotationLerp, positionLerp));
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//TODO_W7_()
		//Set m_ClipSet to false
		//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
		//If found,
		//	Call SetAnimation(Animation Clip) with the found clip
		//Else
		//	Call Reset
		//	Log a warning with an appropriate message
	m_ClipSet = false;
	for (size_t i = 0; i < m_pMeshFilter->m_AnimationClips.size(); i++)
	{
		if (m_pMeshFilter->m_AnimationClips[i].name == clipName)
		{
			SetAnimation(m_pMeshFilter->m_AnimationClips[i]);
		}
		else
		{
			Reset();
			Logger::LogWarning(L"cant set animation of clipname: " + clipName);
		}
	}
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//TODO_W7_()
		//Set m_ClipSet to false
		//Check if clipNumber is smaller than the actual m_AnimationClips vector size
		//If not,
			//	Call Reset
			//	Log a warning with an appropriate message
			//	return
		//else
			//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
			//	Call SetAnimation(AnimationClip clip)
	m_ClipSet = false;
	if (clipNumber >= m_pMeshFilter->m_AnimationClips.size())
	{
		Reset();
		Logger::LogWarning(L"cant set animation of clipNumber: " + clipNumber);
		return;
	}
	else
	{
		auto currentClip = GetClip(clipNumber);
		SetAnimation(currentClip);
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//TODO_W7_()
		//Set m_ClipSet to true
		//Set m_CurrentClip

		//Call Reset(false)

	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//TODO_W7_()
		//If pause is true, set m_IsPlaying to false

		//Set m_TickCount to zero
		//Set m_AnimationSpeed to 1.0f

		//If m_ClipSet is true
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		//Else
		//	Create an IdentityMatrix 
		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
	if (pause)
	{
		m_IsPlaying = false;
	}

	m_TickCount = 0;
	m_AnimationSpeed = 1.f;

	if (m_ClipSet == true)
	{
		auto boneTranforms = m_CurrentClip.keys[0].boneTransforms;
		m_Transforms.assign(boneTranforms.begin(), boneTranforms.end());
	}
	else
	{
		XMFLOAT4X4 identity{};
		XMStoreFloat4x4(&identity, XMMatrixIdentity());
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}
