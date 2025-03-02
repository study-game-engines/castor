#include "TextGeometry.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< Geometry >::TextWriter( String const & tabs )
		: TextWriterT< Geometry >{ tabs }
	{
	}

	bool TextWriter< Geometry >::operator()( Geometry const & geometry
		, StringStream & file )
	{
		bool result{ true };
		auto mesh = geometry.getMesh();

		if ( mesh )
		{
			log::info << tabs() << cuT( "Writing Geometry " ) << geometry.getName() << std::endl;
			result = false;

			if ( auto block{ beginBlock( file, cuT( "object" ), geometry.getName() ) } )
			{
				result = writeName( file, "parent", geometry.getParent()->getName() )
					&& writeOpt( file, cuT( "cast_shadows" ), geometry.isShadowCaster(), true )
					&& writeOpt( file, cuT( "receive_shadows" ), geometry.isShadowReceiver(), true )
					&& writeName( file, cuT( "mesh" ), mesh->getName() );

				if ( result )
				{
					auto it = std::find_if( mesh->begin()
						, mesh->end()
						, [&geometry]( SubmeshUPtr const & lookup )
						{
							return geometry.getMaterial( *lookup ) != lookup->getDefaultMaterial();
						} );

					if ( it != mesh->end() )
					{
						if ( mesh->getSubmeshCount() == 1 )
						{
							result = writeName( file, cuT( "material" ), geometry.getMaterial( *mesh->getSubmesh( 0u ) )->getName() );
						}
						else if ( auto matsBlock{ beginBlock( file, "materials" ) } )
						{
							for ( auto & submesh : *mesh )
							{
								auto material = geometry.getMaterial( *submesh );

								if ( material != submesh->getDefaultMaterial() )
								{
									result = result
										&& writeText( file, tabs() + cuT( "material " ) + string::toString( submesh->getId() ) + cuT( " \"" ) + material->getName() + cuT( "\"\n" ) );
								}
							}
						}
					}
				}
			}
		}

		return result;
	}
}
