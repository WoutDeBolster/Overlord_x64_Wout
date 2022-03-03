#pragma once
class MeshFilterLoader : public ContentLoader<MeshFilter>
{
public:
	MeshFilterLoader() = default;
	~MeshFilterLoader() override = default;
	MeshFilterLoader(const MeshFilterLoader& other) = delete;
	MeshFilterLoader(MeshFilterLoader&& other) noexcept = delete;
	MeshFilterLoader& operator=(const MeshFilterLoader& other) = delete;
	MeshFilterLoader& operator=(MeshFilterLoader&& other) noexcept = delete;

protected:
	MeshFilter* LoadContent(const ContentLoadInfo& loadInfo) override;
	void Destroy(MeshFilter* objToDestroy) override;

private:
	enum class MeshDataType
	{
		END = 0,
		HEADER = 1,
		POSITIONS = 2,
		INDICES = 3,
		NORMALS = 4,
		BINORMALS = 5,
		TANGENTS = 6,
		COLORS = 7,
		TEXCOORDS = 8,
		BLENDINDICES = 9,
		BLENDWEIGHTS = 10,
		ANIMATIONCLIPS = 11,
		SKELETON = 12
	};
};
