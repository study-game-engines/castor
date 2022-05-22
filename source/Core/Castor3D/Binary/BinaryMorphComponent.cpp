#include "Castor3D/Binary/BinaryMorphComponent.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< MorphComponent >::doWrite( MorphComponent const & obj )
	{
		bool result = true;

		for ( auto & it : obj.getMorphTargetsBuffers() )
		{
			if ( result )
			{
				auto size = uint32_t( std::max( { it.positions.size()
					, it.normals.size()
					, it.tangents.size()
					, it.texcoords0.size()
					, it.texcoords1.size()
					, it.texcoords2.size()
					, it.texcoords3.size()
					, it.colours.size() } ) );
				result = doWriteChunk( size, ChunkType::eMorphTargetBufferSize, m_chunk );
			}

			if ( result
				&& !it.positions.empty() )
			{
				result = doWriteChunk( it.positions, ChunkType::eMorphTargetPositions, m_chunk );
			}

			if ( result
				&& !it.normals.empty() )
			{
				result = doWriteChunk( it.normals, ChunkType::eMorphTargetNormals, m_chunk );
			}

			if ( result
				&& !it.tangents.empty() )
			{
				result = doWriteChunk( it.tangents, ChunkType::eMorphTargetTangents, m_chunk );
			}

			if ( result
				&& !it.texcoords0.empty() )
			{
				result = doWriteChunk( it.texcoords0, ChunkType::eMorphTargetTexcoords0, m_chunk );
			}

			if ( result
				&& !it.texcoords1.empty() )
			{
				result = doWriteChunk( it.texcoords1, ChunkType::eMorphTargetTexcoords1, m_chunk );
			}

			if ( result
				&& !it.texcoords2.empty() )
			{
				result = doWriteChunk( it.texcoords2, ChunkType::eMorphTargetTexcoords2, m_chunk );
			}

			if ( result
				&& !it.texcoords3.empty() )
			{
				result = doWriteChunk( it.texcoords3, ChunkType::eMorphTargetTexcoords3, m_chunk );
			}

			if ( result
				&& !it.colours.empty() )
			{
				result = doWriteChunk( it.texcoords3, ChunkType::eMorphTargetColours, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< MorphComponent >::Name = cuT( "MorphComponent" );

	bool BinaryParser< MorphComponent >::doParse( MorphComponent & obj )
	{
		bool result = true;
		SubmeshAnimationBuffer buffer;
		uint32_t count{ 0u };
		std::set< MorphFlag > flags;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMorphTargetBufferSize:
				if ( count )
				{
					obj.addMorphTarget( std::move( buffer ) );
					count = 0u;
				}
				result = doParseChunk( count, chunk );
				checkError( result, "Couldn't parse keyframe buffers size." );
				break;
			case ChunkType::eMorphTargetPositions:
				flags.insert( MorphFlag::ePositions );
				buffer.positions.resize( count );
				result = doParseChunk( buffer.positions, chunk );
				checkError( result, "Couldn't parse keyframe positions." );
				break;
			case ChunkType::eMorphTargetNormals:
				flags.insert( MorphFlag::eNormals );
				buffer.normals.resize( count );
				result = doParseChunk( buffer.normals, chunk );
				checkError( result, "Couldn't parse keyframe normals." );
				break;
			case ChunkType::eMorphTargetTangents:
				flags.insert( MorphFlag::eTangents );
				buffer.tangents.resize( count );
				result = doParseChunk( buffer.tangents, chunk );
				checkError( result, "Couldn't parse keyframe tangents." );
				break;
			case ChunkType::eMorphTargetTexcoords0:
				flags.insert( MorphFlag::eTexcoords0 );
				buffer.texcoords0.resize( count );
				result = doParseChunk( buffer.texcoords0, chunk );
				checkError( result, "Couldn't parse keyframe texcoords0." );
				break;
			case ChunkType::eMorphTargetTexcoords1:
				flags.insert( MorphFlag::eTexcoords1 );
				buffer.texcoords1.resize( count );
				result = doParseChunk( buffer.texcoords1, chunk );
				checkError( result, "Couldn't parse keyframe texcoords1." );
				break;
			case ChunkType::eMorphTargetTexcoords2:
				flags.insert( MorphFlag::eTexcoords2 );
				buffer.texcoords2.resize( count );
				result = doParseChunk( buffer.texcoords2, chunk );
				checkError( result, "Couldn't parse keyframe texcoords2." );
				break;
			case ChunkType::eMorphTargetTexcoords3:
				flags.insert( MorphFlag::eTexcoords3 );
				buffer.texcoords3.resize( count );
				result = doParseChunk( buffer.texcoords3, chunk );
				checkError( result, "Couldn't parse keyframe texcoords3." );
				break;
			case ChunkType::eMorphTargetColours:
				flags.insert( MorphFlag::eColours );
				buffer.colours.resize( count );
				result = doParseChunk( buffer.colours, chunk );
				checkError( result, "Couldn't parse keyframe colours." );
				break;
			default:
				result = false;
				break;
			}
		}

		obj.m_targetDataCount = uint32_t( flags.size() );

		for ( auto & flag : flags )
		{
			obj.m_flags |= flag;
		}

		if ( result && count )
		{
			obj.addMorphTarget( std::move( buffer ) );
		}

		return result;
	}

	//*************************************************************************************************
}