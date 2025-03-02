#include "TextLpvConfig.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< LpvConfig >::TextWriter( String const & tabs )
		: TextWriterT< LpvConfig >{ tabs }
	{
	}

	bool TextWriter< LpvConfig >::operator()( LpvConfig const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing LpvConfig" ) << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "lpv_config" ) ) } )
		{
			result = write( file, cuT( "texel_area_modifier" ), object.texelAreaModifier );
		}

		return result;
	}
}
