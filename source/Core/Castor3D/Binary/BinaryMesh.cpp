#include "Castor3D/Binary/BinaryMesh.hpp"

#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Binary/BinarySubmesh.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Mesh >::doWrite( Mesh const & obj )
	{
		bool result = true;

		for ( auto & submesh : obj )
		{
			result = result && BinaryWriter< Submesh >{}.write( *submesh, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< Mesh >::Name = cuT( "Mesh" );

	bool BinaryParser< Mesh >::doParse( Mesh & obj )
	{
		bool result = true;
		SubmeshUPtr submesh{};
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmesh:
				submesh = castor::makeUnique< Submesh >( obj, obj.getSubmeshCount() );
				result = createBinaryParser< Submesh >().parse( *submesh, chunk );
				checkError( result, "Couldn't parse submesh." );

				if ( result )
				{
					obj.m_submeshes.push_back( std::move( submesh ) );
				}

				break;

			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< Mesh >::doParse_v1_2( Mesh & obj )
	{
		bool result = true;
		SkeletonRPtr skeleton;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeleton:
				skeleton = obj.getScene()->addNewSkeleton( obj.getName(), *obj.getScene() );
				result = createBinaryParser< Skeleton >().parse( *skeleton, chunk );
				checkError( result, "Couldn't parse skeleton." );

				if ( result )
				{
					obj.setSkeleton( skeleton );
				}

				break;

			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< Mesh >::doParse_v1_4( Mesh & obj )
	{
		bool result = true;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				// Name is ignored, since it's now set by the scene file
				break;

			default:
				break;
			}
		}

		return result;
	}
}
