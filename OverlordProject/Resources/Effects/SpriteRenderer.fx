float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
    DepthEnable = FALSE;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
    uint TextureId : TEXCOORD0; //Can be ignored
    float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
    float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
    float4 Color : COLOR;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
    return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
    if (rotation != 0)
    {
		//Step 3.
		//Do rotation calculations
		//Transform to origin
		float2 posOrigin = pos.xy - offset - pivotOffset;
		// rotate
		pos.x = (posOrigin.x * rotCosSin.x) - (posOrigin.y * rotCosSin.y);
		pos.y = (posOrigin.y * rotCosSin.x) + (posOrigin.x * rotCosSin.y);
		//Retransform to initial position
		pos.xy += offset;
    }
    else
    {
		//Step 2.
		//No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
		//Just apply the pivot offset
		pos.xy -= pivotOffset;
    }

	//Geometry Vertex Output
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Given Data (Vertex Data)
    float3 position = float3(vertex[0].TransformData.x, vertex[0].TransformData.y, vertex[0].TransformData.z); //Extract the position data from the VS_DATA vertex struct
    float2 offset = float2(vertex[0].TransformData.x, vertex[0].TransformData.y); //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
    float rotation = vertex[0].TransformData.w; //Extract the rotation data from the VS_DATA vertex struct
    float2 pivot = float2(vertex[0].TransformData2.x, vertex[0].TransformData2.y); //Extract the pivot data from the VS_DATA vertex struct
    float2 scale = float2(vertex[0].TransformData2.z, vertex[0].TransformData2.w); //Extract the scale data from the VS_DATA vertex struct
    float2 texCoord = float2(vertex[0].TextureId, vertex[0].TextureId); //Initial Texture Coordinate
	float4 color = float4(vertex[0].Color.x, vertex[0].Color.y, vertex[0].Color.z, vertex[0].Color.w);
	float2 pivotOffset = pivot * (gTextureSize * scale);
	float2 rotCosSin = float2(0 , 0);
	
	if( rotation != 0)
	{
		rotCosSin.x = cos(rotation);
		rotCosSin.y = sin(rotation);
	}
	//...

	// LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB

	//VERTEX 1 [LT]
	float3 vertexPosLT = position;
	float2 texCoordLT = texCoord;
    CreateVertex(triStream, vertexPosLT, color, texCoordLT, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//VERTEX 2 [RT]
	float3 vertexPosRT = vertexPosLT + float3(gTextureSize.x * scale.x, 0, 0);
	float2 texCoordRT = texCoordLT + float2(1, 0);
    CreateVertex(triStream, vertexPosRT, color, texCoordRT, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!

	//vertex 3 [LB]
	float3 vertexPosLB = vertexPosLT + float3(0, gTextureSize.y * scale.y, 0);
	float2 texCoordLB = texCoordLT + float2(0, 1);
    CreateVertex(triStream, vertexPosLB, color, texCoordLB, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!
	
	//VERTEX 4 [RB]
	float3 vertexPosRB = vertexPosLT + float3(gTextureSize.x * scale.x, gTextureSize.y * scale.y, 0);
	float2 texCoordRB = texCoordLT + float2(1, 1);
    CreateVertex(triStream, vertexPosRB, color, texCoordRB, rotation, rotCosSin, offset, pivotOffset); //Change the color data too!
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;
}

// Default Technique
technique10 Default
{
    pass p0
    {
        SetRasterizerState(BackCulling);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		//SetDepthStencilState(NoDepth,0);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, MainGS()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}
