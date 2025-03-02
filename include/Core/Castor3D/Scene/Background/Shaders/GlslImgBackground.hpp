/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslImgBackgroundModel_H___
#define ___C3D_GlslImgBackgroundModel_H___

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

namespace castor3d::shader
{
	class ImgBackgroundModel
		: public BackgroundModel
	{
	public:
		C3D_API ImgBackgroundModel( sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		C3D_API static BackgroundModelPtr create( Engine const & engine
			, sdw::ShaderWriter & writer
			, Utils & utils
			, VkExtent2D targetSize
			, bool needsForeground
			, uint32_t & binding
			, uint32_t set );

	public:
		static castor::String const Name;
	};
}

#endif
