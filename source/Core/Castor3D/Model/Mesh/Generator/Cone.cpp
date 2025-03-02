#include "Castor3D/Model/Mesh/Generator/Cone.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

CU_ImplementSmartPtr( castor3d, Cone )

namespace castor3d
{
	namespace cone
	{
		static void swapComponents( uint32_t lhs
			, uint32_t rhs
			, InterleavedVertexArray & vertices )
		{
			for ( auto & vtx : vertices )
			{
				std::swap( vtx.pos[lhs], vtx.pos[rhs] );
			}
		}
	}

	Cone::Cone()
		: MeshGenerator( cuT( "cone" ) )
		, m_nbFaces( 0 )
		, m_height( 0 )
		, m_radius( 0 )
	{
	}

	MeshGeneratorUPtr Cone::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Cone >();
	}

	void Cone::doGenerate( Mesh & mesh
		, Parameters const & parameters )
	{
		castor::String param;
		uint32_t axis = 1u;

		if ( parameters.get( cuT( "faces" ), param ) )
		{
			m_nbFaces = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			m_radius = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "height" ), param ) )
		{
			m_height = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "axis" ), param ) )
		{
			if ( param == "x" )
			{
				axis = 0u;
			}
			else if ( param == "z" )
			{
				axis = 2u;
			}
		}

		if ( m_nbFaces >= 2
			&& m_height > std::numeric_limits< float >::epsilon()
			&& m_radius > std::numeric_limits< float >::epsilon() )
		{
			Submesh & submeshBase = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			Submesh & submeshSide = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			//CALCUL DE LA POSITION DES POINTS
			float const dalpha = castor::PiMult2< float > / float( m_nbFaces );
			uint32_t i = 0;
			InterleavedVertexArray baseVertex;
			InterleavedVertexArray sideVertex;
			baseVertex.reserve( size_t( m_nbFaces ) + 1u );
			sideVertex.reserve( ( size_t( m_nbFaces ) + 1u ) * 2u );

			for ( float alpha = 0; i <= m_nbFaces; alpha += dalpha )
			{
				auto rCos = cos( alpha );
				auto rSin = sin( alpha );

				if ( i < m_nbFaces )
				{
					baseVertex.push_back( InterleavedVertex{}
						.position( castor::Point3f{ m_radius * rCos, 0.0, m_radius * rSin } )
						.texcoord( castor::Point2f{ ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 } ) );
				}

				sideVertex.push_back( InterleavedVertex{}
					.position( castor::Point3f{ m_radius * rCos, 0.0, m_radius * rSin } )
					.texcoord( castor::Point2f{ float( i ) / float( m_nbFaces ), float( 1.0 ) } ) );
				sideVertex.push_back( InterleavedVertex{}
					.position( castor::Point3f{ float( 0 ), m_height, float( 0 ) } )
					.texcoord( castor::Point2f{ float( i ) / float( m_nbFaces ), float( 0.0 ) } ) );
				i++;
			}

			if ( axis != 1u )
			{
				cone::swapComponents( axis, 1u, baseVertex );
				cone::swapComponents( axis, 1u, sideVertex );
			}
			
			baseVertex.push_back( InterleavedVertex{}
				.position( castor::Point3f{ 0.0, 0.0, 0.0 } )
				.texcoord( castor::Point2f{ 0.5, 0.5 } ) );
			auto bottomCenterIndex = uint32_t( baseVertex.size() - 1u );
			submeshBase.addPoints( baseVertex );
			submeshSide.addPoints( sideVertex );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			auto indexMappingBase = submeshBase.createComponent< TriFaceMapping >();
			auto indexMappingSide = submeshSide.createComponent< TriFaceMapping >();

			//Composition des extrémités
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				indexMappingBase->addFace( i + 1, i, bottomCenterIndex );
			}

			//Composition du bas
			indexMappingBase->addFace( 0, m_nbFaces - 1, bottomCenterIndex );

			//Composition des côtés
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				indexMappingSide->addFace( i + 1, i + 0, i + 2 );
			}

			indexMappingBase->computeNormals();
			indexMappingBase->computeTangents();
			indexMappingSide->computeNormals();
			indexMappingSide->computeTangents();

			// Join the first and last edge of the cone, tangent space wise.
			auto sideNormals = submeshSide.getComponent< NormalsComponent >();
			auto sideTangents = submeshSide.getComponent< TangentsComponent >();
			auto normal0Top = sideNormals->getData()[0];
			auto normal0Base = sideNormals->getData()[1];
			auto tangent0Top = sideTangents->getData()[0];
			auto tangent0Base = sideTangents->getData()[1];
			normal0Top += sideNormals->getData()[submeshSide.getPointsCount() - 2];
			normal0Base += sideNormals->getData()[submeshSide.getPointsCount() - 1];
			tangent0Top += sideTangents->getData()[submeshSide.getPointsCount() - 2];
			tangent0Base += sideTangents->getData()[submeshSide.getPointsCount() - 1];
			castor::point::normalise( normal0Top );
			castor::point::normalise( normal0Base );
			castor::point::normalise( tangent0Top );
			castor::point::normalise( tangent0Base );
			sideNormals->getData()[submeshSide.getPointsCount() - 2] = normal0Top;
			sideNormals->getData()[submeshSide.getPointsCount() - 1] = normal0Base;
			sideTangents->getData()[submeshSide.getPointsCount() - 2] = tangent0Top;
			sideTangents->getData()[submeshSide.getPointsCount() - 1] = tangent0Base;
		}
	}
}
