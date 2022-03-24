#include "stdafx.h"
#include "SpikyMaterial.h"

SpikyMaterial::SpikyMaterial()
	: Material<SpikyMaterial>(L"Effects/SpikyShader.fx")
{
}

void SpikyMaterial::InitializeEffectVariables()
{
	SetVariable_Vector(L"gColorDiffuse", { XMFLOAT3(Colors::Red) });
	SetVariable_Scalar(L"gSpikeLength", 0.2f);
}
