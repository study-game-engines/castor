/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextCamera_H___
#define ___CSE_TextCamera_H___

#include <Castor3D/Scene/Camera.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Camera >
		: public TextWriterT< castor3d::Camera >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::Camera const & camera
			, castor::StringStream & file )override;
	};
}

#endif
