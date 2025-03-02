#include "Castor3D/Binary/BinarySkeletonAnimationObject.hpp"

#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationBone.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationNode.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace binsklanmobj
	{
		template< typename T >
		struct KeyFrameT
		{
			castor::Milliseconds m_timeIndex{};
			castor::SquareMatrix< T, 4u > m_transform;
		};

		using KeyFrame = KeyFrameT< float >;
		using KeyFramed = std::array< double, 17 >;

		static void doConvert( std::vector< KeyFramed > const & in
			, std::vector< KeyFrame > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & kf : in )
			{
				size_t index{ 0u };
				castor::Milliseconds timeIndex{ int64_t( kf[index++] * 1000.0 ) };
				castor::Matrix4x4f transform{ &kf[index] };
				( *it ) = KeyFrame{ timeIndex, transform };
				++it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationObject >::doWrite( SkeletonAnimationObject const & obj )
	{
		bool result = true;

		for ( auto const & moving : obj.m_children )
		{
			switch ( moving->getType() )
			{
			case SkeletonNodeType::eNode:
				result = result && BinaryWriter< SkeletonAnimationNode >{}.write( static_cast< SkeletonAnimationNode const & >( *moving ), m_chunk );
				break;

			case SkeletonNodeType::eBone:
				result = result && BinaryWriter< SkeletonAnimationBone >{}.write( static_cast< SkeletonAnimationBone const & >( *moving ), m_chunk );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< SkeletonAnimationObject >::Name = cuT( "SkeletonAnimationObject" );

	bool BinaryParser< SkeletonAnimationObject >::doParse( SkeletonAnimationObject & obj )
	{
		bool result = true;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonAnimationBone:
				if ( m_fileVersion > Version{ 1, 5, 0 } )
				{
					auto bone = castor::makeUnique< SkeletonAnimationBone >( *obj.getOwner() );
					result = createBinaryParser< SkeletonAnimationBone >().parse( *bone, chunk );
					checkError( result, "Couldn't parse animation bone." );

					if ( result )
					{
						obj.addChild( bone.get() );
						obj.getOwner()->addObject( castor::ptrRefCast< SkeletonAnimationObject >( bone )
							, &obj );
					}
				}
				break;
			case ChunkType::eSkeletonAnimationNode:
				if ( m_fileVersion > Version{ 1, 5, 0 } )
				{
					auto node = castor::makeUnique< SkeletonAnimationNode >( *obj.getOwner() );
					result = createBinaryParser< SkeletonAnimationNode >().parse( *node, chunk );
					checkError( result, "Couldn't parse animation node." );

					if ( result )
					{
						obj.addChild( node.get() );
						obj.getOwner()->addObject( castor::ptrRefCast< SkeletonAnimationObject >( node )
							, &obj );
					}
				}
				break;
			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationObject >::doParse_v1_1( SkeletonAnimationObject & obj )
	{
		bool result = true;
		std::vector< binsklanmobj::KeyFrame > keyframes;
		std::vector< binsklanmobj::KeyFramed > keyframesd;
		BinaryChunk chunk{ doIsLittleEndian() };
		uint32_t count{ 0 };
		float length{ 0.0f };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			case ChunkType::eKeyframeCount:
#pragma GCC diagnostic pop
#pragma warning( pop )
				result = doParseChunk( count, chunk );
				checkError( result, "Couldn't parse keyframes count." );
				if ( result )
				{
					keyframesd.resize( count );
				}
				break;

#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			case ChunkType::eKeyframes:
#pragma GCC diagnostic pop
#pragma warning( pop )
				result = doParseChunk( keyframesd, chunk );
				checkError( result, "Couldn't parse keyframes." );
				if ( result )
				{
					doConvert( keyframesd, keyframes );
					auto & animation = *obj.getOwner();

					for ( auto & keyframe : keyframes )
					{
						auto it = animation.find( keyframe.m_timeIndex );

						if ( it == animation.end() )
						{
							animation.addKeyFrame( castor::makeUniqueDerived< AnimationKeyFrame, SkeletonAnimationKeyFrame >( *obj.getOwner()
								, keyframe.m_timeIndex ) );
							it = animation.find( keyframe.m_timeIndex );
						}

						auto & keyFrame = static_cast< SkeletonAnimationKeyFrame & >( **it );
						castor::Point3f translate;
						castor::Point3f scale;
						castor::Quaternion rotate;
						castor::matrix::decompose( keyframe.m_transform, translate, scale, rotate );
						keyFrame.addAnimationObject( obj
							, translate
							, rotate
							, scale );
					}

					for ( auto & keyFrame : animation )
					{
						keyFrame->initialise();
					}
				}
				break;
			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				checkError( result, "Couldn't parse animation length." );
				break;
			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationObject >::doParse_v1_5( SkeletonAnimationObject & obj )
	{
		BinaryChunk chunk{ doIsLittleEndian() };
		auto & skeleton = static_cast< Skeleton & >( *obj.getOwner()->getAnimable() );
		auto objNode = ( obj.getType() == SkeletonNodeType::eNode
			? static_cast< SkeletonAnimationNode const & >( obj ).getNode()
			: &static_cast< SkeletonNode & >( *static_cast< SkeletonAnimationBone const & >( obj ).getBone() ) );
		bool result = true;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMovingTransform:
				break;
			case ChunkType::eSkeletonAnimationBone:
				if ( m_fileVersion <= Version{ 1, 5, 0 } )
				{
					auto bone = castor::makeUnique< SkeletonAnimationBone >( *obj.getOwner() );
					result = createBinaryParser< SkeletonAnimationBone >().parse( *bone, chunk );
					checkError( result, "Couldn't parse animation bone." );

					if ( result )
					{
						obj.addChild( bone.get() );

						if ( !bone->getBone()->getParent() )
						{
							skeleton.setNodeParent( *bone->getBone(), *objNode );
						}

						obj.getOwner()->addObject( castor::ptrRefCast< SkeletonAnimationObject >( bone )
							, &obj );
					}
				}
				break;
			case ChunkType::eSkeletonAnimationNode:
				if ( m_fileVersion <= Version{ 1, 5, 0 } )
				{
					auto node = castor::makeUnique< SkeletonAnimationNode >( *obj.getOwner() );
					result = createBinaryParser< SkeletonAnimationNode >().parse( *node, chunk );
					checkError( result, "Couldn't parse animation node." );

					if ( result )
					{
						obj.addChild( node.get() );

						if ( !node->getNode()->getParent() )
						{
							skeleton.setNodeParent( *node->getNode(), *objNode );
						}

						obj.getOwner()->addObject( castor::ptrRefCast< SkeletonAnimationObject >( node )
							, &obj );
					}
				}
				break;
			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
