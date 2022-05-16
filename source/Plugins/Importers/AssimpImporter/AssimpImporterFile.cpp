#include "AssimpImporter/AssimpImporterFile.hpp"

#include "AssimpImporter/AssimpAnimationImporter.hpp"
#include "AssimpImporter/AssimpLightImporter.hpp"
#include "AssimpImporter/AssimpMaterialImporter.hpp"
#include "AssimpImporter/AssimpMeshImporter.hpp"
#include "AssimpImporter/AssimpSceneNodeImporter.hpp"
#include "AssimpImporter/AssimpSkeletonImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/material.h>
#include <assimp/postprocess.h>
#pragma warning( pop )

namespace c3d_assimp
{
	//*********************************************************************************************

	namespace file
	{
		static aiScene const * loadScene( Assimp::Importer & importer
			, castor::Path const & filePath
			, castor3d::Parameters const & parameters )
		{
			bool noOptim = false;
			auto found = parameters.get( "no_optimisations", noOptim );
			uint32_t importFlags{ aiProcess_ValidateDataStructure };

			if ( !found || !noOptim )
			{
				importFlags |= aiProcess_Triangulate
					| aiProcess_JoinIdenticalVertices
					| aiProcess_OptimizeMeshes
					| aiProcess_OptimizeGraph
					| aiProcess_FixInfacingNormals
					| aiProcess_LimitBoneWeights;
			}

			importer.SetPropertyInteger( AI_CONFIG_PP_LBW_MAX_WEIGHTS, 8 ); //< Limit to 8 bone weights
			importer.SetPropertyInteger( AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0 ); //< Get rid of $AssimpFbx$_PreRotation nodes
			bool tangentSpace = false;
			castor::String normals;

			if ( parameters.get( cuT( "normals" ), normals ) )
			{
				if ( normals == cuT( "smooth" ) )
				{
					importFlags |= aiProcess_GenSmoothNormals;
				}
			}

			if ( parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
			{
				importFlags |= aiProcess_CalcTangentSpace;
			}

			try
			{
				auto result = importer.ReadFile( castor::string::stringCast< char >( filePath ), importFlags );

				if ( !result )
				{
					castor3d::log::error << "Scene loading failed : " << importer.GetErrorString() << std::endl;
				}

				return result;
			}
			catch ( std::exception & exc )
			{
				castor3d::log::error << "Scene loading failed : " << exc.what() << std::endl;
			}

			return nullptr;
		}

		castor::String getMaterialName( AssimpImporterFile const & file
			, aiMaterial const & aiMaterial
			, uint32_t materialIndex )
		{
			castor::String result;
			aiString name;

			if ( aiMaterial.Get( AI_MATKEY_NAME, name ) == aiReturn_SUCCESS )
			{
				result += makeString( name );
			}
			else
			{
				result += file.getName() + cuT( "-" ) + castor::string::toString( materialIndex );
			}

			return result;
		}

		static std::map< aiAnimation const *, aiNodeAnim const * > findNodeAnims( aiNode const & aiNode
			, castor::ArrayView< aiAnimation * > const & animations )
		{
			std::map< aiAnimation const *, aiNodeAnim const * > result;

			for ( auto aiAnimation : animations )
			{
				auto channels = castor::makeArrayView( aiAnimation->mChannels, aiAnimation->mNumChannels );
				auto it = std::find_if( channels.begin()
					, channels.end()
					, [&aiNode]( aiNodeAnim const * lookup )
					{
						return lookup->mNodeName == aiNode.mName;
					} );

				if ( it != channels.end() )
				{
					result.emplace( aiAnimation, *it );
				}
			}

			return result;
		}

		static aiNode const * findRootSkeletonNode( aiNode const & sceneRootNode
			, castor::ArrayView< aiBone * > bones
			, aiNode const * meshNode
			, std::map< castor::String, castor::Matrix4x4f > const & bonesNodes )
		{
			std::vector< aiNode const * > bonesRootNodes;
			auto insertNode = [&bonesRootNodes]( aiNode const * node )
			{
				if ( std::all_of( bonesRootNodes.begin()
					, bonesRootNodes.end()
					, [node]( aiNode const * lookup )
					{
						return lookup->FindNode( node->mName ) == nullptr;
					} ) )
				{
					std::erase_if( bonesRootNodes
						, [node]( aiNode const * lookup )
						{
							return node->FindNode( lookup->mName ) != nullptr;
						} );
					bonesRootNodes.push_back( node );
				}
			};

			for ( auto bone : bones )
			{
				auto node = sceneRootNode.FindNode( bone->mName );
				insertNode( node );

				while ( node->mParent )
				{
					node = node->mParent;

					if ( node == meshNode
						|| node->FindNode( meshNode->mName ) )
					{
						break;
					}

					insertNode( node );
				}
			}

			CU_Require( bonesRootNodes.size() == 1u );
			return *bonesRootNodes.begin();
		}

		static bool isAnimForSkeleton( AssimpImporterFile const & file
			, aiAnimation const & animation
			, SkeletonData const & skeleton )
		{
			auto channels = castor::makeArrayView( animation.mChannels, animation.mNumChannels );
			return // The skeleton root node is in the animated channels
				( channels.end() != std::find_if( channels.begin()
					, channels.end()
					, [&skeleton]( aiNodeAnim const * animLookup )
					{
						return skeleton.rootNode->FindNode( animLookup->mNodeName ) != nullptr;
					} ) );
		}

		static std::map< castor::String, aiMeshMorphAnim const * > findMorphAnims( uint32_t aiMeshIndex
			, uint32_t aiNumAnimMeshes
			, aiNode const & rootNode
			, castor::ArrayView< aiAnimation * > animations )
		{
			std::map< castor::String, aiMeshMorphAnim const * > result;

			for ( auto anim : animations )
			{
				auto morphChannels = castor::makeArrayView( anim->mMorphMeshChannels, anim->mNumMorphMeshChannels );
				auto morphIt = std::find_if( morphChannels.begin()
					, morphChannels.end()
					, [aiNumAnimMeshes, aiMeshIndex, &rootNode]( aiMeshMorphAnim const * morphChannel )
					{
						auto node = rootNode.FindNode( morphChannel->mName );
						bool res = node != nullptr
							&& morphChannel->mNumKeys > 0u
							&& morphChannel->mKeys->mNumValuesAndWeights == aiNumAnimMeshes
							&& morphChannel->mKeys[morphChannel->mNumKeys - 1u].mTime > 0.0;

						if ( res )
						{
							auto meshes = castor::makeArrayView( node->mMeshes, node->mNumMeshes );
							res = meshes.end() != std::find( meshes.begin()
								, meshes.end()
								, aiMeshIndex );
						}

						return res;
					} );

				if ( morphIt != morphChannels.end() )
				{
					result.emplace( makeString( anim->mName ), *morphIt );
				}
			}

			return result;
		}

		auto findNodeMesh( uint32_t meshIndex
			, std::map< castor::String, MeshData > const & meshes )
		{
			return std::find_if( meshes.begin()
				, meshes.end()
				, [&meshIndex]( std::map< castor::String, MeshData >::value_type const & lookup )
				{
					return lookup.second.submeshes.end() != std::find_if( lookup.second.submeshes.begin()
						, lookup.second.submeshes.end()
						, [&meshIndex]( SubmeshData const & submesh )
						{
							return submesh.meshIndex == meshIndex;
						} );
				} );
		}

		aiNode const * findMeshNode( uint32_t meshIndex
			, aiNode const & node )
		{
			auto meshes = castor::makeArrayView( node.mMeshes, node.mNumMeshes );
			auto meshIt = std::find( meshes.begin(), meshes.end(), meshIndex );

			if ( meshIt != meshes.end() )
			{
				return &node;
			}

			aiNode const * result{};
			auto children = castor::makeArrayView( node.mChildren, node.mNumChildren );
			auto childIt = children.begin();

			while ( !result && childIt != children.end() )
			{
				result = findMeshNode( meshIndex, **childIt );
				++childIt;
			}

			return result;
		}
	}

	//*********************************************************************************************

	castor::String const AssimpImporterFile::Name = cuT( "ASSIMP Importer" );

	AssimpImporterFile::AssimpImporterFile( castor3d::Engine & engine
		, castor::Path const & path
		, castor3d::Parameters const & parameters )
		: castor3d::ImporterFile{ engine, path, parameters }
		, m_scene{ file::loadScene( m_importer, getFileName(), getParameters() ) }
	{
		if ( m_scene )
		{
			for ( auto aiMesh : castor::makeArrayView( m_scene->mMeshes, m_scene->mNumMeshes ) )
			{
				for ( auto aiBone : castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones ) )
				{
					m_bonesNodes.emplace( makeString( aiBone->mName )
						, fromAssimp( aiBone->mOffsetMatrix ) );
				}
			}

			doPrelistMaterials();
			doPrelistLights();
			doPrelistMeshes( doPrelistSkeletons() );
			doPrelistSceneNodes();
		}
	}

	castor::String AssimpImporterFile::getMaterialName( uint32_t materialIndex )const
	{
		return file::getMaterialName( *this
			, *m_scene->mMaterials[materialIndex]
			, materialIndex );
	}

	NodeAnimations const & AssimpImporterFile::getNodesAnimations( castor3d::SceneNode const & node )const
	{
		auto it = m_sceneData.nodes.find( node.getName() );

		if ( it != m_sceneData.nodes.end() )
		{
			return it->second.anims;
		}

		static NodeAnimations const dummy;
		return dummy;
	}

	SkeletonAnimations const & AssimpImporterFile::getSkeletonsAnimations( castor3d::Skeleton const & skeleton )const
	{
		auto name = skeleton.getName();

		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
		{
			name = getSkeletons().begin()->first;
		}

		auto it = m_sceneData.skeletons.find( name );

		if ( it != m_sceneData.skeletons.end() )
		{
			return it->second.anims;
		}

		static SkeletonAnimations const dummy;
		return dummy;
	}

	MeshAnimations const & AssimpImporterFile::getMeshesAnimations( castor3d::Mesh const & mesh
		, uint32_t submeshIndex )const
	{
		auto it = m_sceneData.meshes.find( mesh.getName() );

		if ( it != m_sceneData.meshes.end()
			&& submeshIndex < it->second.submeshes.size() )
		{
			return it->second.submeshes[submeshIndex].anims;
		}

		static MeshAnimations const dummy;
		return dummy;
	}

	std::vector< castor::String > AssimpImporterFile::listMaterials()
	{
		std::vector< castor::String > result;

		for ( auto materials : m_sceneData.materials )
		{
			result.emplace_back( materials.first );
		}

		return result;
	}

	std::vector< castor::String > AssimpImporterFile::listMeshes()
	{
		m_listedMeshes.clear();
		std::vector< castor::String > result;

		for ( auto it : m_sceneData.meshes )
		{
			m_listedMeshes.emplace_back( it.first );
			result.emplace_back( it.first );
		}

		return result;
	}

	std::vector< castor::String > AssimpImporterFile::listSkeletons()
	{
		std::vector< castor::String > result;

		for ( auto it : m_sceneData.skeletons )
		{
			m_listedSkeletons.emplace_back( it.first );
			result.emplace_back( it.first );
		}

		return result;
	}

	std::vector< castor::String > AssimpImporterFile::listSceneNodes()
	{
		std::vector< castor::String > result;

		for ( auto & node : m_sceneData.nodes )
		{
			result.emplace_back( node.first );
		}

		return result;
	}

	std::vector< std::pair< castor::String, castor3d::LightType > > AssimpImporterFile::listLights()
	{
		std::vector< std::pair< castor::String, castor3d::LightType > > result;

		for ( auto & light : m_sceneData.lights )
		{
			result.emplace_back( light.first
				, ( light.second->mType == aiLightSource_DIRECTIONAL
					? castor3d::LightType::eDirectional
					: ( light.second->mType == aiLightSource_POINT
						? castor3d::LightType::ePoint
						: castor3d::LightType::eSpot ) ) );
		}

		return result;
	}

	std::vector< castor::String > AssimpImporterFile::listMeshAnimations( castor3d::Mesh const & mesh )
	{
		std::set< castor::String > result;
		auto it = m_sceneData.meshes.find( mesh.getName() );

		if ( it != m_sceneData.meshes.end() )
		{
			for ( auto & submesh : it->second.submeshes )
			{
				for ( auto & anim : submesh.anims )
				{
					result.insert( anim.first );
				}
			}
		}

		return std::vector< castor::String >{ result.begin()
			, result.end() };
	}

	std::vector< castor::String > AssimpImporterFile::listSkeletonAnimations( castor3d::Skeleton const & skeleton )
	{
		auto name = skeleton.getName();

		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
		{
			name = getSkeletons().begin()->first;
		}

		std::vector< castor::String > result;
		auto it = m_sceneData.skeletons.find( name );

		if ( it != m_sceneData.skeletons.end() )
		{
			for ( auto & anim : it->second.anims )
			{
				result.push_back( anim.first );
			}
		}

		return result;
	}

	std::vector< castor::String > AssimpImporterFile::listSceneNodeAnimations( castor3d::SceneNode const & node )
	{
		std::vector< castor::String > result;
		auto it = m_sceneData.nodes.find( node.getName() );

		if ( it != m_sceneData.nodes.end() )
		{
			for ( auto & anim : it->second.anims )
			{
				result.push_back( anim.first );
			}
		}

		return result;
	}

	castor3d::MaterialImporterUPtr AssimpImporterFile::createMaterialImporter()
	{
		return std::make_unique< AssimpMaterialImporter >( *getOwner() );
	}

	castor3d::AnimationImporterUPtr AssimpImporterFile::createAnimationImporter()
	{
		return std::make_unique< AssimpAnimationImporter >( *getOwner() );
	}

	castor3d::SkeletonImporterUPtr AssimpImporterFile::createSkeletonImporter()
	{
		return std::make_unique< AssimpSkeletonImporter >( *getOwner() );
	}

	castor3d::MeshImporterUPtr AssimpImporterFile::createMeshImporter()
	{
		return std::make_unique< AssimpMeshImporter >( *getOwner() );
	}

	castor3d::SceneNodeImporterUPtr AssimpImporterFile::createSceneNodeImporter()
	{
		return std::make_unique< AssimpSceneNodeImporter >( *getOwner() );
	}

	castor3d::LightImporterUPtr AssimpImporterFile::createLightImporter()
	{
		return std::make_unique< AssimpLightImporter >( *getOwner() );
	}

	castor3d::ImporterFileUPtr AssimpImporterFile::create( castor3d::Engine & engine
		, castor::Path const & path
		, castor3d::Parameters const & parameters )
	{
		return std::make_unique< AssimpImporterFile >( engine, path, parameters );
	}

	void AssimpImporterFile::doPrelistMaterials()
	{
		uint32_t materialIndex = 0u;

		for ( auto aiMaterial : castor::makeArrayView( m_scene->mMaterials, m_scene->mNumMaterials ) )
		{
			auto name = file::getMaterialName( *this, *aiMaterial, materialIndex );
			m_sceneData.materials.emplace( name, aiMaterial );
			++materialIndex;
		}
	}

	void AssimpImporterFile::doPrelistLights()
	{
		for ( auto aiLight : castor::makeArrayView( m_scene->mLights, m_scene->mNumLights ) )
		{
			if ( aiLight->mType == aiLightSource_DIRECTIONAL
				|| aiLight->mType == aiLightSource_POINT
				|| aiLight->mType == aiLightSource_SPOT )
			{
				castor::String name = getInternalName( aiLight->mName );
				m_sceneData.lights.emplace( name, aiLight );

				auto position = castor::Point3f{};
				auto orientation = castor::Quaternion::identity();

				if ( aiLight->mType == aiLightSource_DIRECTIONAL
					|| aiLight->mType == aiLightSource_SPOT )
				{
					auto direction = castor::point::getNormalised( fromAssimp( aiLight->mDirection ) );
					auto up = castor::point::getNormalised( fromAssimp( aiLight->mUp ) );
					orientation = castor::Quaternion::fromMatrix( fromAssimp( direction, up ) );
				}

				if ( aiLight->mType != aiLightSource_DIRECTIONAL )
				{
					position = fromAssimp( aiLight->mPosition );
				}

				auto transform = castor::Matrix4x4f{ 1.0f };
				castor::matrix::setTransform( transform
					, position
					, castor::Point3f{ 1.0, 1.0, 1.0 }
				, orientation );
				m_sceneData.nodes.emplace( name
					, NodeData{ castor::String{}
					, nullptr
					, transform } );
			}
		}
	}

	std::map< aiMesh const *, aiNode const * > AssimpImporterFile::doPrelistSkeletons()
	{
		std::map< aiMesh const *, aiNode const * > result;
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : castor::makeArrayView( m_scene->mMeshes, m_scene->mNumMeshes ) )
		{
			if ( aiMesh->HasBones() )
			{
				auto meshNode = file::findMeshNode( meshIndex, *m_scene->mRootNode );

				if ( meshNode == nullptr )
				{
					CU_Failure( "Could not find mesh' node ?" );
				}
				else
				{
					auto rootNode = file::findRootSkeletonNode( *m_scene->mRootNode
						, castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones )
						, meshNode
						, m_bonesNodes );
					auto skelName = normalizeName( makeString( rootNode->mName ) );
					auto & skeleton = m_sceneData.skeletons.emplace( skelName, SkeletonData{ rootNode } ).first->second;

					for ( auto aiAnimation : castor::makeArrayView( m_scene->mAnimations, m_scene->mNumAnimations ) )
					{
						if ( file::isAnimForSkeleton( *this, *aiAnimation, skeleton ) )
						{
							auto [frameCount, frameTicks] = getAnimationFrameTicks( *aiAnimation );

							if ( frameCount > 1 )
							{
								castor::String animName{ normalizeName( makeString( aiAnimation->mName ) ) };

								if ( animName.empty() )
								{
									animName = normalizeName( getName() );
								}

								skeleton.anims.emplace( animName, aiAnimation );
							}
						}
					}

					result.emplace( aiMesh, rootNode );
				}
			}

			++meshIndex;
		}

		return result;
	}

	void AssimpImporterFile::doPrelistMeshes( std::map< aiMesh const *, aiNode const * > const & meshSkeletons )
	{
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : castor::makeArrayView( m_scene->mMeshes, m_scene->mNumMeshes ) )
		{
			auto faces = castor::makeArrayView( aiMesh->mFaces, aiMesh->mNumFaces );
			auto count = uint32_t( std::count_if( faces.begin()
				, faces.end()
				, []( aiFace const & face )
				{
					return face.mNumIndices == 3
						|| face.mNumIndices == 4;
				} ) );

			if ( count > 0 && aiMesh->HasPositions() )
			{
				castor::String meshName = normalizeName( getInternalName( aiMesh->mName ) );

				if ( meshName.size() > 150u )
				{
					meshName = getInternalName( getName() );
				}

				auto regIt = m_sceneData.meshes.find( meshName );
				aiNode const * skelNode{};

				if ( regIt == m_sceneData.meshes.end() )
				{
					auto it = meshSkeletons.find( aiMesh );

					if ( it != meshSkeletons.end() )
					{
						skelNode = it->second;
					}

					regIt = std::find_if( m_sceneData.meshes.begin()
						, m_sceneData.meshes.end()
						, [&skelNode]( std::map< castor::String, MeshData >::value_type const & lookup )
						{
							return skelNode == lookup.second.skelNode;
						} );
				}

				if ( regIt == m_sceneData.meshes.end() )
				{
					regIt = m_sceneData.meshes.emplace( meshName
						, MeshData{ skelNode } ).first;
				}

				regIt->second.submeshes.push_back( { aiMesh, meshIndex, {} } );

				if ( aiMesh->mNumAnimMeshes )
				{
					auto anims = file::findMorphAnims( meshIndex
						, aiMesh->mNumAnimMeshes
						, *m_scene->mRootNode
						, castor::makeArrayView( m_scene->mAnimations, m_scene->mNumAnimations ) );

					for ( auto anim : anims )
					{
						castor::String animName{ normalizeName( anim.first ) };
						regIt->second.submeshes.back().anims.emplace( animName
							, std::make_pair( aiMesh, anim.second ) );
					}
				}
			}

			++meshIndex;
		}
	}

	void AssimpImporterFile::doPrelistSceneNodes()
	{
		std::vector< NodeData > work;
		work.emplace_back( castor::String{}
			, m_scene->mRootNode
			, fromAssimp( m_scene->mRootNode->mTransformation ) );

		while ( !work.empty() )
		{
			auto nodeTransform = work.front();
			auto node = nodeTransform.node;
			auto transform = nodeTransform.transform;
			work.erase( work.begin() );

			if ( m_bonesNodes.find( makeString( node->mName ) ) != m_bonesNodes.end() )
			{
				continue;
			}

			castor::String nodeName;
			auto anims = file::findNodeAnims( *node
				, castor::makeArrayView( m_scene->mAnimations, m_scene->mNumAnimations ) );

			if ( node->mMeshes
				|| !anims.empty() )
			{
				nodeName = getInternalName( node->mName );
				auto & nodeData = m_sceneData.nodes.emplace( nodeName, nodeTransform ).first->second;

				for ( auto anim : anims )
				{
					auto [frameCount, frameTicks] = getNodeAnimFrameTicks( *anim.second );

					if ( frameCount > 1 )
					{
						castor::String animName{ normalizeName( makeString( anim.first->mName ) ) };

						if ( animName.empty() )
						{
							animName = normalizeName( makeString( node->mName ) );
						}

						nodeData.anims.emplace( animName, anim );
					}
				}

				for ( auto meshIndex : castor::makeArrayView( node->mMeshes, node->mNumMeshes ) )
				{
					auto it = file::findNodeMesh( meshIndex, m_sceneData.meshes );

					if ( it != m_sceneData.meshes.end() )
					{
						nodeData.meshes.push_back( &it->second );
					}
					else
					{
						CU_Failure( "Could not find node's mesh ?" );
					}
				}
			}
			else
			{
				nodeName = nodeTransform.parent;
				transform = transform * fromAssimp( node->mTransformation );
			}

			// continue for all child nodes
			if ( node->mChildren )
			{
				for ( auto aiChild : castor::makeArrayView( node->mChildren, node->mNumChildren ) )
				{
					work.emplace_back( nodeName, aiChild, transform );
				}
			}
		}
	}

	//*********************************************************************************************
}