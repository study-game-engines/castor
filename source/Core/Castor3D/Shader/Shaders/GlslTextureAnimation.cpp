#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"

#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

namespace castor3d
{
	namespace shader
	{
		TextureAnimations::TextureAnimations( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable )
			: BufferT< TextureAnimData >{ writer
				, "C3D_TextureAnimations"
				, "c3d_textureAnimations"
				, binding
				, set
				, enable }
		{
		}
	}
}
