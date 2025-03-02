/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfImporterFile___
#define ___C3D_GltfImporterFile___

#include <Castor3D/ImporterFile.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#ifdef None
#	undef None
#endif
#pragma warning( disable: 4715 )
#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Gltf_API
#else
#	ifdef GltfImporter_EXPORTS
#		define C3D_Gltf_API __declspec(dllexport)
#	else
#		define C3D_Gltf_API __declspec(dllimport)
#	endif
#endif

namespace fastgltf
{
	template<>
	struct ElementTraits< castor::Point2ub > : ElementTraitsBase< uint8_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3ub > : ElementTraitsBase< uint8_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4ub > : ElementTraitsBase< uint8_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Point2us > : ElementTraitsBase< uint16_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3us > : ElementTraitsBase< uint16_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4us > : ElementTraitsBase< uint16_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Point2ui > : ElementTraitsBase< uint32_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3ui > : ElementTraitsBase< uint32_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4ui > : ElementTraitsBase< uint32_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Point2f > : ElementTraitsBase< float, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3f > : ElementTraitsBase< float, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4f > : ElementTraitsBase< float, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Quaternion > : ElementTraitsBase< float, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Matrix4x4f > : ElementTraitsBase< float, AccessorType::Mat4 > {};
}

namespace c3d_gltf
{
	inline const castor::String DefaultMaterial = cuT( "GLTF_DefaultMaterial" );

	castor3d::NodeTransform convert( std::variant< fastgltf::Node::TRS, fastgltf::Node::TransformMatrix > const & transform );
	castor::Point3f convert( std::array< float, 3u > const & value );
	castor::Quaternion convert( std::array< float, 4u > const & value );

	using AnimationChannelSampler = std::pair< fastgltf::AnimationChannel, fastgltf::AnimationSampler >;
	using NodeAnimationChannelSampler = std::vector< AnimationChannelSampler >;
	using AnimationChannelSamplers = std::map< fastgltf::AnimationPath, NodeAnimationChannelSampler >;
	using Animations = std::map< castor::String, AnimationChannelSamplers >;

	using IndexName = std::pair< size_t, castor::String >;
	using NameContainer = std::vector< IndexName >;

	struct GltfSubmeshData
	{
		GltfSubmeshData( fastgltf::Mesh const * pmesh
			, uint32_t pmeshIndex )
			: mesh{ pmesh }
			, meshIndex{ pmeshIndex }
		{
		}

		fastgltf::Mesh const * mesh;
		uint32_t meshIndex;
		Animations anims;
	};

	struct GltfMeshData
	{
		GltfMeshData( fastgltf::Skin const * pskin
			, size_t pskinIndex )
			: skin{ pskin }
			, skinIndex{ pskinIndex }
		{
		}
		std::vector< GltfSubmeshData > submeshes;
		fastgltf::Skin const * skin{};
		size_t skinIndex{};
	};

	struct GltfNodeData
		: castor3d::ImporterFile::NodeData
	{
		GltfNodeData( castor::String pparent
			, castor::String pname
			, size_t pindex
			, size_t pinstance
			, size_t pinstanceCount
			, castor3d::NodeTransform ptransform
			, fastgltf::Node const * pnode )
			: NodeData{ std::move( pparent )
				, std::move( pname ) }
			, index{ pindex }
			, instance{ pinstance }
			, instanceCount{ pinstanceCount }
			, transform{ std::move( ptransform ) }
			, node{ pnode }
		{
		}

		size_t index;
		size_t instance;
		size_t instanceCount;
		castor3d::NodeTransform transform{};
		fastgltf::Node const * node;
		std::vector< GltfMeshData const * > meshes{};
		Animations anims;
	};

	struct GlSkeletonData
	{
		Animations anims;
	};

	struct GlSceneData
	{
		std::vector< GltfNodeData > nodes;
		std::vector< GltfNodeData > skeletonNodes;
		std::map< castor::String, GltfMeshData > meshes;
		std::map< castor::String, GlSkeletonData > skeletons;
	};

	class GltfImporterFile
		: public castor3d::ImporterFile
	{
	public:
		C3D_Gltf_API GltfImporterFile( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters
			, fastgltf::Category category = fastgltf::Category::All );

		static castor3d::ImporterFileUPtr create( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters );

		using castor3d::ImporterFile::getInternalName;

		castor::String getMaterialName( size_t index )const;
		castor::String getMeshName( size_t index )const;
		castor::String getNodeName( size_t index, size_t instance )const;
		castor::String getSkinName( size_t index )const;
		castor::String getLightName( size_t index )const;
		castor::String getSamplerName( size_t index )const;
		castor::String getGeometryName( size_t nodeIndex, size_t meshIndex, size_t instance )const;
		castor::String getAnimationName( size_t index )const;

		size_t getNodeIndex( castor::String const & name )const;
		size_t getSkeletonNodeIndex( castor::String const & name )const;
		size_t getMeshIndex( castor::String const & name, uint32_t submeshIndex )const;

		Animations getMeshAnimations( castor3d::Mesh const & mesh, uint32_t submeshIndex )const;
		Animations getSkinAnimations( castor3d::Skeleton const & skeleton )const;
		Animations getNodeAnimations( castor3d::SceneNode const & node )const;

		bool isSkeletonNode( size_t nodeIndex )const;

		std::vector< castor::String > listMaterials()override;
		std::vector< MeshData > listMeshes()override;
		std::vector< castor::String > listSkeletons()override;
		std::vector< NodeData > listSceneNodes()override;
		std::vector< LightData > listLights()override;
		std::vector< GeometryData > listGeometries()override;
		std::vector< castor::String > listMeshAnimations( castor3d::Mesh const & mesh )override;
		std::vector< castor::String > listSkeletonAnimations( castor3d::Skeleton const & skeleton )override;
		std::vector< castor::String > listSceneNodeAnimations( castor3d::SceneNode const & node )override;

		castor3d::MaterialImporterUPtr createMaterialImporter()override;
		castor3d::AnimationImporterUPtr createAnimationImporter()override;
		castor3d::SkeletonImporterUPtr createSkeletonImporter()override;
		castor3d::MeshImporterUPtr createMeshImporter()override;
		castor3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		castor3d::LightImporterUPtr createLightImporter()override;

		std::vector< GltfNodeData > const & getNodes()const noexcept
		{
			return m_sceneData.nodes;
		}

		fastgltf::Asset const & getAsset()const noexcept
		{
			return *m_asset;
		}

		bool isValid()const noexcept
		{
			return m_expAsset.error() == fastgltf::Error::None;
		}

		auto const & getMeshes()const
		{
			return m_sceneData.meshes;
		}

	public:
		static castor::String const Name;

	private:
		void doPrelistNodes();
		void doPrelistMeshes();
		void doAddNode( GltfNodeData nodeData
			, bool isParentSkeletonNode
			, size_t parentIndex
			, size_t & parentInstanceCount );

	private:
		fastgltf::Expected< fastgltf::Asset > m_expAsset;
		fastgltf::Asset const * m_asset{};
		std::vector< size_t > m_sceneIndices{};
		GlSceneData m_sceneData;
		mutable NameContainer m_materialNames;
		mutable NameContainer m_meshNames;
		mutable NameContainer m_nodeNames;
		mutable NameContainer m_skinNames;
		mutable NameContainer m_lightNames;
		mutable NameContainer m_samplerNames;
		mutable NameContainer m_animationNames;
	};
}

#endif
